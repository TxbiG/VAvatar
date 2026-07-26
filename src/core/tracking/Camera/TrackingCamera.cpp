#include "TrackingCamera.h"

// Apply face/head/mouth/blink tracking while respecting enabled tracking channels.
void TrackingCamera::updateFromFace(float yaw, float pitch, float roll, float mouthOpen, bool leftBlink, bool rightBlink, bool smiling)
{
    m_state.foundPerson = true;
    if (m_headTrack) {
        m_state.headYaw = yaw;
        m_state.headPitch = pitch;
        m_state.headRoll = roll;
    }
    if (m_faceTrack) {
        m_state.mouthOpen = std::clamp(mouthOpen, 0.0f, 1.0f);
        m_state.smiling = smiling;
    }
    if (m_blinkTrack) {
        m_state.leftBlink = leftBlink;
        m_state.rightBlink = rightBlink;
    }
}

void TrackingCamera::updateHands(int detectedHands)
{
    if (m_handTrack) m_state.detectedHands = std::clamp(detectedHands, 0, 2);
}

void TrackingCamera::updateContinuousFace(float left, float right, float smile, float mouthForm, float gazeX, float gazeY, float browLeft, float browRight)
{
    if (m_blinkTrack) {
        m_state.leftEyeOpen = std::clamp(left, 0.0f, 1.0f);
        m_state.rightEyeOpen = std::clamp(right, 0.0f, 1.0f);
    }
    if (m_faceTrack) {
        m_state.smile = std::clamp(smile, 0.0f, 1.0f);
        m_state.mouthForm = std::clamp(mouthForm, -1.0f, 1.0f);
        m_state.eyeGazeX = std::clamp(gazeX, -1.0f, 1.0f);
        m_state.eyeGazeY = std::clamp(gazeY, -1.0f, 1.0f);
        m_state.browLeft = std::clamp(browLeft, -1.0f, 1.0f);
        m_state.browRight = std::clamp(browRight, -1.0f, 1.0f);
    }
}

void TrackingCamera::updateBody(float yaw, float pitch)
{
    if (!m_upperbodyTrack) return;
    m_state.shoulderYaw = std::clamp(yaw, -30.0f, 30.0f);
    m_state.shoulderPitch = std::clamp(pitch, -30.0f, 30.0f);
}

void TrackingCamera::updateHandLandmarks(const FaceTrackingState::HandLandmark* landmarks, int count)
{
    if (!m_handTrack) return;
    m_state.handLandmarkCount = landmarks ? std::clamp(count, 0, 42) : 0;
    for (int i = 0; i < m_state.handLandmarkCount; ++i) {
        m_state.handLandmarks[static_cast<size_t>(i)] = landmarks[i];
    }
}

void TrackingCamera::clearFrame()
{
    m_state = FaceTrackingState{};
}