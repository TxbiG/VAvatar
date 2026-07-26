#pragma once

#include "PNGTuberAssets.h"
#include "../project/ProjectDocument.h"

#include <string>

namespace PNGTuberEffectProject {
inline float readFloat(const ProjectDocument& document, const std::string& key, float fallback)
{
    try { return std::stof(document.get(key)); }
    catch (...) { return fallback; }
}

inline bool readBool(const ProjectDocument& document, const std::string& key, bool fallback)
{
    const std::string value = document.get(key);
    if (value.empty()) return fallback;
    return value == "1" || value == "true";
}

inline void saveVec2(ProjectDocument& document, const std::string& key, const PNGTuberEffectVec2& value)
{
    document.set(key + ".x", std::to_string(value.x));
    document.set(key + ".y", std::to_string(value.y));
}

inline void loadVec2(const ProjectDocument& document, const std::string& key, PNGTuberEffectVec2& value)
{
    value.x = readFloat(document, key + ".x", value.x);
    value.y = readFloat(document, key + ".y", value.y);
}

inline void saveState(ProjectDocument& document, const std::string& prefix, const PNGTuberEffectState& state)
{
    saveVec2(document, prefix + ".bounceMin", state.bounceMin);
    saveVec2(document, prefix + ".squishMin", state.squishMin);
    saveVec2(document, prefix + ".frequencyMin", state.frequencyMin);
    saveVec2(document, prefix + ".amplitudeMin", state.amplitudeMin);
    document.set(prefix + ".rotationalMin", std::to_string(state.rotationalMin));
    document.set(prefix + ".dragMin", std::to_string(state.dragMin));

    saveVec2(document, prefix + ".bounceMax", state.bounceMax);
    saveVec2(document, prefix + ".squishMax", state.squishMax);
    saveVec2(document, prefix + ".frequencyMax", state.frequencyMax);
    saveVec2(document, prefix + ".amplitudeMax", state.amplitudeMax);
    document.set(prefix + ".rotationalMax", std::to_string(state.rotationalMax));
    document.set(prefix + ".dragMax", std::to_string(state.dragMax));

    document.set(prefix + ".color.r", std::to_string(state.color.r));
    document.set(prefix + ".color.g", std::to_string(state.color.g));
    document.set(prefix + ".color.b", std::to_string(state.color.b));
    document.set(prefix + ".color.a", std::to_string(state.color.a));
}

inline void loadState(const ProjectDocument& document, const std::string& prefix, PNGTuberEffectState& state)
{
    loadVec2(document, prefix + ".bounceMin", state.bounceMin);
    loadVec2(document, prefix + ".squishMin", state.squishMin);
    loadVec2(document, prefix + ".frequencyMin", state.frequencyMin);
    loadVec2(document, prefix + ".amplitudeMin", state.amplitudeMin);
    state.rotationalMin = readFloat(document, prefix + ".rotationalMin", state.rotationalMin);
    state.dragMin = readFloat(document, prefix + ".dragMin", state.dragMin);

    loadVec2(document, prefix + ".bounceMax", state.bounceMax);
    loadVec2(document, prefix + ".squishMax", state.squishMax);
    loadVec2(document, prefix + ".frequencyMax", state.frequencyMax);
    loadVec2(document, prefix + ".amplitudeMax", state.amplitudeMax);
    state.rotationalMax = readFloat(document, prefix + ".rotationalMax", state.rotationalMax);
    state.dragMax = readFloat(document, prefix + ".dragMax", state.dragMax);

    state.color.r = readFloat(document, prefix + ".color.r", state.color.r);
    state.color.g = readFloat(document, prefix + ".color.g", state.color.g);
    state.color.b = readFloat(document, prefix + ".color.b", state.color.b);
    state.color.a = readFloat(document, prefix + ".color.a", state.color.a);
}

inline void save(ProjectDocument& document, const PNGTuberEffects& effects)
{
    saveState(document, "png.effects.silent", effects.silent);
    saveState(document, "png.effects.talking", effects.talking);
    saveState(document, "png.effects.expression", effects.expression);
    document.set("png.effects.expressionEnabled", effects.expressionEnabled ? "1" : "0");
    document.set("png.effects.expressionThreshold", std::to_string(effects.expressionThreshold));
}

inline void load(const ProjectDocument& document, PNGTuberEffects& effects)
{
    loadState(document, "png.effects.silent", effects.silent);
    loadState(document, "png.effects.talking", effects.talking);
    loadState(document, "png.effects.expression", effects.expression);
    effects.expressionEnabled = readBool(document, "png.effects.expressionEnabled", effects.expressionEnabled);
    effects.expressionThreshold = readFloat(document, "png.effects.expressionThreshold", effects.expressionThreshold);
}
}