#ifndef TRACKING_CAMERA_H
#define TRACKING_CAMERA_H

#include <algorithm>
#include <array>

struct FaceTrackingState {
    bool foundPerson = false;
    bool leftBlink = false;
    bool rightBlink = false;
    bool smiling = false;
    float leftEyeOpen = 1.0f, rightEyeOpen = 1.0f, smile = 0.0f, mouthForm = 0.0f;
    float eyeGazeX = 0.0f, eyeGazeY = 0.0f, browLeft = 0.0f, browRight = 0.0f;
    float mouthOpen = 0.0f;
    float headPitch = 0.0f;
    float headYaw = 0.0f;
    float headRoll = 0.0f;
    float shoulderYaw = 0.0f, shoulderPitch = 0.0f;
    struct HandLandmark { float x=0.0f, y=0.0f, z=0.0f, confidence=0.0f; };
    std::array<HandLandmark, 42> handLandmarks{};
    int handLandmarkCount = 0;
    int detectedHands = 0;
};

class TrackingCamera
{
public:
    TrackingCamera() = default;

    // Set whether the current frame has a detected person.
    bool set_detect_person(bool value) { m_state.foundPerson = value; return m_state.foundPerson; }

    // Enable or disable neutral face channels.
    bool set_face_tracking(bool value) { m_faceTrack = value; return m_faceTrack; }

    // Enable or disable neutral head rotation channels.
    bool set_head_tracking(bool value) { m_headTrack = value; return m_headTrack; }

    // Enable or disable blink channels.
    bool set_blink_tracking(bool value) { m_blinkTrack = value; return m_blinkTrack; }

    // Enable or disable hand-presence channels.
    bool set_hand_tracking(bool value) { m_handTrack = value; return m_handTrack; }

    // Enable or disable shoulder/body-orientation channels.
    bool set_upperbody_tracking(bool value) { m_upperbodyTrack = value; return m_upperbodyTrack; }

    // Return whether the current state has a detected person.
    bool get_detect_person() const { return m_state.foundPerson; }

    // Return whether hand tracking is enabled.
    bool get_hand_tracking() const { return m_handTrack; }

    // Return whether face tracking is enabled.
    bool get_face_tracking() const { return m_faceTrack; }

    // Return whether head tracking is enabled.
    bool get_head_tracking() const { return m_headTrack; }

    // Return whether blink tracking is enabled.
    bool get_blink_tracking() const { return m_blinkTrack; }

    // Return whether upper-body tracking is enabled.
    bool get_upperbody_tracking() const { return m_upperbodyTrack; }

    // Mark whether a camera debug/vision marker mode is enabled.
    bool set_vision_mark(bool value) { m_camVision = value; return m_camVision; }

    // Mark whether normal camera vision mode is enabled.
    bool set_vision_normal(bool value) { m_camNormal = value; return m_camNormal; }

    // Return whether camera debug/vision marker mode is enabled.
    bool get_vision_mark() const { return m_camVision; }

    // Return whether normal camera vision mode is enabled.
    bool get_vision_normal() const { return m_camNormal; }

    // Update neutral face/head/mouth/blink state from a camera tracker.
    void updateFromFace(float yaw, float pitch, float roll, float mouthOpen, bool leftBlink, bool rightBlink, bool smiling);

    // Update neutral hand-presence state from a camera tracker.
    void updateHands(int detectedHands);
    void updateContinuousFace(float leftEyeOpen, float rightEyeOpen, float smile, float mouthForm, float gazeX, float gazeY, float browLeft, float browRight);
    void updateBody(float shoulderYaw, float shoulderPitch);
    void updateHandLandmarks(const FaceTrackingState::HandLandmark* landmarks, int count);

    // Clear per-frame tracking values when no person/frame is available.
    void clearFrame();

    // Return the current neutral camera tracking state.
    const FaceTrackingState& getState() const { return m_state; }

private:
    FaceTrackingState m_state;
    bool m_handTrack = false;
    bool m_faceTrack = true;
    bool m_blinkTrack = true;
    bool m_headTrack = true;
    bool m_upperbodyTrack = false;
    bool m_camNormal = false;
    bool m_camVision = false;
};

#endif
