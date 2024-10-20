#ifndef AUDIOTRACKING_H
#define AUDIOTRACKING_H

#include <iostream>
#include <cmath>
#include <Windows.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

/*
    Future Development Add vowel detection
*/

class AudioTracking
{
public:
    AudioTracking();
    ~AudioTracking();

    bool isActivated() const { return m_isActivated; }
    void captureAudio();
private:
    bool m_isActivated;
    IAudioClient* pAudioClient;
    IAudioCaptureClient* pCaptureClient;
    IMMDevice* pDevice;
    IMMDeviceEnumerator* pEnumerator;

    bool initializeAudio();

    static DWORD WINAPI captureThread(LPVOID lpParam);
};

#endif // AUDIOTRACKING_H