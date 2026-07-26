#pragma once

#include "../core/avatar/AvatarRigs.h"
#include "../core/avatar/PNGTuberAssets.h"
#include "../renderer/MossTexture.h"

#include <Moss/Moss_Renderer.h>
#include <imgui.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct AvatarRenderContext {
    ImDrawList* drawList = nullptr;
    ImVec2 min{};
    ImVec2 max{};
};

class IAvatarRuntime {
public:
    virtual ~IAvatarRuntime() = default;
    virtual bool load(const std::string& projectFolder) = 0;
    virtual void unload() = 0;
    virtual void applyTrackingPose(const AvatarTrackingPose& pose) = 0;
    virtual void update(float deltaSeconds) = 0;
    virtual void render(const AvatarRenderContext& context) = 0;
    virtual const std::string& getStatus() const = 0;
    virtual bool isReady() const = 0;
};

class PNGTuberRuntime final : public IAvatarRuntime {
public:
    PNGTuberAssetSlots slots;
    PNGTuberEffects effects;
    ~PNGTuberRuntime() override { unload(); }
    bool load(const std::string& projectFolder) override;
    void unload() override;
    void applyTrackingPose(const AvatarTrackingPose& pose) override;
    void update(float deltaSeconds) override;
    void render(const AvatarRenderContext& context) override;
    const std::string& getStatus() const override { return status; }
    bool isReady() const override { return ready; }
private:
    std::array<MossTexture, 5> textures;
    PNGTuberPose pose;
    std::string folder;
    std::string status = "Not loaded";
    bool ready = false;
    float elapsedSeconds = 0.0f;
    float followedX = 0.0f;
    float followedY = 0.0f;
};

class Live2DRuntime final : public IAvatarRuntime {
public:
    Live2DRig rig;
    ~Live2DRuntime() override { unload(); }
    bool load(const std::string& projectFolder) override;
    void unload() override;
    void applyTrackingPose(const AvatarTrackingPose& pose) override;
    void update(float deltaSeconds) override;
    void render(const AvatarRenderContext& context) override;
    const std::string& getStatus() const override { return status; }
    bool isReady() const override { return ready; }
private:
    struct LayerPhysicsState { float x=0.0f, y=0.0f, velocityX=0.0f, velocityY=0.0f; };
    std::vector<MossTexture> textures;
    std::vector<LayerPhysicsState> physicsStates;
    AvatarTrackingPose pose;
    std::string folder;
    std::string status = "No layers";
    bool ready = false;
};

class Model3DRuntime final : public IAvatarRuntime {
public:
    Model3DRig rig;
    ~Model3DRuntime() override { unload(); }
    bool load(const std::string& projectFolder) override;
    void unload() override;
    void applyTrackingPose(const AvatarTrackingPose& pose) override;
    void update(float deltaSeconds) override;
    void render(const AvatarRenderContext& context) override;
    const std::string& getStatus() const override { return status; }
    bool isReady() const override { return model != nullptr; }
    std::vector<std::string> boneNames() const;
    std::vector<std::string> morphTargetNames() const;
    std::vector<std::string> validateBindings() const;
private:
    Moss_Model* model = nullptr;
    std::string folder;
    std::string status = "Not loaded";
};

std::unique_ptr<IAvatarRuntime> CreateAvatarRuntime(AvatarRigType type);
