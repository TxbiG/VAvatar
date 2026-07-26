#include "AudioTracking.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>
#include <mmreg.h>
#include <propsys.h>
#include <propvarutil.h>

namespace {
// Convert a Windows wide string into UTF-8 for UI/status text.
std::string WideStringToUtf8(const wchar_t* value)
{
    if (!value) {
        return {};
    }

    const int length = WideCharToMultiByte(CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);
    if (length <= 0) {
        return {};
    }

    std::string result(static_cast<size_t>(length - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value, -1, result.data(), length, nullptr, nullptr);
    return result;
}

// Read the friendly display name for a WASAPI capture device.
std::string ReadDeviceName(IMMDevice* device)
{
    IPropertyStore* properties = nullptr;
    if (!device || FAILED(device->OpenPropertyStore(STGM_READ, &properties))) {
        return "Default microphone";
    }

    PROPVARIANT name;
    PropVariantInit(&name);

    std::string result = "Default microphone";
    if (SUCCEEDED(properties->GetValue(PKEY_Device_FriendlyName, &name)) && name.vt == VT_LPWSTR) {
        result = WideStringToUtf8(name.pwszVal);
    }

    PropVariantClear(&name);
    properties->Release();
    return result;
}
}

AudioTracking::AudioTracking()
{
}

AudioTracking::~AudioTracking()
{
    stop();
}

bool AudioTracking::start()
{
    if (m_running.load() || m_starting.load()) {
        return true;
    }

    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }

    m_starting.store(true);
    m_stopRequested.store(false);
    m_captureThread = std::thread(&AudioTracking::captureLoop, this);
    return true;
}

void AudioTracking::stop()
{
    m_stopRequested.store(true);

    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }

    m_running.store(false);
    m_starting.store(false);
    m_isActivated.store(false);
    m_level.store(0.0f);
    m_smoothedLevel.store(0.0f);
    setStatus("Microphone stopped");
}

float AudioTracking::getMouthOpen() const
{
    const float threshold = std::max(m_threshold.load(), 0.0001f);
    const float normalized = (m_smoothedLevel.load() - threshold) / (threshold * 5.0f);
    return std::clamp(normalized, 0.0f, 1.0f);
}

void AudioTracking::setThreshold(float threshold)
{
    m_threshold.store(std::clamp(threshold, 0.001f, 0.25f));
}

std::string AudioTracking::getDeviceName() const
{
    std::lock_guard<std::mutex> lock(m_deviceNameMutex);
    return m_deviceName;
}

std::string AudioTracking::getStatus() const
{
    std::lock_guard<std::mutex> lock(m_deviceNameMutex);
    return m_status;
}

VoiceTrackingState AudioTracking::getState() const
{
    VoiceTrackingState state;
    state.available = m_running.load() || m_starting.load();
    state.enabled = m_running.load();
    state.speaking = m_isActivated.load();
    state.rawLevel = m_level.load();
    state.smoothedLevel = m_smoothedLevel.load();
    state.threshold = m_threshold.load();
    state.mouthOpen = getMouthOpen();
    state.deviceName = getDeviceName();
    return state;
}

void AudioTracking::captureLoop()
{
    const HRESULT coResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(coResult) && coResult != RPC_E_CHANGED_MODE) {
        setStatus("Could not initialize microphone COM");
        m_starting.store(false);
        return;
    }

    if (!initializeAudio()) {
        shutdownAudio();
        m_starting.store(false);
        if (SUCCEEDED(coResult)) {
            CoUninitialize();
        }
        return;
    }

    m_running.store(true);
    m_starting.store(false);
    setStatus("Microphone running");

    while (!m_stopRequested.load()) {
        captureAvailablePackets();
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    shutdownAudio();
    if (SUCCEEDED(coResult)) {
        CoUninitialize();
    }
}

bool AudioTracking::initializeAudio()
{
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&m_enumerator));
    if (FAILED(hr)) {
        setStatus("Could not create microphone device enumerator");
        return false;
    }

    hr = m_enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_device);
    if (FAILED(hr)) {
        setStatus("No default microphone available");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(m_deviceNameMutex);
        m_deviceName = ReadDeviceName(m_device);
    }

    hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&m_audioClient));
    if (FAILED(hr)) {
        setStatus("Could not activate microphone");
        return false;
    }

    hr = m_audioClient->GetMixFormat(&m_mixFormat);
    if (FAILED(hr)) {
        setStatus("Could not read microphone mix format");
        return false;
    }

    REFERENCE_TIME bufferDuration = 10000000 / 10;
    hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, bufferDuration, 0, m_mixFormat, nullptr);
    if (FAILED(hr)) {
        setStatus("Could not initialize microphone capture");
        return false;
    }

    hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&m_captureClient));
    if (FAILED(hr)) {
        setStatus("Could not get microphone capture service");
        return false;
    }

    hr = m_audioClient->Start();
    if (FAILED(hr)) {
        setStatus("Could not start microphone");
        return false;
    }

    return true;
}

void AudioTracking::shutdownAudio()
{
    if (m_audioClient) {
        m_audioClient->Stop();
    }

    if (m_mixFormat) {
        CoTaskMemFree(m_mixFormat);
        m_mixFormat = nullptr;
    }

    if (m_captureClient) {
        m_captureClient->Release();
        m_captureClient = nullptr;
    }

    if (m_audioClient) {
        m_audioClient->Release();
        m_audioClient = nullptr;
    }

    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_enumerator) {
        m_enumerator->Release();
        m_enumerator = nullptr;
    }

    m_running.store(false);
}

void AudioTracking::captureAvailablePackets()
{
    if (!m_captureClient || !m_mixFormat) {
        return;
    }

    UINT32 packetLength = 0;
    if (FAILED(m_captureClient->GetNextPacketSize(&packetLength))) {
        setStatus("Microphone disconnected or capture failed");
        m_stopRequested.store(true);
        return;
    }

    while (packetLength > 0) {
        BYTE* data = nullptr;
        UINT32 framesAvailable = 0;
        DWORD flags = 0;

        if (FAILED(m_captureClient->GetBuffer(&data, &framesAvailable, &flags, nullptr, nullptr))) {
            setStatus("Microphone packet read failed");
            m_stopRequested.store(true);
            return;
        }

        float rms = 0.0f;
        if ((flags & AUDCLNT_BUFFERFLAGS_SILENT) == 0 && data && framesAvailable > 0) {
            rms = calculateRms(data, framesAvailable, *m_mixFormat);
        }

        const float previous = m_smoothedLevel.load();
        const float smoothed = previous + (rms - previous) * 0.28f;
        m_level.store(rms);
        m_smoothedLevel.store(smoothed);
        m_isActivated.store(smoothed > m_threshold.load());

        m_captureClient->ReleaseBuffer(framesAvailable);

        if (FAILED(m_captureClient->GetNextPacketSize(&packetLength))) {
            setStatus("Microphone disconnected or capture failed");
            m_stopRequested.store(true);
            return;
        }
    }
}

void AudioTracking::setStatus(const std::string& status)
{
    std::lock_guard<std::mutex> lock(m_deviceNameMutex);
    m_status = status;
}

float AudioTracking::calculateRms(const BYTE* data, UINT32 frameCount, const WAVEFORMATEX& format) const
{
    if (!data || frameCount == 0 || format.nChannels == 0) {
        return 0.0f;
    }

    const UINT32 sampleCount = frameCount * format.nChannels;
    double sum = 0.0;

    bool isFloat = format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
    if (format.wFormatTag == WAVE_FORMAT_EXTENSIBLE && format.cbSize >= 22) {
        const auto& extensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE&>(format);
        isFloat = extensible.SubFormat.Data1 == WAVE_FORMAT_IEEE_FLOAT;
    }

    if (isFloat && format.wBitsPerSample == 32) {
        const float* samples = reinterpret_cast<const float*>(data);
        for (UINT32 i = 0; i < sampleCount; ++i) {
            sum += samples[i] * samples[i];
        }
    } else if (format.wBitsPerSample == 16) {
        const int16_t* samples = reinterpret_cast<const int16_t*>(data);
        for (UINT32 i = 0; i < sampleCount; ++i) {
            const float normalized = static_cast<float>(samples[i]) / 32768.0f;
            sum += normalized * normalized;
        }
    } else if (format.wBitsPerSample == 24) {
        for (UINT32 i = 0; i < sampleCount; ++i) {
            const BYTE* sample = data + i * 3;
            int32_t value = sample[0] | (sample[1] << 8) | (sample[2] << 16);
            if (value & 0x800000) {
                value |= ~0xFFFFFF;
            }
            const float normalized = static_cast<float>(value) / 8388608.0f;
            sum += normalized * normalized;
        }
    } else {
        return 0.0f;
    }

    return static_cast<float>(std::sqrt(sum / sampleCount));
}
