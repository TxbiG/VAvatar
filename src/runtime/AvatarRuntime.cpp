#include "AvatarRuntime.h"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace {
std::string resolved(const std::string& folder, const std::string& path) {
    if (path.empty()) return {};
    std::filesystem::path p(path);
    return (p.is_absolute() || folder.empty()) ? p.string() : (std::filesystem::path(folder) / p).string();
}

int pngIndex(const PNGTuberAssetSlots& slots, const PNGTuberPose& pose) {
    if (pose.useExpressionSprite && !slots.expressionImage.empty()) return 4;
    if (pose.useBlinkSprite && !slots.blinkImage.empty()) return 3;
    if (pose.useOpenMouthSprite && !slots.mouthOpenImage.empty()) return 1;
    if (!slots.mouthClosedImage.empty()) return 2;
    return slots.baseImage.empty() ? -1 : 0;
}

void drawTexture(
    const MossTexture& texture,
    const AvatarRenderContext& context,
    ImVec2 offset = {},
    float scale = 1.0f,
    float opacity = 1.0f,
    float degrees = 0.0f,
    ImVec2 axisScale = ImVec2(1.0f, 1.0f),
    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
{
    if (!context.drawList || !texture.id || texture.width <= 0 || texture.height <= 0) return;

    const ImVec2 center(
        (context.min.x + context.max.x) * 0.5f + offset.x,
        (context.min.y + context.max.y) * 0.5f + offset.y);
    const float baseHeight = (context.max.y - context.min.y) * 0.9f * scale;
    const float height = baseHeight * axisScale.y;
    const float width = baseHeight * static_cast<float>(texture.width) / static_cast<float>(texture.height) * axisScale.x;
    const float angle = degrees * 0.0174532925f;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    auto point = [&](float x, float y) {
        return ImVec2(center.x + x * cosine - y * sine, center.y + x * sine + y * cosine);
    };
    const ImU32 tint = ImGui::ColorConvertFloat4ToU32(ImVec4(
        std::clamp(color.x, 0.0f, 1.0f),
        std::clamp(color.y, 0.0f, 1.0f),
        std::clamp(color.z, 0.0f, 1.0f),
        std::clamp(color.w * opacity, 0.0f, 1.0f)));
    context.drawList->AddImageQuad(
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture.id)),
        point(-width * 0.5f, -height * 0.5f),
        point(width * 0.5f, -height * 0.5f),
        point(width * 0.5f, height * 0.5f),
        point(-width * 0.5f, height * 0.5f),
        ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1), tint);
}

float mixValue(float minimum, float maximum, float amount) {
    const float t = std::clamp(amount, 0.0f, 1.0f);
    return minimum + (maximum - minimum) * t;
}

PNGTuberEffectVec2 mixValue(PNGTuberEffectVec2 minimum, PNGTuberEffectVec2 maximum, float amount) {
    return {mixValue(minimum.x, maximum.x, amount), mixValue(minimum.y, maximum.y, amount)};
}

const PNGTuberEffectState& activeEffect(
    const PNGTuberEffects& effects,
    const PNGTuberPose& pose,
    float& amount)
{
    if (effects.expressionEnabled && pose.expressionAmount >= effects.expressionThreshold) {
        amount = pose.expressionAmount;
        return effects.expression;
    }
    if (pose.talkAmount > 0.18f) {
        amount = pose.talkAmount;
        return effects.talking;
    }
    amount = 0.5f;
    return effects.silent;
}

float normalizedLive2DParameter(const Live2DRig& rig, const std::string& name)
{
    if (name.empty()) return 0.0f;
    const float value = rig.parameter(name);
    return std::clamp(std::abs(value) <= 1.0f ? value : value / 30.0f, -1.0f, 1.0f);
}

ImVec2 applyLive2DDeformer(
    const Live2DRig& rig,
    const std::string& name,
    ImVec2 point,
    const ImVec2& center,
    float width,
    float height,
    int depth = 0)
{
    if (name.empty() || depth >= 16) return point;
    const Live2DDeformer* deformer = rig.findDeformer(name);
    if (!deformer) return point;

    const ImVec2 pivot(
        center.x + (deformer->pivotX - 0.5f) * width,
        center.y + (deformer->pivotY - 0.5f) * height);
    const float scaleX = std::max(0.01f, deformer->scaleX);
    const float scaleY = std::max(0.01f, deformer->scaleY);
    const float angle =
        (deformer->rotation +
         normalizedLive2DParameter(rig, deformer->rotationParameter) * deformer->rotationAmount) *
        0.0174532925f;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const float localX = (point.x - pivot.x) * scaleX;
    const float localY = (point.y - pivot.y) * scaleY;
    point = ImVec2(
        pivot.x + localX * cosine - localY * sine +
            deformer->offsetX +
            normalizedLive2DParameter(rig, deformer->translateXParameter) * deformer->translateXAmount,
        pivot.y + localX * sine + localY * cosine +
            deformer->offsetY +
            normalizedLive2DParameter(rig, deformer->translateYParameter) * deformer->translateYAmount);
    return applyLive2DDeformer(rig, deformer->parent, point, center, width, height, depth + 1);
}

ImVec2 live2DLayerPoint(
    const Live2DRig& rig,
    const Live2DLayer& layer,
    const MossTexture& texture,
    const AvatarRenderContext& context,
    float u,
    float v,
    float physicsX,
    float physicsY)
{
    const float height = (context.max.y - context.min.y) * 0.9f * layer.scale;
    const float width = height * static_cast<float>(texture.width) / std::max(1.0f, static_cast<float>(texture.height));
    const float headX = rig.parameter("ParamAngleX") / 30.0f;
    const float headY = rig.parameter("ParamAngleY") / 30.0f;
    const ImVec2 center(
        (context.min.x + context.max.x) * 0.5f +
            headX * layer.parallaxX +
            (0.5f - layer.pivotX) * width +
            physicsX,
        (context.min.y + context.max.y) * 0.5f +
            headY * layer.parallaxY +
            (0.5f - layer.pivotY) * height +
            physicsY);

    const float nx = u * 2.0f - 1.0f;
    const float ny = v * 2.0f - 1.0f;
    float localX = (u - 0.5f) * width;
    float localY = (v - 0.5f) * height;
    localX += normalizedLive2DParameter(rig, layer.warpParameterX) * layer.warpStrengthX * (1.0f - ny * ny);
    localY += normalizedLive2DParameter(rig, layer.warpParameterY) * layer.warpStrengthY * (1.0f - nx * nx);

    const float angle = layer.rotation * 0.0174532925f;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    ImVec2 point(
        center.x + localX * cosine - localY * sine,
        center.y + localX * sine + localY * cosine);
    return applyLive2DDeformer(rig, layer.deformer, point, center, width, height);
}

bool live2DLayerBounds(
    const Live2DRig& rig,
    const Live2DLayer& layer,
    const MossTexture& texture,
    const AvatarRenderContext& context,
    float physicsX,
    float physicsY,
    ImVec2& minimum,
    ImVec2& maximum)
{
    if (!texture.id) return false;
    const std::array<ImVec2, 4> points = {
        live2DLayerPoint(rig, layer, texture, context, 0.0f, 0.0f, physicsX, physicsY),
        live2DLayerPoint(rig, layer, texture, context, 1.0f, 0.0f, physicsX, physicsY),
        live2DLayerPoint(rig, layer, texture, context, 1.0f, 1.0f, physicsX, physicsY),
        live2DLayerPoint(rig, layer, texture, context, 0.0f, 1.0f, physicsX, physicsY)
    };
    minimum = maximum = points[0];
    for (const ImVec2& point : points) {
        minimum.x = std::min(minimum.x, point.x);
        minimum.y = std::min(minimum.y, point.y);
        maximum.x = std::max(maximum.x, point.x);
        maximum.y = std::max(maximum.y, point.y);
    }
    return true;
}
void rotationMatrix(float yaw, float pitch, float roll, float* matrix) {
    const float cy = std::cos(yaw), sy = std::sin(yaw), cx = std::cos(pitch), sx = std::sin(pitch), cz = std::cos(roll), sz = std::sin(roll);
    std::fill(matrix, matrix + 16, 0.0f);
    matrix[0] = cy * cz + sy * sx * sz; matrix[1] = cx * sz; matrix[2] = -sy * cz + cy * sx * sz;
    matrix[4] = -cy * sz + sy * sx * cz; matrix[5] = cx * cz; matrix[6] = sy * sz + cy * sx * cz;
    matrix[8] = sy * cx; matrix[9] = -sx; matrix[10] = cy * cx; matrix[15] = 1.0f;
}
}

bool PNGTuberRuntime::load(const std::string& projectFolder) {
    unload();
    folder = projectFolder;
    const std::array<std::string, 5> paths = {
        slots.baseImage, slots.mouthOpenImage, slots.mouthClosedImage, slots.blinkImage, slots.expressionImage
    };
    for (size_t i = 0; i < paths.size(); ++i) {
        if (!paths[i].empty()) MossTextureLoader::loadFromFile(resolved(folder, paths[i]), textures[i]);
    }
    ready = std::any_of(textures.begin(), textures.end(), [](const MossTexture& texture) { return texture.id != 0; });
    status = ready ? "Ready" : "No readable PNG assets";
    return ready;
}

void PNGTuberRuntime::unload() {
    for (auto& texture : textures) MossTextureLoader::destroy(texture);
    ready = false;
    elapsedSeconds = 0.0f;
    followedX = 0.0f;
    followedY = 0.0f;
}

void PNGTuberRuntime::applyTrackingPose(const AvatarTrackingPose& trackingPose) {
    pose = trackingPose.png;
    pose.useExpressionSprite =
        effects.expressionEnabled &&
        pose.expressionAmount >= effects.expressionThreshold;
}

void PNGTuberRuntime::update(float deltaSeconds) {
    const float dt = std::max(deltaSeconds, 0.0f);
    elapsedSeconds += dt;

    float amount = 0.0f;
    const PNGTuberEffectState& state = activeEffect(effects, pose, amount);
    const float drag = mixValue(state.dragMin, state.dragMax, amount);
    const float followRate = std::max(0.1f, 18.0f / (1.0f + std::max(0.0f, drag)));
    const float follow = 1.0f - std::exp(-followRate * dt);
    followedX = mixValue(followedX, pose.offsetX, follow);
    followedY = mixValue(followedY, pose.offsetY, follow);
}

void PNGTuberRuntime::render(const AvatarRenderContext& context) {
    const int index = pngIndex(slots, pose);
    if (index < 0) return;

    float amount = 0.0f;
    const PNGTuberEffectState& state = activeEffect(effects, pose, amount);
    const PNGTuberEffectVec2 bounce = mixValue(state.bounceMin, state.bounceMax, amount);
    const PNGTuberEffectVec2 squish = mixValue(state.squishMin, state.squishMax, amount);
    const PNGTuberEffectVec2 frequency = mixValue(state.frequencyMin, state.frequencyMax, amount);
    const PNGTuberEffectVec2 amplitude = mixValue(state.amplitudeMin, state.amplitudeMax, amount);
    const float phaseX = elapsedSeconds * frequency.x * 6.2831853f;
    const float phaseY = elapsedSeconds * frequency.y * 6.2831853f;
    const float waveX = std::sin(phaseX);
    const float waveY = std::sin(phaseY);
    const float rotation =
        mixValue(state.rotationalMin, state.rotationalMax, amount) *
        std::sin((phaseX + phaseY) * 0.5f);

    drawTexture(
        textures[static_cast<size_t>(index)],
        context,
        ImVec2(
            followedX + waveX * (bounce.x + amplitude.x),
            followedY - waveY * (bounce.y + amplitude.y)),
        1.0f,
        1.0f,
        pose.tiltDegrees + rotation,
        ImVec2(
            std::max(0.05f, 1.0f + squish.x * waveX),
            std::max(0.05f, 1.0f - squish.y * waveY)),
        ImVec4(state.color.r, state.color.g, state.color.b, state.color.a));
}

bool Live2DRuntime::load(const std::string& projectFolder) {
    unload();
    folder = projectFolder;
    textures.resize(rig.layers.size());
    physicsStates.resize(rig.layers.size());
    for (size_t i = 0; i < rig.layers.size(); ++i) {
        if (!rig.layers[i].imagePath.empty()) {
            MossTextureLoader::loadFromFile(resolved(folder, rig.layers[i].imagePath), textures[i]);
        }
    }
    ready = std::any_of(textures.begin(), textures.end(), [](const MossTexture& texture) { return texture.id != 0; });
    status = ready ? "Ready" : "No readable Live2D layers";
    return ready;
}

void Live2DRuntime::unload() {
    for (auto& texture : textures) MossTextureLoader::destroy(texture);
    textures.clear();
    physicsStates.clear();
    ready = false;
}

void Live2DRuntime::applyTrackingPose(const AvatarTrackingPose& trackingPose) {
    pose = trackingPose;
}

void Live2DRuntime::update(float deltaSeconds) {
    const float dt = std::clamp(deltaSeconds, 0.0f, 1.0f / 20.0f);
    rig.evaluate(pose, dt);
    if (physicsStates.size() != rig.layers.size()) physicsStates.resize(rig.layers.size());

    for (size_t i = 0; i < rig.layers.size(); ++i) {
        const Live2DLayer& layer = rig.layers[i];
        LayerPhysicsState& state = physicsStates[i];
        if (!layer.physicsEnabled) {
            state = {};
            continue;
        }

        const float input = normalizedLive2DParameter(rig, layer.physicsParameter);
        const float targetX = input * layer.physicsStrengthX;
        const float targetY = input * layer.physicsStrengthY + layer.physicsGravity;
        const float stiffness = std::max(0.0f, layer.physicsStiffness);
        const float damping = std::max(0.0f, layer.physicsDamping);
        state.velocityX += ((targetX - state.x) * stiffness - state.velocityX * damping) * dt;
        state.velocityY += ((targetY - state.y) * stiffness - state.velocityY * damping) * dt;
        state.x += state.velocityX * dt;
        state.y += state.velocityY * dt;
    }
}

void Live2DRuntime::render(const AvatarRenderContext& context) {
    if (!context.drawList) return;

    std::vector<size_t> order(rig.layers.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [this](size_t a, size_t b) {
        return rig.layers[a].depth < rig.layers[b].depth;
    });

    for (size_t layerIndex : order) {
        if (layerIndex >= textures.size()) continue;
        const Live2DLayer& layer = rig.layers[layerIndex];
        const MossTexture& texture = textures[layerIndex];
        if (!layer.visible || !texture.id) continue;

        const float visibility = layer.visibilityParameter.empty() ? 0.0f : rig.parameter(layer.visibilityParameter);
        if (!layer.visibilityParameter.empty() &&
            (visibility < layer.visibilityMin || visibility > layer.visibilityMax)) {
            continue;
        }

        bool clipped = false;
        if (!layer.maskLayer.empty()) {
            for (size_t maskIndex = 0; maskIndex < rig.layers.size(); ++maskIndex) {
                if (rig.layers[maskIndex].name != layer.maskLayer || maskIndex >= textures.size()) continue;
                ImVec2 clipMin;
                ImVec2 clipMax;
                const float maskPhysicsX = maskIndex < physicsStates.size() ? physicsStates[maskIndex].x : 0.0f;
                const float maskPhysicsY = maskIndex < physicsStates.size() ? physicsStates[maskIndex].y : 0.0f;
                if (live2DLayerBounds(rig, rig.layers[maskIndex], textures[maskIndex], context, maskPhysicsX, maskPhysicsY, clipMin, clipMax)) {
                    context.drawList->PushClipRect(clipMin, clipMax, true);
                    clipped = true;
                }
                break;
            }
        }

        const int columns = std::clamp(layer.meshColumns, 1, 32);
        const int rows = std::clamp(layer.meshRows, 1, 32);
        const float physicsX = layerIndex < physicsStates.size() ? physicsStates[layerIndex].x : 0.0f;
        const float physicsY = layerIndex < physicsStates.size() ? physicsStates[layerIndex].y : 0.0f;
        const ImU32 tint = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, std::clamp(layer.opacity, 0.0f, 1.0f)));
        const ImTextureID textureId = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture.id));

        for (int row = 0; row < rows; ++row) {
            const float v0 = static_cast<float>(row) / static_cast<float>(rows);
            const float v1 = static_cast<float>(row + 1) / static_cast<float>(rows);
            for (int column = 0; column < columns; ++column) {
                const float u0 = static_cast<float>(column) / static_cast<float>(columns);
                const float u1 = static_cast<float>(column + 1) / static_cast<float>(columns);
                context.drawList->AddImageQuad(
                    textureId,
                    live2DLayerPoint(rig, layer, texture, context, u0, v0, physicsX, physicsY),
                    live2DLayerPoint(rig, layer, texture, context, u1, v0, physicsX, physicsY),
                    live2DLayerPoint(rig, layer, texture, context, u1, v1, physicsX, physicsY),
                    live2DLayerPoint(rig, layer, texture, context, u0, v1, physicsX, physicsY),
                    ImVec2(u0, v0), ImVec2(u1, v0), ImVec2(u1, v1), ImVec2(u0, v1), tint);
            }
        }

        if (clipped) context.drawList->PopClipRect();
    }
}

bool Model3DRuntime::load(const std::string& projectFolder) { unload(); folder = projectFolder; model = Moss_ModelLoad(resolved(folder, rig.modelPath).c_str()); status = model ? "Ready" : Moss_ModelGetLastLoadErrorMessage(); return model != nullptr; }
void Model3DRuntime::unload() { if (model) Moss_ModelRemove(model); model = nullptr; }
void Model3DRuntime::applyTrackingPose(const AvatarTrackingPose& trackingPose) { rig.evaluate(trackingPose); }
void Model3DRuntime::update(float) {
    if (!model) return;
    float matrix[16];
    rotationMatrix(rig.evaluatedPose.headYaw * 0.0174533f, rig.evaluatedPose.headPitch * 0.0174533f, rig.evaluatedPose.headRoll * 0.0174533f, matrix);
    const int bone = Moss_ModelFindBone(model, rig.headBone.c_str());
    if (bone >= 0) Moss_ModelSetBoneTransform(model, static_cast<uint32_t>(bone), matrix);
    const std::array<std::pair<std::string, float>, 4> morphs = {{
        {rig.jawBlendshape, rig.evaluatedPose.jawOpen},
        {rig.blinkLeftBlendshape, rig.evaluatedPose.blinkLeft},
        {rig.blinkRightBlendshape, rig.evaluatedPose.blinkRight},
        {rig.smileBlendshape, rig.evaluatedPose.smile}
    }};
    for (const auto& morph : morphs) {
        const int index = Moss_ModelFindMorphTarget(model, morph.first.c_str());
        if (index >= 0) Moss_ModelSetMorphWeight(model, static_cast<uint32_t>(index), morph.second);
    }
}
void Model3DRuntime::render(const AvatarRenderContext& context) { if (!context.drawList) return; const ImVec2 center((context.min.x + context.max.x) * .5f, (context.min.y + context.max.y) * .5f); context.drawList->AddCircle(center, 80, model ? IM_COL32(110, 220, 150, 255) : IM_COL32(220, 100, 100, 255), 48, 4); context.drawList->AddText(ImVec2(center.x - 55, center.y - 8), IM_COL32_WHITE, status.c_str()); }
std::vector<std::string> Model3DRuntime::boneNames() const { std::vector<std::string> result; if (model) for (uint32_t i = 0; i < model->bone_count; ++i) result.emplace_back(model->bones[i].name); return result; }
std::vector<std::string> Model3DRuntime::morphTargetNames() const { std::vector<std::string> result; if (model) for (uint32_t i = 0; i < model->morph_target_count; ++i) result.emplace_back(model->morph_targets[i].name); return result; }
std::vector<std::string> Model3DRuntime::validateBindings() const { std::vector<std::string> result; if (!model) { result.emplace_back(status); return result; } if (Moss_ModelFindBone(model, rig.headBone.c_str()) < 0) result.emplace_back("Missing head bone: " + rig.headBone); for (const auto& name : {rig.jawBlendshape, rig.blinkLeftBlendshape, rig.blinkRightBlendshape, rig.smileBlendshape}) if (Moss_ModelFindMorphTarget(model, name.c_str()) < 0) result.emplace_back("Missing morph target: " + name); return result; }

std::unique_ptr<IAvatarRuntime> CreateAvatarRuntime(AvatarRigType type) {
    if (type == AvatarRigType::PNGTuber) return std::make_unique<PNGTuberRuntime>();
    if (type == AvatarRigType::Live2D) return std::make_unique<Live2DRuntime>();
    return std::make_unique<Model3DRuntime>();
}