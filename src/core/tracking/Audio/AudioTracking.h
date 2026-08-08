#ifndef AUDIOTRACKING_H
#define AUDIOTRACKING_H

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <cstdint>

#ifdef _WIN32
#include <Audioclient.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#else
// Provide lightweight stand-ins for Windows audio types so the header can be
// included/compiled on non-Windows platforms. The real WASAPI implementation
// remains in AudioTracking.cpp and is only built on Windows.
using BYTE = unsigned char;
using UINT32 = uint32_t;

struct WAVEFORMATEX {
    int wFormatTag;
    int nChannels;
    int nSamplesPerSec;
    int nAvgBytesPerSec;
    int nBlockAlign;
    int wBitsPerSample;
    int cbSize;
};

struct WAVEFORMATEXTENSIBLE {
    WAVEFORMATEX Format;
    struct { unsigned int Data1; } SubFormat;
    int cbSize;
};

// Dummy forward declarations to avoid depending on Windows COM types.
struct IAudioClient {};
struct IAudioCaptureClient {};
struct IMMDevice {};
struct IMMDeviceEnumerator {};
#endif

struct VoiceTrackingState {
    bool available = false;
    bool enabled = false;
    bool speaking = false;
    float rawLevel = 0.0f;
    float smoothedLevel = 0.0f;
    float threshold = 0.025f;
    float mouthOpen = 0.0f;
    std::string deviceName = "No microphone";
};

class AudioTracking
{
public:
    // Construct an idle microphone tracker.
    AudioTracking();

    // Stop capture and release audio resources.
    ~AudioTracking();

    AudioTracking(const AudioTracking&) = delete;
    AudioTracking& operator=(const AudioTracking&) = delete;

    // Start the capture worker if it is not already running.
    bool start();

    // Stop the capture worker and release audio resources.
    void stop();

    // Return whether the smoothed audio level is above the speech threshold.
    bool isActivated() const { return m_isActivated.load(); }

    // Return whether the capture worker is actively reading audio.
    bool isRunning() const { return m_running.load(); }

    // Return the most recent raw RMS level.
    float getLevel() const { return m_level.load(); }

    // Return the smoothed RMS level used for speaking detection.
    float getSmoothedLevel() const { return m_smoothedLevel.load(); }

    // Convert the smoothed level and threshold into a normalized mouth-open value.
    float getMouthOpen() const;

    // Return the current speaking threshold.
    float getThreshold() const { return m_threshold.load(); }

    // Set the speaking threshold, clamped to a usable range.
    void setThreshold(float threshold);

    // Return the friendly name of the active microphone.
    std::string getDeviceName() const;

    // Return user-facing microphone status text.
    std::string getStatus() const;

    // Return a complete snapshot of the current voice tracking state.
    VoiceTrackingState getState() const;

private:
    // Worker-thread loop that initializes audio and captures packets.
    void captureLoop();

    // Open the default microphone and prepare capture.
    bool initializeAudio();

    // Release all audio objects owned by this tracker.
    void shutdownAudio();

    // Drain all currently available microphone packets into RMS state.
    void captureAvailablePackets();

    // Store user-facing microphone status text.
    void setStatus(const std::string& status);

    // Calculate RMS for supported sample formats.
    float calculateRms(const BYTE* data, UINT32 frameCount, const WAVEFORMATEX& format) const;

    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_starting{ false };
    std::atomic<bool> m_stopRequested{ false };
    std::atomic<bool> m_isActivated{ false };
    std::atomic<float> m_level{ 0.0f };
    std::atomic<float> m_smoothedLevel{ 0.0f };
    std::atomic<float> m_threshold{ 0.025f };

    std::thread m_captureThread;
    mutable std::mutex m_deviceNameMutex;
    std::string m_deviceName = "No microphone";
    std::string m_status = "Microphone stopped";

#ifdef _WIN32
    IAudioClient* m_audioClient = nullptr;
    IAudioCaptureClient* m_captureClient = nullptr;
    IMMDevice* m_device = nullptr;
    IMMDeviceEnumerator* m_enumerator = nullptr;
    WAVEFORMATEX* m_mixFormat = nullptr;
#endif
};

#endif
