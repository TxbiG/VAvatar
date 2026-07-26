#ifndef VEDITOR_H
#define VEDITOR_H

#include "app.h"
#include "../core/avatar/AvatarTrackingMapper.h"
#include "../core/avatar/AvatarRigs.h"
#include "../core/avatar/PNGTuberAssets.h"
#include "../core/tracking/Audio/AudioTracking.h"
#include "../core/tracking/Camera/OpenCV/CameraTrackingOpenCV.h"
#include "../core/tracking/Camera/TrackingCamera.h"
#include "../runtime/AvatarRuntime.h"
#include "../core/project/ProjectDocument.h"

#include <array>
#include <string>
#include <vector>

class VEditor
{
public:
    // Run the Editor window until the user navigates away or exits.
    EditorResult init();

private:
    // Draw the full Editor layout for scene, tracking, assets, and output.
    void GUI();

    // Keep tracking services running or stopped according to Editor settings.
    void updateTrackingPreview();

    // Convert current tracking data into the active avatar output pose.
    void updateAvatarPose();

    // Draw the top menu bar and app navigation actions.
    void showMainMenu();

    // Draw the main avatar preview area.
    void showSceneView();

    // Draw avatar-type-specific tuning controls.
    void showProperties();

    // Draw voice, camera, OpenCV, and calibration controls.
    void showTrackingPanel();

    // Draw PNGTuber asset-slot and project save/load controls.
    void showPNGTuberAssetPanel();

    // Draw Live2D / 2.5D parameter binding controls.
    void showLive2DBindingPanel();
    void showModel3DRigPanel();
    void rebuildAvatarRuntime();

    // Draw the mapped tracking output values for the selected avatar type.
    void showRigOutputPanel();

    // Draw the Editor status/console section.
    void showConsole();

    // Refresh the list of available camera device indices.
    void refreshCameraDevices();

    // Validate and apply camera calibration values to the OpenCV tracker.
    void applyCameraCalibration();

    // Load all configured PNGTuber image slots into shared textures.
    void loadPNGTuberTextures();

    // Release PNGTuber preview textures.
    void destroyPNGTuberTextures();

    // Return the texture selected by the current PNGTuber tracking pose.
    Texture* activePNGTuberTexture();

    // Refresh named calibration presets from the project Calibration folder.
    void refreshCalibrationProfiles();

    // Copy project state strings into ImGui input buffers.
    void copyProjectStringsToBuffers();

    // Save the current editor project to a .vavatar file.
    bool saveProject(const std::string& path) const;

    // Load editor state from a .vavatar file.
    bool loadProject(const std::string& path);

    // Save voice and camera calibration to a .vcal preset.
    bool saveCalibrationPreset(const std::string& path) const;

    // Load voice and camera calibration from a .vcal preset.
    bool loadCalibrationPreset(const std::string& path);

    App app;
    AudioTracking audioTracking;
    TrackingCamera cameraTracking;
    CameraTrackingOpenCV opencvTracker;
    AvatarTrackingMapper trackingMapper;
    AvatarTrackingPose avatarPose;
    PNGTuberAssetSlots pngSlots;
    PNGTuberEffects pngEffects;
    std::array<Texture, 5> pngTextures;
    Live2DRig live2DRig;
    Model3DRig model3DRig;
    std::unique_ptr<IAvatarRuntime> avatarRuntime;
    ProjectDocument projectDocument;
    std::vector<ProjectAssetIssue> missingAssetIssues;
    std::vector<CameraDeviceInfo> cameraDevices;
    std::vector<std::string> calibrationProfiles;
    AvatarRigType activeRigType = AvatarRigType::PNGTuber;
    bool voiceTrackingEnabled = false;
    bool cameraTrackingEnabled = false;
    bool simulateCameraTracking = true;
    bool useRealCameraTracking = false;
    int cameraIndex = 0;
    std::string faceCascadePath = "res/data/haarcascade_frontalface_default.xml";
    std::string lbfModelPath = "res/data/lbfmodel.yaml";
    std::string projectFolder = "Projects/UntitledAvatar";
    EditorResult result = EditorResult::Exit;
    float voiceThreshold = 0.025f;
    float voiceMouthResponse = 1.0f;
    CameraTrackingCalibration cameraCalibration;
    std::array<char, 260> faceCascadePathBuffer{};
    std::array<char, 260> lbfModelPathBuffer{};
    std::array<char, 260> projectPathBuffer{};
    std::array<char, 260> projectFolderBuffer{};
    std::array<char, 260> calibrationPresetPathBuffer{};
    std::array<char, 64> calibrationProfileNameBuffer{};
    std::array<char, 260> pngBaseBuffer{};
    std::array<char, 260> pngMouthOpenBuffer{};
    std::array<char, 260> pngMouthClosedBuffer{};
    std::array<char, 260> pngBlinkBuffer{};
    std::array<char, 260> pngExpressionBuffer{};
    std::array<std::array<char, 64>, 6> live2DParameterBuffers{};
    std::array<std::array<char, 64>, 6> live2DSourceBuffers{};
    std::array<std::array<char, 32>, 6> live2DCurveBuffers{};
    std::array<char, 260> modelPathBuffer{};
    std::array<char, 128> rigSearchBuffer{};
    std::array<char, 64> headBoneBuffer{};
    std::array<char, 64> neckBoneBuffer{};
    std::array<char, 64> chestBoneBuffer{};
    std::array<char, 64> jawBlendshapeBuffer{};
    std::array<char, 64> blinkLeftBlendshapeBuffer{};
    std::array<char, 64> blinkRightBlendshapeBuffer{};
    std::array<char, 64> smileBlendshapeBuffer{};
    std::string projectStatus = "No project loaded";
};

#endif
