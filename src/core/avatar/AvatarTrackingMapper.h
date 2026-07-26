#ifndef AVATAR_TRACKING_MAPPER_H
#define AVATAR_TRACKING_MAPPER_H

#include "../tracking/Camera/TrackingCamera.h"

#include <algorithm>

enum class AvatarRigType {
    PNGTuber,
    Live2D,
    Model3D
};

struct TrackingInput {
    FaceTrackingState face;
    float voiceMouthOpen = 0.0f;
};

struct PNGTuberPose {
    bool useOpenMouthSprite = false;
    bool useBlinkSprite = false;
    bool useExpressionSprite = false;
    float talkAmount = 0.0f;
    float expressionAmount = 0.0f;
    float bounce = 0.0f;
    float squish = 0.0f;
    float tiltDegrees = 0.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
};

struct Live2DPose {
    float paramAngleX = 0.0f;
    float paramAngleY = 0.0f;
    float paramAngleZ = 0.0f;
    float paramMouthOpenY = 0.0f;
    float paramMouthForm = 0.0f;
    float paramEyeLOpen = 1.0f;
    float paramEyeROpen = 1.0f;
    float paramEyeBallX = 0.0f;
    float paramEyeBallY = 0.0f;
    float paramBrowLY = 0.0f;
    float paramBrowRY = 0.0f;
    float paramBodyAngleX = 0.0f;
    float paramBodyAngleY = 0.0f;
};

struct Model3DPose {
    float headYaw = 0.0f;
    float headPitch = 0.0f;
    float headRoll = 0.0f;
    float jawOpen = 0.0f;
    float blinkLeft = 0.0f;
    float blinkRight = 0.0f;
    float smile = 0.0f;
    float handPresence = 0.0f;
};

struct AvatarTrackingPose {
    AvatarRigType rigType = AvatarRigType::PNGTuber;
    float mouthOpen = 0.0f;
    bool faceFound = false;
    PNGTuberPose png;
    Live2DPose live2D;
    Model3DPose model3D;
};

class AvatarTrackingMapper {
public:
    // Convert neutral tracking input into all avatar-specific output poses.
    AvatarTrackingPose map(AvatarRigType rigType, const TrackingInput& input) const;

private:
    // Clamp normalized avatar channels to the 0..1 range.
    static float clamp01(float value) { return std::clamp(value, 0.0f, 1.0f); }

    // Normalize a signed tracking value against a symmetric maximum.
    static float normalizeSigned(float value, float maxAbs);

    // Map neutral tracking into PNGTuber sprite and 2D motion values.
    PNGTuberPose mapPNGTuber(const TrackingInput& input, float mouthOpen) const;

    // Map neutral tracking into Live2D / 2.5D parameter values.
    Live2DPose mapLive2D(const TrackingInput& input, float mouthOpen) const;

    // Map neutral tracking into 3D head/blendshape values.
    Model3DPose mapModel3D(const TrackingInput& input, float mouthOpen) const;
};

#endif
