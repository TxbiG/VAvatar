#include "AudioTracking.h"
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <thread>

#include <functiondiscoverykeys_devpkey.h> // Contains the PKEY_Device_FriendlyName
#include <string>

// Function to convert wide-character (WCHAR) to multibyte (char) string using pure Windows API
std::string WideCharToString(LPCWSTR wideStr)
{
    std::wstring ws(wideStr);
    // your new String
    std::string str(ws.begin(), ws.end());
    // Show String
    return str;
};

static std::string GetDeviceName(IMMDevice* pDevice)
{
    IPropertyStore* pPropertyStore = nullptr;
    HRESULT hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore); // Open the property store for reading
    if (FAILED(hr))
    {
        std::cerr << "Failed to open property store. HRESULT: " << hr << std::endl;
        return "Unknown Device";
    }

    PROPVARIANT varName;
    PropVariantInit(&varName);

    // Use the correct property key (PKEY_Device_FriendlyName) to retrieve the friendly name
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &varName);
    std::string deviceName;

    if (SUCCEEDED(hr) && varName.vt == VT_LPWSTR)  // Ensure the value type is a wide string (VT_LPWSTR)
    {
        // Convert the wide string to a standard string using Windows API (WideCharToMultiByte)
        deviceName = WideCharToString(varName.pwszVal);
    }
    else
    {
        std::cout << "Failed to get device friendly name or incorrect variant type. HRESULT: " << hr << std::endl;
        deviceName = "Unknown Device";
    }

    // Clean up
    PropVariantClear(&varName);
    pPropertyStore->Release();

    std::cout << deviceName;

    return deviceName;
};

AudioTracking::AudioTracking() : m_isActivated(false), pAudioClient(nullptr), pCaptureClient(nullptr), pDevice(nullptr), pEnumerator(nullptr)
{

    if (initializeAudio()) {
        std::thread audioThread(captureThread, this);
        audioThread.detach(); // Run capture in a separate thread
    }
}

AudioTracking::~AudioTracking()
{
    if (pAudioClient) pAudioClient->Stop();
    if (pCaptureClient) pCaptureClient->Release();
    if (pAudioClient) pAudioClient->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();
}

bool AudioTracking::initializeAudio()
{
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) return false;


    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return false;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    if (FAILED(hr)) return false;

    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
    if (FAILED(hr)) return false;

    WAVEFORMATEX* pwfx = nullptr;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) return false;

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, pwfx, NULL);
    if (FAILED(hr)) return false;

    hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
    if (FAILED(hr)) return false;

    CoTaskMemFree(pwfx);
    pAudioClient->Start();

    return true;
}

void AudioTracking::captureAudio()
{
    UINT32 packetLength = 0;
    HRESULT hr = pCaptureClient->GetNextPacketSize(&packetLength);
    if (FAILED(hr)) return;

    BYTE* pData;
    UINT32 numFramesAvailable;
    DWORD flags;

    hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
    if (FAILED(hr)) return;

    float sum = 0.0f;
    const float* data = reinterpret_cast<const float*>(pData);
    for (UINT32 i = 0; i < numFramesAvailable; i++) {
                sum += data[i] * data[i];
    }

    float rms = std::sqrt(sum / numFramesAvailable);
    float threshold = 0.0001f;
    m_isActivated = rms > threshold;

    hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
    if (FAILED(hr)) return;

    hr = pCaptureClient->GetNextPacketSize(&packetLength);
    if (FAILED(hr)) return;
}

DWORD WINAPI AudioTracking::captureThread(LPVOID lpParam)
{
    AudioTracking* self = static_cast<AudioTracking*>(lpParam);
    self->captureAudio();
    return 0;
}