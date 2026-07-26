#ifndef CAMERA_TRACKING_OPENCV_H
#define CAMERA_TRACKING_OPENCV_H

#include "../TrackingCamera.h"

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct CameraDeviceInfo {
    int index = 0;
    std::string name;
};

struct CameraTrackingCalibration {
    float headYawOffset = 0.0f;
    float headPitchOffset = 0.0f;
    float headRollOffset = 0.0f;
    float headYawScale = 1.0f;
    float headPitchScale = 1.0f;
    float headRollScale = 1.0f;
    float blinkThreshold = 0.2f;
    float mouthClosed = 0.18f;
    float mouthOpenRange = 0.55f;
    float smileThreshold = 1.8f;
    float smoothing = 0.35f;
};

class CameraTrackingOpenCV
{
public:
    // Construct an idle OpenCV camera tracker.
    CameraTrackingOpenCV() = default;

    // Stop capture and release camera resources.
    ~CameraTrackingOpenCV();

    CameraTrackingOpenCV(const CameraTrackingOpenCV&) = delete;
    CameraTrackingOpenCV& operator=(const CameraTrackingOpenCV&) = delete;

    // Start camera capture using the requested device and OpenCV model files.
    bool start(int cameraIndex, const std::string& faceCascadePath, const std::string& lbfModelPath);

    // Stop the capture worker and release the camera device.
    void stop();

    // Return whether the capture worker is actively producing frames.
    bool isRunning() const { return m_running.load(); }

    // Return whether this build was compiled with OpenCV support.
    bool isAvailable() const;

    // Return user-facing camera tracker status text.
    std::string getStatus() const;

    // Return the latest neutral face tracking state.
    FaceTrackingState getState() const;

    // Return the current camera calibration values.
    CameraTrackingCalibration getCalibration() const;

    // Clamp and store camera calibration values.
    void setCalibration(const CameraTrackingCalibration& calibration);

    // Probe sequential camera indices and return the usable ones.
    static std::vector<CameraDeviceInfo> listAvailableDevices(int maxDevices = 8);

private:
    // Worker-thread loop that owns OpenCV camera capture and landmark tracking.
    void captureLoop(int cameraIndex, std::string faceCascadePath, std::string lbfModelPath);

    // Store the latest camera tracking state.
    void setState(const FaceTrackingState& state);

    // Store user-facing camera tracker status text.
    void setStatus(const std::string& status);

    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_starting{ false };
    std::atomic<bool> m_stopRequested{ false };
    std::thread m_thread;
    mutable std::mutex m_stateMutex;
    FaceTrackingState m_state;
    std::string m_status = "Camera tracker stopped";
    CameraTrackingCalibration m_calibration;
};

#endif
