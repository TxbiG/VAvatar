#ifndef VSTAGE_H
#define VSTAGE_H

#include "../core/avatar/AvatarTrackingMapper.h"
#include "../core/avatar/AvatarRigs.h"
#include "../core/avatar/PNGTuberAssets.h"
#include "../core/tracking/Audio/AudioTracking.h"
#include "../core/tracking/Camera/OpenCV/CameraTrackingOpenCV.h"
#include "../core/tracking/Camera/TrackingCamera.h"
#include "../editor/app.h"
#include "../renderer/MossTexture.h"
#include "AvatarRuntime.h"
#include "../core/project/ProjectDocument.h"

#include <array>
#include <string>

class VStage {
public:
    // Run the transparent/capture-friendly Stage window.
    StageResult init();

private:
    // Keep voice and camera tracking services in sync with Stage settings.
    void updateTrackingPreview();

    // Convert current tracking data into the active avatar output pose.
    void updateAvatarPose();

    // Draw the stream/runtime avatar output.
    void renderStage();

    // Draw optional controls over the Stage output.
    void renderControlOverlay();
    void rebuildAvatarRuntime();

    // Load a saved .vavatar project into the runtime window.
    void loadProject(const std::string& path);
    bool saveRuntimeSettings();

    // Load PNGTuber image slots through the shared texture loader.
    void loadPNGTuberTextures();

    // Release PNGTuber runtime textures.
    void destroyPNGTuberTextures();

    // Return the texture selected by the current PNGTuber tracking pose.
    MossTexture* activePNGTuberTexture();

    App app;
    AudioTracking audioTracking;
    TrackingCamera cameraTracking;
    CameraTrackingOpenCV opencvTracker;
    AvatarTrackingMapper trackingMapper;
    AvatarTrackingPose avatarPose;
    Live2DRig live2DRig;
    Model3DRig model3DRig;
    std::unique_ptr<IAvatarRuntime> avatarRuntime;
    PNGTuberAssetSlots pngSlots;
    PNGTuberEffects pngEffects;
    std::array<MossTexture, 5> pngTextures;
    AvatarRigType activeRigType = AvatarRigType::PNGTuber;
    StageResult result = StageResult::Exit;
    std::string projectPath = "Projects/UntitledAvatar/project.vavatar";
    std::string projectFolder = "Projects/UntitledAvatar";
    bool voiceTrackingEnabled = true;
    bool cameraTrackingEnabled = true;
    bool simulateCameraTracking = true;
    bool useRealCameraTracking = false;
    int cameraIndex = 0;
    std::string faceCascadePath = "res/data/haarcascade_frontalface_default.xml";
    std::string lbfModelPath = "res/data/lbfmodel.yaml";
    bool showControls = true;
    bool chromaKeyEnabled = false;
    bool alwaysOnTop = false;
    bool borderless = false;
    float chromaKeyColor[4] = {0.0f,1.0f,0.0f,1.0f};
    int targetFps = 60;
    float runtimeFps = 0.0f;
    float voiceThreshold = 0.025f;
    float voiceMouthResponse = 1.0f;
    CameraTrackingCalibration cameraCalibration;
};

#endif
