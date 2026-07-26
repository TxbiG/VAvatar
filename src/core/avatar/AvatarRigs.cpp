#include "AvatarRigs.h"

#include <algorithm>
#include <cmath>

namespace {
float sourceValue(const Live2DBinding& binding, const Live2DPose& pose)
{
    if (binding.source == "headYaw") return pose.paramAngleX / 30.0f;
    if (binding.source == "headPitch") return pose.paramAngleY / 30.0f;
    if (binding.source == "headRoll") return pose.paramAngleZ / 30.0f;
    if (binding.source == "mouthOpen" || binding.source == "voiceMouthOpen") return pose.paramMouthOpenY;
    if (binding.source == "mouthForm" || binding.source == "smile") return pose.paramMouthForm;
    if (binding.source == "leftEyeOpen") return pose.paramEyeLOpen;
    if (binding.source == "rightEyeOpen") return pose.paramEyeROpen;
    if (binding.source == "bodyAngleX") return pose.paramBodyAngleX / 30.0f;
    if (binding.source == "bodyAngleY") return pose.paramBodyAngleY / 30.0f;
    if (binding.source == "eyeGazeX") return pose.paramEyeBallX;
    if (binding.source == "eyeGazeY") return pose.paramEyeBallY;
    if (binding.source == "browLeft") return pose.paramBrowLY;
    if (binding.source == "browRight") return pose.paramBrowRY;
    return 0.0f;
}

bool isUnipolarSource(const std::string& source)
{
    return source == "mouthOpen" ||
        source == "voiceMouthOpen" ||
        source == "smile" ||
        source == "leftEyeOpen" ||
        source == "rightEyeOpen" ||
        source == "handPresence";
}
float curveValue(float value, const std::string& curve)
{
    const float sign = value < 0.0f ? -1.0f : 1.0f;
    const float magnitude = std::clamp(std::abs(value), 0.0f, 1.0f);
    if (curve == "Ease") return sign * magnitude * magnitude * (3.0f - 2.0f * magnitude);
    if (curve == "Responsive") return sign * std::sqrt(magnitude);
    if (curve == "Snap") return magnitude >= 0.5f ? sign : 0.0f;
    return std::clamp(value, -1.0f, 1.0f);
}
}

void Live2DRig::resetBindings()
{
    bindings = {{
        {"ParamAngleX", "headYaw", "Linear", -30.0f, 30.0f, 0.35f},
        {"ParamAngleY", "headPitch", "Ease", -30.0f, 30.0f, 0.35f},
        {"ParamAngleZ", "headRoll", "Linear", -30.0f, 30.0f, 0.35f},
        {"ParamMouthOpenY", "mouthOpen", "Responsive", 0.0f, 1.0f, 0.2f},
        {"ParamEyeLOpen", "leftEyeOpen", "Linear", 0.0f, 1.0f, 0.15f},
        {"ParamEyeROpen", "rightEyeOpen", "Linear", 0.0f, 1.0f, 0.15f}
    }};
}

void Live2DRig::evaluate(const AvatarTrackingPose& pose, float deltaSeconds)
{
    evaluatedPose = pose.live2D;
    for (Live2DBinding& binding : bindings) {
        float normalized = curveValue(sourceValue(binding, pose.live2D), binding.curve);
        if (binding.inverted) normalized = -normalized;
        const float unit = isUnipolarSource(binding.source)
            ? std::clamp(normalized, 0.0f, 1.0f)
            : normalized * 0.5f + 0.5f;
        const float target = binding.minValue + unit * (binding.maxValue - binding.minValue);
        const float response = std::max(0.01f, 1.0f - binding.smoothing);
        const float alpha = 1.0f - std::exp(-std::max(0.0f, deltaSeconds) * (4.0f + response * 26.0f));
        binding.currentValue += (target - binding.currentValue) * std::clamp(alpha, 0.0f, 1.0f);
    }
}

float Live2DRig::parameter(const std::string& name, float fallback) const
{
    for (const Live2DBinding& binding : bindings) {
        if (!binding.parameter.empty() && binding.parameter == name) return binding.currentValue;
    }
    if (name == "ParamAngleX") return evaluatedPose.paramAngleX;
    if (name == "ParamAngleY") return evaluatedPose.paramAngleY;
    if (name == "ParamAngleZ") return evaluatedPose.paramAngleZ;
    if (name == "ParamMouthOpenY") return evaluatedPose.paramMouthOpenY;
    if (name == "ParamMouthForm") return evaluatedPose.paramMouthForm;
    if (name == "ParamEyeLOpen") return evaluatedPose.paramEyeLOpen;
    if (name == "ParamEyeROpen") return evaluatedPose.paramEyeROpen;
    if (name == "ParamEyeBallX") return evaluatedPose.paramEyeBallX;
    if (name == "ParamEyeBallY") return evaluatedPose.paramEyeBallY;
    if (name == "ParamBrowLY") return evaluatedPose.paramBrowLY;
    if (name == "ParamBrowRY") return evaluatedPose.paramBrowRY;
    if (name == "ParamBodyAngleX") return evaluatedPose.paramBodyAngleX;
    if (name == "ParamBodyAngleY") return evaluatedPose.paramBodyAngleY;
    return fallback;
}

const Live2DDeformer* Live2DRig::findDeformer(const std::string& name) const
{
    for (const Live2DDeformer& deformer : deformers) {
        if (deformer.name == name) return &deformer;
    }
    return nullptr;
}

void Model3DRig::evaluate(const AvatarTrackingPose& pose)
{
    evaluatedPose = pose.model3D;
    evaluatedPose.headYaw *= headFollow;
    evaluatedPose.headPitch *= headFollow;
    evaluatedPose.headRoll *= headFollow;
}