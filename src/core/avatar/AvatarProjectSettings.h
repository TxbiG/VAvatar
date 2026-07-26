#pragma once

#include "AvatarRigs.h"
#include "../project/ProjectDocument.h"
#include "../tracking/Camera/OpenCV/CameraTrackingOpenCV.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <string>

namespace AvatarProjectSettings {
inline float number(const ProjectDocument& document, const std::string& key, float fallback)
{
    try { return std::stof(document.get(key)); }
    catch (...) { return fallback; }
}

inline int integer(const ProjectDocument& document, const std::string& key, int fallback)
{
    try { return std::stoi(document.get(key)); }
    catch (...) { return fallback; }
}

inline bool boolean(const ProjectDocument& document, const std::string& key, bool fallback)
{
    const std::string value = document.get(key);
    if (value.empty()) return fallback;
    return value == "1" || value == "true";
}

inline void saveCameraCalibration(ProjectDocument& document, const CameraTrackingCalibration& value)
{
    document.set("camera.headYawOffset", std::to_string(value.headYawOffset));
    document.set("camera.headPitchOffset", std::to_string(value.headPitchOffset));
    document.set("camera.headRollOffset", std::to_string(value.headRollOffset));
    document.set("camera.headYawScale", std::to_string(value.headYawScale));
    document.set("camera.headPitchScale", std::to_string(value.headPitchScale));
    document.set("camera.headRollScale", std::to_string(value.headRollScale));
    document.set("camera.blinkThreshold", std::to_string(value.blinkThreshold));
    document.set("camera.mouthClosed", std::to_string(value.mouthClosed));
    document.set("camera.mouthOpenRange", std::to_string(value.mouthOpenRange));
    document.set("camera.smileThreshold", std::to_string(value.smileThreshold));
    document.set("camera.smoothing", std::to_string(value.smoothing));
}

inline void loadCameraCalibration(const ProjectDocument& document, CameraTrackingCalibration& value)
{
    value.headYawOffset = number(document, "camera.headYawOffset", value.headYawOffset);
    value.headPitchOffset = number(document, "camera.headPitchOffset", value.headPitchOffset);
    value.headRollOffset = number(document, "camera.headRollOffset", value.headRollOffset);
    value.headYawScale = number(document, "camera.headYawScale", value.headYawScale);
    value.headPitchScale = number(document, "camera.headPitchScale", value.headPitchScale);
    value.headRollScale = number(document, "camera.headRollScale", value.headRollScale);
    value.blinkThreshold = number(document, "camera.blinkThreshold", value.blinkThreshold);
    value.mouthClosed = number(document, "camera.mouthClosed", value.mouthClosed);
    value.mouthOpenRange = number(document, "camera.mouthOpenRange", value.mouthOpenRange);
    value.smileThreshold = number(document, "camera.smileThreshold", value.smileThreshold);
    value.smoothing = number(document, "camera.smoothing", value.smoothing);
}

inline void saveModel3D(
    ProjectDocument& document,
    const Model3DRig& rig,
    const std::function<std::string(const std::string&)>& encodePath)
{
    document.set("model3d.path", encodePath(rig.modelPath));
    document.set("model3d.headBone", rig.headBone);
    document.set("model3d.neckBone", rig.neckBone);
    document.set("model3d.chestBone", rig.chestBone);
    document.set("model3d.jawBlendshape", rig.jawBlendshape);
    document.set("model3d.blinkLeftBlendshape", rig.blinkLeftBlendshape);
    document.set("model3d.blinkRightBlendshape", rig.blinkRightBlendshape);
    document.set("model3d.smileBlendshape", rig.smileBlendshape);
    document.set("model3d.headFollow", std::to_string(rig.headFollow));
    document.set("model3d.neckFollow", std::to_string(rig.neckFollow));
    document.set("model3d.chestFollow", std::to_string(rig.chestFollow));
}

inline void loadModel3D(
    const ProjectDocument& document,
    Model3DRig& rig,
    const std::function<std::string(const std::string&)>& decodePath)
{
    rig.modelPath = decodePath(document.get("model3d.path"));
    rig.headBone = document.get("model3d.headBone", rig.headBone);
    rig.neckBone = document.get("model3d.neckBone", rig.neckBone);
    rig.chestBone = document.get("model3d.chestBone", rig.chestBone);
    rig.jawBlendshape = document.get("model3d.jawBlendshape", rig.jawBlendshape);
    rig.blinkLeftBlendshape = document.get("model3d.blinkLeftBlendshape", rig.blinkLeftBlendshape);
    rig.blinkRightBlendshape = document.get("model3d.blinkRightBlendshape", rig.blinkRightBlendshape);
    rig.smileBlendshape = document.get("model3d.smileBlendshape", rig.smileBlendshape);
    rig.headFollow = number(document, "model3d.headFollow", rig.headFollow);
    rig.neckFollow = number(document, "model3d.neckFollow", rig.neckFollow);
    rig.chestFollow = number(document, "model3d.chestFollow", rig.chestFollow);
}

inline void saveLive2D(
    ProjectDocument& document,
    const Live2DRig& rig,
    const std::function<std::string(const std::string&)>& encodePath)
{
    document.set("live2d.layerCount", std::to_string(rig.layers.size()));
    for (size_t i = 0; i < rig.layers.size(); ++i) {
        const Live2DLayer& layer = rig.layers[i];
        const std::string prefix = "live2d.layer." + std::to_string(i) + ".";
        document.set(prefix + "name", layer.name);
        document.set(prefix + "imagePath", encodePath(layer.imagePath));
        document.set(prefix + "depth", std::to_string(layer.depth));
        document.set(prefix + "pivotX", std::to_string(layer.pivotX));
        document.set(prefix + "pivotY", std::to_string(layer.pivotY));
        document.set(prefix + "parallaxX", std::to_string(layer.parallaxX));
        document.set(prefix + "parallaxY", std::to_string(layer.parallaxY));
        document.set(prefix + "scale", std::to_string(layer.scale));
        document.set(prefix + "rotation", std::to_string(layer.rotation));
        document.set(prefix + "opacity", std::to_string(layer.opacity));
        document.set(prefix + "visibilityParameter", layer.visibilityParameter);
        document.set(prefix + "visibilityMin", std::to_string(layer.visibilityMin));
        document.set(prefix + "visibilityMax", std::to_string(layer.visibilityMax));
        document.set(prefix + "visible", layer.visible ? "1" : "0");
        document.set(prefix + "meshColumns", std::to_string(layer.meshColumns));
        document.set(prefix + "meshRows", std::to_string(layer.meshRows));
        document.set(prefix + "warpParameterX", layer.warpParameterX);
        document.set(prefix + "warpParameterY", layer.warpParameterY);
        document.set(prefix + "warpStrengthX", std::to_string(layer.warpStrengthX));
        document.set(prefix + "warpStrengthY", std::to_string(layer.warpStrengthY));
        document.set(prefix + "deformer", layer.deformer);
        document.set(prefix + "maskLayer", layer.maskLayer);
        document.set(prefix + "physicsEnabled", layer.physicsEnabled ? "1" : "0");
        document.set(prefix + "physicsParameter", layer.physicsParameter);
        document.set(prefix + "physicsStrengthX", std::to_string(layer.physicsStrengthX));
        document.set(prefix + "physicsStrengthY", std::to_string(layer.physicsStrengthY));
        document.set(prefix + "physicsStiffness", std::to_string(layer.physicsStiffness));
        document.set(prefix + "physicsDamping", std::to_string(layer.physicsDamping));
        document.set(prefix + "physicsGravity", std::to_string(layer.physicsGravity));
    }

    document.set("live2d.deformerCount", std::to_string(rig.deformers.size()));
    for (size_t i = 0; i < rig.deformers.size(); ++i) {
        const Live2DDeformer& deformer = rig.deformers[i];
        const std::string prefix = "live2d.deformer." + std::to_string(i) + ".";
        document.set(prefix + "name", deformer.name);
        document.set(prefix + "parent", deformer.parent);
        document.set(prefix + "pivotX", std::to_string(deformer.pivotX));
        document.set(prefix + "pivotY", std::to_string(deformer.pivotY));
        document.set(prefix + "offsetX", std::to_string(deformer.offsetX));
        document.set(prefix + "offsetY", std::to_string(deformer.offsetY));
        document.set(prefix + "rotation", std::to_string(deformer.rotation));
        document.set(prefix + "scaleX", std::to_string(deformer.scaleX));
        document.set(prefix + "scaleY", std::to_string(deformer.scaleY));
        document.set(prefix + "translateXParameter", deformer.translateXParameter);
        document.set(prefix + "translateYParameter", deformer.translateYParameter);
        document.set(prefix + "rotationParameter", deformer.rotationParameter);
        document.set(prefix + "translateXAmount", std::to_string(deformer.translateXAmount));
        document.set(prefix + "translateYAmount", std::to_string(deformer.translateYAmount));
        document.set(prefix + "rotationAmount", std::to_string(deformer.rotationAmount));
    }

    for (size_t i = 0; i < rig.bindings.size(); ++i) {
        const Live2DBinding& binding = rig.bindings[i];
        const std::string prefix = "live2d.binding." + std::to_string(i) + ".";
        document.set(prefix + "parameter", binding.parameter);
        document.set(prefix + "source", binding.source);
        document.set(prefix + "curve", binding.curve);
        document.set(prefix + "min", std::to_string(binding.minValue));
        document.set(prefix + "max", std::to_string(binding.maxValue));
        document.set(prefix + "smoothing", std::to_string(binding.smoothing));
        document.set(prefix + "inverted", binding.inverted ? "1" : "0");

        // Retain legacy keys for migration compatibility.
        const std::string legacy = "live2d." + std::to_string(i) + ".";
        document.set(legacy + "parameter", binding.parameter);
        document.set(legacy + "source", binding.source);
        document.set(legacy + "curve", binding.curve);
        document.set(legacy + "min", std::to_string(binding.minValue));
        document.set(legacy + "max", std::to_string(binding.maxValue));
        document.set(legacy + "smoothing", std::to_string(binding.smoothing));
        document.set(legacy + "inverted", binding.inverted ? "1" : "0");
    }
}

inline void loadLive2D(
    const ProjectDocument& document,
    Live2DRig& rig,
    const std::function<std::string(const std::string&)>& decodePath)
{
    rig.layers.clear();
    const int layerCount = std::max(0, integer(document, "live2d.layerCount", 0));
    for (int i = 0; i < layerCount; ++i) {
        Live2DLayer layer;
        const std::string prefix = "live2d.layer." + std::to_string(i) + ".";
        layer.name = document.get(prefix + "name");
        layer.imagePath = decodePath(document.get(prefix + "imagePath"));
        layer.depth = number(document, prefix + "depth", static_cast<float>(i));
        layer.pivotX = number(document, prefix + "pivotX", layer.pivotX);
        layer.pivotY = number(document, prefix + "pivotY", layer.pivotY);
        layer.parallaxX = number(document, prefix + "parallaxX", layer.parallaxX);
        layer.parallaxY = number(document, prefix + "parallaxY", layer.parallaxY);
        layer.scale = number(document, prefix + "scale", layer.scale);
        layer.rotation = number(document, prefix + "rotation", layer.rotation);
        layer.opacity = number(document, prefix + "opacity", layer.opacity);
        layer.visibilityParameter = document.get(prefix + "visibilityParameter");
        layer.visibilityMin = number(document, prefix + "visibilityMin", layer.visibilityMin);
        layer.visibilityMax = number(document, prefix + "visibilityMax", layer.visibilityMax);
        layer.visible = boolean(document, prefix + "visible", layer.visible);
        layer.meshColumns = std::clamp(integer(document, prefix + "meshColumns", layer.meshColumns), 1, 32);
        layer.meshRows = std::clamp(integer(document, prefix + "meshRows", layer.meshRows), 1, 32);
        layer.warpParameterX = document.get(prefix + "warpParameterX", layer.warpParameterX);
        layer.warpParameterY = document.get(prefix + "warpParameterY", layer.warpParameterY);
        layer.warpStrengthX = number(document, prefix + "warpStrengthX", layer.warpStrengthX);
        layer.warpStrengthY = number(document, prefix + "warpStrengthY", layer.warpStrengthY);
        layer.deformer = document.get(prefix + "deformer");
        layer.maskLayer = document.get(prefix + "maskLayer");
        layer.physicsEnabled = boolean(document, prefix + "physicsEnabled", layer.physicsEnabled);
        layer.physicsParameter = document.get(prefix + "physicsParameter", layer.physicsParameter);
        layer.physicsStrengthX = number(document, prefix + "physicsStrengthX", layer.physicsStrengthX);
        layer.physicsStrengthY = number(document, prefix + "physicsStrengthY", layer.physicsStrengthY);
        layer.physicsStiffness = number(document, prefix + "physicsStiffness", layer.physicsStiffness);
        layer.physicsDamping = number(document, prefix + "physicsDamping", layer.physicsDamping);
        layer.physicsGravity = number(document, prefix + "physicsGravity", layer.physicsGravity);
        rig.layers.push_back(layer);
    }

    rig.deformers.clear();
    const int deformerCount = std::max(0, integer(document, "live2d.deformerCount", 0));
    for (int i = 0; i < deformerCount; ++i) {
        Live2DDeformer deformer;
        const std::string prefix = "live2d.deformer." + std::to_string(i) + ".";
        deformer.name = document.get(prefix + "name");
        deformer.parent = document.get(prefix + "parent");
        deformer.pivotX = number(document, prefix + "pivotX", deformer.pivotX);
        deformer.pivotY = number(document, prefix + "pivotY", deformer.pivotY);
        deformer.offsetX = number(document, prefix + "offsetX", deformer.offsetX);
        deformer.offsetY = number(document, prefix + "offsetY", deformer.offsetY);
        deformer.rotation = number(document, prefix + "rotation", deformer.rotation);
        deformer.scaleX = number(document, prefix + "scaleX", deformer.scaleX);
        deformer.scaleY = number(document, prefix + "scaleY", deformer.scaleY);
        deformer.translateXParameter = document.get(prefix + "translateXParameter");
        deformer.translateYParameter = document.get(prefix + "translateYParameter");
        deformer.rotationParameter = document.get(prefix + "rotationParameter");
        deformer.translateXAmount = number(document, prefix + "translateXAmount", deformer.translateXAmount);
        deformer.translateYAmount = number(document, prefix + "translateYAmount", deformer.translateYAmount);
        deformer.rotationAmount = number(document, prefix + "rotationAmount", deformer.rotationAmount);
        rig.deformers.push_back(deformer);
    }

    for (size_t i = 0; i < rig.bindings.size(); ++i) {
        Live2DBinding& binding = rig.bindings[i];
        const std::string prefix = "live2d.binding." + std::to_string(i) + ".";
        const std::string legacy = "live2d." + std::to_string(i) + ".";
        const bool modern = !document.get(prefix + "parameter").empty();
        const std::string& key = modern ? prefix : legacy;
        binding.parameter = document.get(key + "parameter", binding.parameter);
        binding.source = document.get(key + "source", binding.source);
        binding.curve = document.get(key + "curve", binding.curve);
        binding.minValue = number(document, key + "min", binding.minValue);
        binding.maxValue = number(document, key + "max", binding.maxValue);
        binding.smoothing = number(document, key + "smoothing", binding.smoothing);
        binding.inverted = boolean(document, key + "inverted", binding.inverted);
    }
}
}