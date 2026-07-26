#pragma once

#include "AvatarTrackingMapper.h"

#include <array>
#include <string>
#include <vector>

struct Live2DLayer {
    std::string name;
    std::string imagePath;
    float depth = 0.0f;
    float parallaxX = 0.0f;
    float parallaxY = 0.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float rotation = 0.0f;
    float scale = 1.0f;
    float opacity = 1.0f;
    std::string visibilityParameter;
    float visibilityMin = -1000.0f;
    float visibilityMax = 1000.0f;
    bool visible = true;

    // Grid deformation. A 1x1 grid behaves like the original image quad.
    int meshColumns = 1;
    int meshRows = 1;
    std::string warpParameterX = "ParamAngleX";
    std::string warpParameterY = "ParamAngleY";
    float warpStrengthX = 0.0f;
    float warpStrengthY = 0.0f;

    // Named hierarchy and clip-mask attachment.
    std::string deformer;
    std::string maskLayer;

    // Damped spring motion applied after tracking/deformer transforms.
    bool physicsEnabled = false;
    std::string physicsParameter = "ParamAngleX";
    float physicsStrengthX = 0.0f;
    float physicsStrengthY = 0.0f;
    float physicsStiffness = 45.0f;
    float physicsDamping = 9.0f;
    float physicsGravity = 0.0f;
};

struct Live2DDeformer {
    std::string name;
    std::string parent;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    std::string translateXParameter;
    std::string translateYParameter;
    std::string rotationParameter;
    float translateXAmount = 0.0f;
    float translateYAmount = 0.0f;
    float rotationAmount = 0.0f;
};

struct Live2DBinding {
    std::string parameter;
    std::string source;
    std::string curve = "Linear";
    float minValue = -30.0f;
    float maxValue = 30.0f;
    float smoothing = 0.35f;
    bool inverted = false;
    float currentValue = 0.0f;
};

struct Live2DRig {
    std::vector<Live2DLayer> layers;
    std::vector<Live2DDeformer> deformers;
    std::array<Live2DBinding, 6> bindings;
    Live2DPose evaluatedPose;

    void resetBindings();
    void evaluate(const AvatarTrackingPose& pose, float deltaSeconds);
    float parameter(const std::string& name, float fallback = 0.0f) const;
    const Live2DDeformer* findDeformer(const std::string& name) const;
};

struct Model3DRig {
    std::string modelPath;
    std::string headBone = "Head";
    std::string neckBone = "Neck";
    std::string chestBone = "Chest";
    std::string jawBlendshape = "JawOpen";
    std::string blinkLeftBlendshape = "BlinkLeft";
    std::string blinkRightBlendshape = "BlinkRight";
    std::string smileBlendshape = "Smile";
    float headFollow = 1.0f;
    float neckFollow = 0.35f;
    float chestFollow = 0.15f;
    Model3DPose evaluatedPose;

    void evaluate(const AvatarTrackingPose& pose);
};