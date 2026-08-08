#include "AudioTracking.h"

#include <string>

AudioTracking::AudioTracking()
{
}

AudioTracking::~AudioTracking()
{
    stop();
}

bool AudioTracking::start()
{
    // No audio capture support on non-Windows platforms yet.
    setStatus("Microphone not available on this platform");
    m_starting.store(false);
    m_running.store(false);
    return false;
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
    state.available = false;
    state.enabled = false;
    state.speaking = false;
    state.rawLevel = m_level.load();
    state.smoothedLevel = m_smoothedLevel.load();
    state.threshold = m_threshold.load();
    state.mouthOpen = getMouthOpen();
    state.deviceName = getDeviceName();
    return state;
}

void AudioTracking::captureLoop()
{
    // Not implemented on this platform.
}

bool AudioTracking::initializeAudio()
{
    // No platform audio initialization; return false to indicate unavailable.
    setStatus("No audio backend on this platform");
    return false;
}

void AudioTracking::shutdownAudio()
{
    // Nothing to do for the stub.
    m_running.store(false);
}

void AudioTracking::captureAvailablePackets()
{
    // No capture on non-Windows platforms.
}

void AudioTracking::setStatus(const std::string& status)
{
    std::lock_guard<std::mutex> lock(m_deviceNameMutex);
    m_status = status;
}

float AudioTracking::calculateRms(const BYTE* /*data*/, UINT32 /*frameCount*/, const WAVEFORMATEX& /*format*/) const
{
    return 0.0f;
}
