#ifndef PNGTUBER_ASSETS_H
#define PNGTUBER_ASSETS_H

#include <string>

struct PNGTuberEffectVec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct PNGTuberEffectColor {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

// A state's minimum and maximum motion values. The runtime blends within this
// range using mouth/expression intensity and its animation phase.
struct PNGTuberEffectState {
    PNGTuberEffectVec2 bounceMin{};
    PNGTuberEffectVec2 squishMin{};
    PNGTuberEffectVec2 frequencyMin{1.0f, 1.0f};
    PNGTuberEffectVec2 amplitudeMin{};
    float rotationalMin = 0.0f;
    float dragMin = 0.0f;

    PNGTuberEffectVec2 bounceMax{};
    PNGTuberEffectVec2 squishMax{};
    PNGTuberEffectVec2 frequencyMax{1.0f, 1.0f};
    PNGTuberEffectVec2 amplitudeMax{};
    float rotationalMax = 0.0f;
    float dragMax = 0.0f;

    PNGTuberEffectColor color{};
};

struct PNGTuberEffects {
    PNGTuberEffectState silent;
    PNGTuberEffectState talking;
    PNGTuberEffectState expression;
    bool expressionEnabled = false;
    float expressionThreshold = 0.65f;

    PNGTuberEffects()
    {
        silent.bounceMax = {0.0f, 2.0f};
        silent.frequencyMin = silent.frequencyMax = {0.8f, 0.8f};
        talking.bounceMax = {0.0f, 18.0f};
        talking.squishMax = {0.08f, 0.18f};
        talking.frequencyMin = {2.0f, 2.0f};
        talking.frequencyMax = {4.0f, 4.0f};
        expression.frequencyMin = expression.frequencyMax = {1.0f, 1.0f};
    }
};

// File paths for the image slots a PNGTuber avatar can switch between.
struct PNGTuberAssetSlots {
    std::string baseImage;
    std::string mouthOpenImage;
    std::string mouthClosedImage;
    std::string blinkImage;
    std::string expressionImage;
};

#endif