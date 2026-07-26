#include "AvatarTrackingMapper.h"

#include <cmath>

// Normalize a signed tracking channel against a maximum absolute value.
float AvatarTrackingMapper::normalizeSigned(float value, float maxAbs)
{
    if (maxAbs <= 0.0f) {
        return 0.0f;
    }

    return std::clamp(value / maxAbs, -1.0f, 1.0f);
}

// Build a complete avatar pose packet for all supported avatar output types.
AvatarTrackingPose AvatarTrackingMapper::map(AvatarRigType rigType, const TrackingInput& input) const
{
    AvatarTrackingPose pose;
    pose.rigType = rigType;
    pose.faceFound = input.face.foundPerson;
    pose.mouthOpen = clamp01(std::max(input.face.mouthOpen, input.voiceMouthOpen));
    pose.png = mapPNGTuber(input, pose.mouthOpen);
    pose.live2D = mapLive2D(input, pose.mouthOpen);
    pose.model3D = mapModel3D(input, pose.mouthOpen);
    return pose;
}

// Convert mouth/blink/head data into PNGTuber slot and 2D motion outputs.
PNGTuberPose AvatarTrackingMapper::mapPNGTuber(const TrackingInput& input, float mouthOpen) const
{
    PNGTuberPose pose;
    pose.useOpenMouthSprite = mouthOpen > 0.18f;
    pose.useBlinkSprite = input.face.leftBlink || input.face.rightBlink;
    pose.talkAmount = mouthOpen;
    pose.expressionAmount = clamp01(input.face.smile > 0.0f ? input.face.smile : (input.face.smiling ? 1.0f : 0.0f));
    pose.bounce = mouthOpen * 18.0f;
    pose.squish = mouthOpen * 0.18f;
    pose.tiltDegrees = input.face.headRoll * 0.55f;
    pose.offsetX = normalizeSigned(input.face.headYaw, 30.0f) * 28.0f;
    pose.offsetY = normalizeSigned(input.face.headPitch, 20.0f) * 18.0f;
    return pose;
}

// Convert neutral tracking into common Live2D/Cubism-style parameters.
Live2DPose AvatarTrackingMapper::mapLive2D(const TrackingInput& input, float mouthOpen) const
{
    Live2DPose pose;
    pose.paramAngleX = normalizeSigned(input.face.headYaw, 30.0f) * 30.0f;
    pose.paramAngleY = normalizeSigned(-input.face.headPitch, 20.0f) * 30.0f;
    pose.paramAngleZ = normalizeSigned(input.face.headRoll, 20.0f) * 30.0f;
    pose.paramMouthOpenY = mouthOpen;
    pose.paramMouthForm = input.face.smile > 0.0f ? input.face.smile : (input.face.smiling ? 1.0f : input.face.mouthForm);
    pose.paramEyeLOpen = input.face.leftBlink ? 0.0f : input.face.leftEyeOpen;
    pose.paramEyeROpen = input.face.rightBlink ? 0.0f : input.face.rightEyeOpen;
    pose.paramEyeBallX = std::clamp(input.face.eyeGazeX, -1.0f, 1.0f);
    pose.paramEyeBallY = std::clamp(input.face.eyeGazeY, -1.0f, 1.0f);
    pose.paramBrowLY = std::clamp(input.face.browLeft, -1.0f, 1.0f);
    pose.paramBrowRY = std::clamp(input.face.browRight, -1.0f, 1.0f);
    pose.paramBodyAngleX = std::clamp(input.face.shoulderYaw, -30.0f, 30.0f);
    pose.paramBodyAngleY = std::clamp(input.face.shoulderPitch, -30.0f, 30.0f);
    return pose;
}

// Convert neutral tracking into 3D head rotation and blendshape-style values.
Model3DPose AvatarTrackingMapper::mapModel3D(const TrackingInput& input, float mouthOpen) const
{
    Model3DPose pose;
    pose.headYaw = input.face.headYaw;
    pose.headPitch = input.face.headPitch;
    pose.headRoll = input.face.headRoll;
    pose.jawOpen = mouthOpen;
    pose.blinkLeft = input.face.leftBlink ? 1.0f : 1.0f - input.face.leftEyeOpen;
    pose.blinkRight = input.face.rightBlink ? 1.0f : 1.0f - input.face.rightEyeOpen;
    pose.smile = input.face.smile > 0.0f ? input.face.smile : (input.face.smiling ? 1.0f : 0.0f);
    pose.handPresence = clamp01(static_cast<float>(input.face.detectedHands) / 2.0f);
    return pose;
}
