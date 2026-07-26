#include "VEditor.h"
#include "../core/avatar/PNGTuberEffectsProject.h"
#include "../core/avatar/AvatarProjectSettings.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <Windows.h>
#include <GL/gl.h>
#include <commdlg.h>
#include <shlobj.h>

namespace {
// Copy a std::string into a fixed-size ImGui text buffer.
void copyStringToBuffer(const std::string& value, char* buffer, size_t bufferSize)
{
    if (!buffer || bufferSize == 0) {
        return;
    }

    std::snprintf(buffer, bufferSize, "%s", value.c_str());
}

// Show a native Windows file-open dialog and return the selected path.
std::string openFileDialog(const char* title, const char* filter)
{
    char path[MAX_PATH] = {};
    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrFilter = filter;
    dialog.lpstrTitle = title;
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameA(&dialog) ? std::string(path) : std::string{};
}

// Show a native Windows save dialog and return the selected path.
std::string saveFileDialog(const char* title, const char* filter, const char* defaultExtension)
{
    char path[MAX_PATH] = {};
    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrFilter = filter;
    dialog.lpstrTitle = title;
    dialog.lpstrDefExt = defaultExtension;
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    return GetSaveFileNameA(&dialog) ? std::string(path) : std::string{};
}

// Show a native Windows folder picker and return the selected folder.
std::string chooseFolderDialog(const char* title)
{
    BROWSEINFOA browse = {};
    browse.lpszTitle = title;
    browse.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    PIDLIST_ABSOLUTE item = SHBrowseForFolderA(&browse);
    if (!item) {
        return {};
    }

    char path[MAX_PATH] = {};
    const bool ok = SHGetPathFromIDListA(item, path) == TRUE;
    CoTaskMemFree(item);
    return ok ? std::string(path) : std::string{};
}

// Convert an absolute path to a project-folder-relative path when possible.
std::string makeRelativePath(const std::string& baseFolder, const std::string& path)
{
    if (baseFolder.empty() || path.empty()) {
        return path;
    }

    try {
        const std::filesystem::path base = std::filesystem::absolute(baseFolder);
        const std::filesystem::path target = std::filesystem::absolute(path);
        return std::filesystem::relative(target, base).generic_string();
    } catch (const std::exception&) {
        return path;
    }
}

// Resolve a project-relative path against the active project folder.
std::string resolveProjectPath(const std::string& baseFolder, const std::string& path)
{
    if (path.empty()) {
        return {};
    }

    std::filesystem::path filePath(path);
    if (filePath.is_absolute() || baseFolder.empty()) {
        return filePath.string();
    }

    return (std::filesystem::path(baseFolder) / filePath).string();
}

// Copy a selected asset into a project subfolder and return its stored path.
std::string importAssetToProjectFolder(const std::string& projectFolder, const std::string& sourcePath, const char* subfolder)
{
    if (projectFolder.empty() || sourcePath.empty()) {
        return sourcePath;
    }

    try {
        const std::filesystem::path source(sourcePath);
        if (!source.is_absolute() || !std::filesystem::exists(source)) {
            return sourcePath;
        }

        const std::filesystem::path destinationFolder = std::filesystem::path(projectFolder) / subfolder;
        std::filesystem::create_directories(destinationFolder);

        std::filesystem::path destination = destinationFolder / source.filename();
        int suffix = 1;
        while (std::filesystem::exists(destination) && !std::filesystem::equivalent(source, destination)) {
            destination = destinationFolder / (source.stem().string() + "_" + std::to_string(suffix++) + source.extension().string());
        }

        if (!std::filesystem::exists(destination)) {
            std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        }

        return makeRelativePath(projectFolder, destination.string());
    } catch (const std::exception&) {
        return makeRelativePath(projectFolder, sourcePath);
    }
}

// Return the text after the first '=' in a simple key=value line.
std::string valueAfterEquals(const std::string& line)
{
    const size_t separator = line.find('=');
    return separator == std::string::npos ? std::string{} : line.substr(separator + 1);
}

// Test whether a key=value line starts with a specific key prefix.
bool lineStartsWith(const std::string& line, const char* prefix)
{
    return line.rfind(prefix, 0) == 0;
}

// Return the PNGTuber slot path selected by the current tracking pose.
std::string activePngSpriteName(const PNGTuberAssetSlots& slots, const PNGTuberPose& pose)
{
    if (pose.useExpressionSprite && !slots.expressionImage.empty()) {
        return slots.expressionImage;
    }

    if (pose.useBlinkSprite && !slots.blinkImage.empty()) {
        return slots.blinkImage;
    }

    if (pose.useOpenMouthSprite && !slots.mouthOpenImage.empty()) {
        return slots.mouthOpenImage;
    }

    if (!slots.mouthClosedImage.empty()) {
        return slots.mouthClosedImage;
    }

    return slots.baseImage.empty() ? "No PNG asset selected" : slots.baseImage;
}

// Return the PNGTuber texture slot index selected by the current tracking pose.
int activePngTextureIndex(const PNGTuberAssetSlots& slots, const PNGTuberPose& pose)
{
    if (pose.useExpressionSprite && !slots.expressionImage.empty()) {
        return 4;
    }

    if (pose.useBlinkSprite && !slots.blinkImage.empty()) {
        return 3;
    }

    if (pose.useOpenMouthSprite && !slots.mouthOpenImage.empty()) {
        return 1;
    }

    if (!slots.mouthClosedImage.empty()) {
        return 2;
    }

    return slots.baseImage.empty() ? -1 : 0;
}

// Parse a float while preserving a fallback for malformed project values.
float readFloatValue(const std::string& value, float fallback)
{
    try {
        return std::stof(value);
    } catch (const std::exception&) {
        return fallback;
    }
}

void drawEffectStateEditor(const char* label, PNGTuberEffectState& state)
{
    if (!ImGui::TreeNode(label)) return;

    ImGui::PushID(label);
    ImGui::TextUnformatted("Minimum");
    ImGui::DragFloat2("Bounce##Min", &state.bounceMin.x, 0.1f, -200.0f, 200.0f);
    ImGui::DragFloat2("Squish##Min", &state.squishMin.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat2("Frequency##Min", &state.frequencyMin.x, 0.05f, 0.0f, 20.0f);
    ImGui::DragFloat2("Amplitude##Min", &state.amplitudeMin.x, 0.1f, -200.0f, 200.0f);
    ImGui::DragFloat("Rotational##Min", &state.rotationalMin, 0.1f, -180.0f, 180.0f);
    ImGui::DragFloat("Drag##Min", &state.dragMin, 0.1f, 0.0f, 100.0f);

    ImGui::Separator();
    ImGui::TextUnformatted("Maximum");
    ImGui::DragFloat2("Bounce##Max", &state.bounceMax.x, 0.1f, -200.0f, 200.0f);
    ImGui::DragFloat2("Squish##Max", &state.squishMax.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat2("Frequency##Max", &state.frequencyMax.x, 0.05f, 0.0f, 20.0f);
    ImGui::DragFloat2("Amplitude##Max", &state.amplitudeMax.x, 0.1f, -200.0f, 200.0f);
    ImGui::DragFloat("Rotational##Max", &state.rotationalMax, 0.1f, -180.0f, 180.0f);
    ImGui::DragFloat("Drag##Max", &state.dragMax, 0.1f, 0.0f, 100.0f);
    ImGui::ColorEdit4("Color", &state.color.r);
    ImGui::PopID();
    ImGui::TreePop();
}
}

EditorResult VEditor::init()
{
    if (!app.init("VAvatar Editor", 960, 640)) {
        return EditorResult::Exit;
    }

    projectPathBuffer.fill('\0');
    live2DRig.bindings = std::array<Live2DBinding, 6>{
        Live2DBinding{ "ParamAngleX", "headYaw", "Linear", -30.0f, 30.0f, 0.35f, false },
        Live2DBinding{ "ParamAngleY", "headPitch", "Ease", -30.0f, 30.0f, 0.35f, true },
        Live2DBinding{ "ParamAngleZ", "headRoll", "Linear", -30.0f, 30.0f, 0.35f, false },
        Live2DBinding{ "ParamMouthOpenY", "mouthOpen", "Responsive", 0.0f, 1.0f, 0.2f, false },
        Live2DBinding{ "ParamEyeLOpen", "leftEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false },
        Live2DBinding{ "ParamEyeROpen", "rightEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false }
    };
    copyProjectStringsToBuffers();
    refreshCameraDevices();
    refreshCalibrationProfiles();
    applyCameraCalibration();
    rebuildAvatarRuntime();

    while (app.beginFrame(0.08f, 0.08f, 0.09f, 1.0f)) {
        updateTrackingPreview();
        updateAvatarPose();
        GUI();
        app.endFrame();
    }

    audioTracking.stop();
    opencvTracker.stop();
    destroyPNGTuberTextures();
    return result;
}

void VEditor::GUI()
{
    showMainMenu();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float menuHeight = ImGui::GetFrameHeight();
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + menuHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - menuHeight));

    ImGui::Begin("Editor", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    ImGui::Columns(2, "editor_columns", true);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.68f);
    showSceneView();
    ImGui::NextColumn();
    showTrackingPanel();
    showPNGTuberAssetPanel();
    showLive2DBindingPanel();
    showModel3DRigPanel();
    showRigOutputPanel();
    showProperties();
    showConsole();
    ImGui::Columns(1);

    ImGui::End();
}

void VEditor::updateAvatarPose()
{
    TrackingInput input;
    input.face = cameraTracking.getState();
    input.voiceMouthOpen = voiceTrackingEnabled ? std::clamp(audioTracking.getMouthOpen() * voiceMouthResponse, 0.0f, 1.0f) : 0.0f;
    avatarPose = trackingMapper.map(activeRigType, input);
    avatarPose.png.useExpressionSprite = pngEffects.expressionEnabled && avatarPose.png.expressionAmount >= pngEffects.expressionThreshold;
    live2DRig.evaluate(avatarPose, std::max(ImGui::GetIO().DeltaTime, 1.0f / 240.0f));
    model3DRig.evaluate(avatarPose);
    if (avatarRuntime) { avatarRuntime->applyTrackingPose(avatarPose); avatarRuntime->update(std::max(ImGui::GetIO().DeltaTime, 1.0f / 240.0f)); }
}

void VEditor::updateTrackingPreview()
{
    if (voiceTrackingEnabled && !audioTracking.isRunning()) {
        audioTracking.start();
    } else if (!voiceTrackingEnabled && audioTracking.isRunning()) {
        audioTracking.stop();
    }

    audioTracking.setThreshold(voiceThreshold);

    if (cameraTrackingEnabled && useRealCameraTracking) {
        if (!opencvTracker.isRunning()) {
            opencvTracker.start(cameraIndex, faceCascadePath, lbfModelPath);
        }

        const FaceTrackingState faceState = opencvTracker.getState();
        if (faceState.foundPerson) {
            cameraTracking.updateFromFace(
                faceState.headYaw,
                faceState.headPitch,
                faceState.headRoll,
                std::max(std::clamp(audioTracking.getMouthOpen() * voiceMouthResponse, 0.0f, 1.0f), faceState.mouthOpen),
                faceState.leftBlink,
                faceState.rightBlink,
                faceState.smiling);
            cameraTracking.updateContinuousFace(
                faceState.leftEyeOpen, faceState.rightEyeOpen,
                faceState.smile, faceState.mouthForm,
                faceState.eyeGazeX, faceState.eyeGazeY,
                faceState.browLeft, faceState.browRight);
            cameraTracking.updateBody(faceState.shoulderYaw, faceState.shoulderPitch);
            cameraTracking.updateHands(faceState.detectedHands);
            cameraTracking.updateHandLandmarks(
                faceState.handLandmarks.data(), faceState.handLandmarkCount);
        } else {
            cameraTracking.clearFrame();
        }
    } else if (cameraTrackingEnabled && simulateCameraTracking) {
        if (opencvTracker.isRunning()) {
            opencvTracker.stop();
        }

        const float t = static_cast<float>(ImGui::GetTime());
        cameraTracking.updateFromFace(
            std::sin(t * 0.8f) * 18.0f,
            std::sin(t * 0.5f) * 8.0f,
            std::sin(t * 0.35f) * 5.0f,
            std::max(std::clamp(audioTracking.getMouthOpen() * voiceMouthResponse, 0.0f, 1.0f), (std::sin(t * 3.0f) + 1.0f) * 0.08f),
            std::fmod(t, 4.0f) < 0.15f,
            std::fmod(t + 0.04f, 4.0f) < 0.15f,
            std::sin(t * 0.6f) > 0.55f);
        const float eyeOpen = std::fmod(t, 4.0f) < 0.15f ? 0.0f : 1.0f;
        cameraTracking.updateContinuousFace(
            eyeOpen, eyeOpen,
            (std::sin(t * 0.6f) + 1.0f) * 0.5f,
            std::sin(t * 0.9f) * 0.4f,
            std::sin(t * 0.4f) * 0.5f,
            std::sin(t * 0.3f) * 0.35f,
            std::sin(t * 0.5f) * 0.4f,
            std::sin(t * 0.55f) * 0.4f);
        cameraTracking.updateBody(
            std::sin(t * 0.35f) * 8.0f,
            std::sin(t * 0.25f) * 5.0f);
        cameraTracking.updateHands(static_cast<int>((std::sin(t * 0.7f) + 1.0f) * 1.1f));
    } else {
        if (opencvTracker.isRunning()) {
            opencvTracker.stop();
        }
        cameraTracking.clearFrame();
    }
}

void VEditor::showMainMenu()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Back to Hub")) {
                result = EditorResult::BackToHub;
                app.close();
            }
            if (ImGui::MenuItem("Go Live")) {
                result = EditorResult::StartStage;
                app.close();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                result = EditorResult::Exit;
                app.close();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Reset Layout");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void VEditor::showSceneView()
{
    ImGui::Text("Scene View"); ImGui::Separator();
    const ImVec2 size=ImGui::GetContentRegionAvail();
    ImGui::InvisibleButton("viewport_canvas",ImVec2(size.x,size.y>180.0f?size.y:180.0f));
    const ImVec2 min=ImGui::GetItemRectMin(), max=ImGui::GetItemRectMax();
    ImGui::GetWindowDrawList()->AddRectFilled(min,max,IM_COL32(24,25,29,255));
    if(avatarRuntime) avatarRuntime->render({ImGui::GetWindowDrawList(),min,max});
    else ImGui::GetWindowDrawList()->AddText(ImVec2(min.x+12,min.y+12),IM_COL32_WHITE,"No avatar runtime");
}

void VEditor::rebuildAvatarRuntime()
{
    avatarRuntime=CreateAvatarRuntime(activeRigType);
    if(auto* p=dynamic_cast<PNGTuberRuntime*>(avatarRuntime.get())) { p->slots=pngSlots; p->effects=pngEffects; }
    if(auto* l=dynamic_cast<Live2DRuntime*>(avatarRuntime.get())) l->rig=live2DRig;
    if(auto* m=dynamic_cast<Model3DRuntime*>(avatarRuntime.get())) m->rig=model3DRig;
    avatarRuntime->load(projectFolder);
}

void VEditor::showProperties()
{
    if (activeRigType == AvatarRigType::PNGTuber) {
        ImGui::Text("PNGTuber State Effects");
        ImGui::Separator();
        drawEffectStateEditor("Silent", pngEffects.silent);
        drawEffectStateEditor("Talking", pngEffects.talking);
        ImGui::Checkbox("Enable expression effects", &pngEffects.expressionEnabled);
        if (pngEffects.expressionEnabled) {
            ImGui::SliderFloat("Expression threshold", &pngEffects.expressionThreshold, 0.0f, 1.0f);
            drawEffectStateEditor("Expression", pngEffects.expression);
        }
        if (ImGui::Button("Apply effect changes")) rebuildAvatarRuntime();
    } else if (activeRigType == AvatarRigType::Live2D) {
        ImGui::Text("Live2D Properties");
    } else {
        ImGui::Text("3D VTuber Properties");
    }
}

void VEditor::showTrackingPanel()
{
    ImGui::Text("Tracking");
    ImGui::Separator();

    const char* rigNames[] = { "PNGTuber", "Live2D", "3D VTuber" };
    int selectedRig = static_cast<int>(activeRigType);
    if (ImGui::Combo("Avatar type", &selectedRig, rigNames, 3)) {
        activeRigType = static_cast<AvatarRigType>(selectedRig);
        rebuildAvatarRuntime();
    }

    if (ImGui::Checkbox("Voice tracking", &voiceTrackingEnabled) && !voiceTrackingEnabled) {
        audioTracking.stop();
    }

    ImGui::SliderFloat("Voice threshold", &voiceThreshold, 0.001f, 0.12f, "%.3f");
    ImGui::SliderFloat("Mouth response", &voiceMouthResponse, 0.1f, 3.0f, "%.2f");
    ImGui::ProgressBar(std::clamp(audioTracking.getSmoothedLevel() * 12.0f, 0.0f, 1.0f), ImVec2(-1.0f, 0.0f), audioTracking.isActivated() ? "Speaking" : "Quiet");
    ImGui::Text("Mic: %s", audioTracking.getDeviceName().c_str());

    ImGui::Spacing();
    ImGui::Checkbox("Camera tracking", &cameraTrackingEnabled);
    ImGui::Checkbox("Preview camera data", &simulateCameraTracking);
    ImGui::Checkbox("Use OpenCV camera", &useRealCameraTracking);

    if (ImGui::Button("Refresh cameras")) {
        refreshCameraDevices();
    }

    const char* previewName = cameraDevices.empty() ? "Camera 0" : cameraDevices.front().name.c_str();
    for (const CameraDeviceInfo& device : cameraDevices) {
        if (device.index == cameraIndex) {
            previewName = device.name.c_str();
            break;
        }
    }

    if (ImGui::BeginCombo("Camera device", previewName)) {
        for (const CameraDeviceInfo& device : cameraDevices) {
            const bool selected = device.index == cameraIndex;
            if (ImGui::Selectable(device.name.c_str(), selected)) {
                cameraIndex = device.index;
                opencvTracker.stop();
            }
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::InputText("Face cascade", faceCascadePathBuffer.data(), faceCascadePathBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##FaceCascade")) {
        const std::string selected = openFileDialog("Select face cascade", "OpenCV XML (*.xml)\0*.xml\0All files (*.*)\0*.*\0");
        if (!selected.empty()) {
            copyStringToBuffer(selected, faceCascadePathBuffer.data(), faceCascadePathBuffer.size());
        }
    }
    ImGui::InputText("Landmark model", lbfModelPathBuffer.data(), lbfModelPathBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##LandmarkModel")) {
        const std::string selected = openFileDialog("Select landmark model", "YAML files (*.yaml;*.yml)\0*.yaml;*.yml\0All files (*.*)\0*.*\0");
        if (!selected.empty()) {
            copyStringToBuffer(selected, lbfModelPathBuffer.data(), lbfModelPathBuffer.size());
        }
    }
    if (ImGui::Button("Apply camera files")) {
        faceCascadePath = faceCascadePathBuffer.data();
        lbfModelPath = lbfModelPathBuffer.data();
        opencvTracker.stop();
    }

    bool calibrationChanged = false;
    calibrationChanged |= ImGui::SliderFloat("Head yaw offset", &cameraCalibration.headYawOffset, -45.0f, 45.0f, "%.1f");
    calibrationChanged |= ImGui::SliderFloat("Head pitch offset", &cameraCalibration.headPitchOffset, -45.0f, 45.0f, "%.1f");
    calibrationChanged |= ImGui::SliderFloat("Head roll offset", &cameraCalibration.headRollOffset, -45.0f, 45.0f, "%.1f");
    calibrationChanged |= ImGui::SliderFloat("Head yaw scale", &cameraCalibration.headYawScale, 0.1f, 3.0f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Head pitch scale", &cameraCalibration.headPitchScale, 0.1f, 3.0f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Head roll scale", &cameraCalibration.headRollScale, 0.1f, 3.0f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Blink threshold", &cameraCalibration.blinkThreshold, 0.05f, 0.45f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Mouth closed", &cameraCalibration.mouthClosed, 0.0f, 0.6f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Mouth range", &cameraCalibration.mouthOpenRange, 0.05f, 1.2f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Smile threshold", &cameraCalibration.smileThreshold, 1.0f, 3.5f, "%.2f");
    calibrationChanged |= ImGui::SliderFloat("Camera smoothing", &cameraCalibration.smoothing, 0.02f, 1.0f, "%.2f");
    if (calibrationChanged) {
        applyCameraCalibration();
    }

    ImGui::Text("Camera: %s", opencvTracker.getStatus().c_str());
    ImGui::Text("OpenCV: %s", opencvTracker.isAvailable() ? "available" : "not enabled");
    ImGui::Text("Audio: %s", audioTracking.getStatus().c_str());

    bool face = cameraTracking.get_face_tracking();
    bool head = cameraTracking.get_head_tracking();
    bool blink = cameraTracking.get_blink_tracking();
    bool hands = cameraTracking.get_hand_tracking();
    bool upperBody = cameraTracking.get_upperbody_tracking();

    if (ImGui::Checkbox("Face", &face)) cameraTracking.set_face_tracking(face);
    ImGui::SameLine();
    if (ImGui::Checkbox("Head", &head)) cameraTracking.set_head_tracking(head);
    if (ImGui::Checkbox("Blink", &blink)) cameraTracking.set_blink_tracking(blink);
    ImGui::SameLine();
    if (ImGui::Checkbox("Hands", &hands)) cameraTracking.set_hand_tracking(hands);
    ImGui::SameLine();
    if (ImGui::Checkbox("Upper body", &upperBody)) cameraTracking.set_upperbody_tracking(upperBody);

    const FaceTrackingState& state = cameraTracking.getState();
    ImGui::Text("Face: %s", state.foundPerson ? "found" : "not found");
    ImGui::Text("Head yaw %.1f  pitch %.1f  roll %.1f", state.headYaw, state.headPitch, state.headRoll);
    ImGui::Text("Mouth %.2f  Hands %d", state.mouthOpen, state.detectedHands);
}

void VEditor::showPNGTuberAssetPanel()
{
    if (activeRigType != AvatarRigType::PNGTuber) {
        return;
    }

    ImGui::Spacing();
    ImGui::Text("PNGTuber Assets");
    ImGui::Separator();

    ImGui::InputText("Base image", pngBaseBuffer.data(), pngBaseBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##BaseImage")) {
        const std::string selected = openFileDialog("Select base image", "Images (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, pngBaseBuffer.data(), pngBaseBuffer.size());
    }
    ImGui::InputText("Mouth open", pngMouthOpenBuffer.data(), pngMouthOpenBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##MouthOpen")) {
        const std::string selected = openFileDialog("Select mouth-open image", "Images (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, pngMouthOpenBuffer.data(), pngMouthOpenBuffer.size());
    }
    ImGui::InputText("Mouth closed", pngMouthClosedBuffer.data(), pngMouthClosedBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##MouthClosed")) {
        const std::string selected = openFileDialog("Select mouth-closed image", "Images (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, pngMouthClosedBuffer.data(), pngMouthClosedBuffer.size());
    }
    ImGui::InputText("Blink image", pngBlinkBuffer.data(), pngBlinkBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##BlinkImage")) {
        const std::string selected = openFileDialog("Select blink image", "Images (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, pngBlinkBuffer.data(), pngBlinkBuffer.size());
    }
    ImGui::InputText("Expression", pngExpressionBuffer.data(), pngExpressionBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##ExpressionImage")) {
        const std::string selected = openFileDialog("Select expression image", "Images (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, pngExpressionBuffer.data(), pngExpressionBuffer.size());
    }

    if (ImGui::Button("Apply PNG slots")) {
        pngSlots.baseImage = pngBaseBuffer.data();
        pngSlots.mouthOpenImage = pngMouthOpenBuffer.data();
        pngSlots.mouthClosedImage = pngMouthClosedBuffer.data();
        pngSlots.blinkImage = pngBlinkBuffer.data();
        pngSlots.expressionImage = pngExpressionBuffer.data();
        loadPNGTuberTextures();
    }

    ImGui::Text("Active sprite: %s", activePngSpriteName(pngSlots, avatarPose.png).c_str());

    ImGui::Spacing();
    ImGui::InputText("Project folder", projectFolderBuffer.data(), projectFolderBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##ProjectFolder")) {
        const std::string selected = chooseFolderDialog("Select VAvatar project folder");
        if (!selected.empty()) {
            copyStringToBuffer(selected, projectFolderBuffer.data(), projectFolderBuffer.size());
        }
    }
    ImGui::InputText("Project file", projectPathBuffer.data(), projectPathBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Choose##ProjectFile")) {
        const std::string selected = saveFileDialog("Save VAvatar project", "VAvatar projects (*.vavatar)\0*.vavatar\0All files (*.*)\0*.*\0", "vavatar");
        if (!selected.empty()) {
            copyStringToBuffer(selected, projectPathBuffer.data(), projectPathBuffer.size());
        }
    }
    if (ImGui::Button("Save .vavatar")) {
        pngSlots.baseImage = pngBaseBuffer.data();
        pngSlots.mouthOpenImage = pngMouthOpenBuffer.data();
        pngSlots.mouthClosedImage = pngMouthClosedBuffer.data();
        pngSlots.blinkImage = pngBlinkBuffer.data();
        pngSlots.expressionImage = pngExpressionBuffer.data();
        faceCascadePath = faceCascadePathBuffer.data();
        lbfModelPath = lbfModelPathBuffer.data();
        projectFolder = projectFolderBuffer.data();
        projectStatus = saveProject(projectPathBuffer.data()) ? "Project saved" : "Could not save project";
    }
    ImGui::SameLine();
    if (ImGui::Button("Load .vavatar")) {
        projectStatus = loadProject(projectPathBuffer.data()) ? "Project loaded" : "Could not load project";
    }
    ImGui::InputText("Calibration preset", calibrationPresetPathBuffer.data(), calibrationPresetPathBuffer.size());
    ImGui::InputText("Preset name", calibrationProfileNameBuffer.data(), calibrationProfileNameBuffer.size());
    if (ImGui::Button("Refresh presets")) {
        refreshCalibrationProfiles();
    }
    const char* selectedPreset = calibrationProfiles.empty() ? "No presets" : calibrationProfiles.front().c_str();
    if (ImGui::BeginCombo("Preset profiles", selectedPreset)) {
        for (const std::string& profile : calibrationProfiles) {
            if (ImGui::Selectable(profile.c_str())) {
                copyStringToBuffer(profile, calibrationProfileNameBuffer.data(), calibrationProfileNameBuffer.size());
                const std::filesystem::path presetPath = std::filesystem::path(projectFolder) / "Calibration" / (profile + ".vcal");
                copyStringToBuffer(presetPath.string(), calibrationPresetPathBuffer.data(), calibrationPresetPathBuffer.size());
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Choose##CalibrationPreset")) {
        const std::string selected = saveFileDialog("Save calibration preset", "Calibration presets (*.vcal)\0*.vcal\0All files (*.*)\0*.*\0", "vcal");
        if (!selected.empty()) {
            copyStringToBuffer(selected, calibrationPresetPathBuffer.data(), calibrationPresetPathBuffer.size());
        }
    }
    if (ImGui::Button("Save calibration")) {
        std::string presetPath = calibrationPresetPathBuffer.data();
        if (presetPath.empty() && calibrationProfileNameBuffer[0] != '\0') {
            presetPath = (std::filesystem::path(projectFolder) / "Calibration" / (std::string(calibrationProfileNameBuffer.data()) + ".vcal")).string();
        }
        projectStatus = saveCalibrationPreset(presetPath) ? "Calibration saved" : "Could not save calibration";
        refreshCalibrationProfiles();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load calibration")) {
        projectStatus = loadCalibrationPreset(calibrationPresetPathBuffer.data()) ? "Calibration loaded" : "Could not load calibration";
    }
    ImGui::Text("%s", projectStatus.c_str());
    if(!missingAssetIssues.empty()){ImGui::TextColored(ImVec4(1,.45f,.25f,1),"Missing assets: %d",static_cast<int>(missingAssetIssues.size()));for(size_t i=0;i<missingAssetIssues.size();++i){const auto issue=missingAssetIssues[i];ImGui::PushID(static_cast<int>(3000+i));ImGui::TextWrapped("%s: %s",issue.key.c_str(),issue.storedPath.c_str());ImGui::SameLine();if(ImGui::Button("Repair")){const std::string replacement=openFileDialog("Repair missing asset","All files (*.*)\0*.*\0");if(!replacement.empty()&&projectDocument.replaceAsset(issue.key,replacement,projectFolder)){projectDocument.save(projectPathBuffer.data());loadProject(projectPathBuffer.data());projectStatus="Asset repaired";}}ImGui::PopID();}}
}

void VEditor::showLive2DBindingPanel()
{
    if (activeRigType != AvatarRigType::Live2D) {
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Live2D / 2.5D Bindings");
    if(ImGui::Button("Add image layer")){const std::string selected=openFileDialog("Add Live2D layer","PNG images (*.png)\0*.png\0All files (*.*)\0*.*\0");if(!selected.empty()){Live2DLayer layer;layer.name=std::filesystem::path(selected).stem().string();layer.imagePath=selected;layer.depth=static_cast<float>(live2DRig.layers.size());live2DRig.layers.push_back(layer);rebuildAvatarRuntime();}}
    for (size_t i = 0; i < live2DRig.layers.size(); ++i) {
        Live2DLayer& layer = live2DRig.layers[i];
        ImGui::PushID(static_cast<int>(1000 + i));
        char name[128]{}, image[260]{}, visible[64]{}, warpX[64]{}, warpY[64]{};
        char deformer[64]{}, maskLayer[128]{}, physicsParameter[64]{};
        copyStringToBuffer(layer.name, name, sizeof(name));
        copyStringToBuffer(layer.imagePath, image, sizeof(image));
        copyStringToBuffer(layer.visibilityParameter, visible, sizeof(visible));
        copyStringToBuffer(layer.warpParameterX, warpX, sizeof(warpX));
        copyStringToBuffer(layer.warpParameterY, warpY, sizeof(warpY));
        copyStringToBuffer(layer.deformer, deformer, sizeof(deformer));
        copyStringToBuffer(layer.maskLayer, maskLayer, sizeof(maskLayer));
        copyStringToBuffer(layer.physicsParameter, physicsParameter, sizeof(physicsParameter));

        ImGui::InputText("Layer", name, sizeof(name));
        ImGui::InputText("Image", image, sizeof(image));
        ImGui::DragFloat("Depth", &layer.depth, 0.1f);
        ImGui::SliderFloat2("Pivot", &layer.pivotX, 0.0f, 1.0f);
        ImGui::DragFloat2("Parallax", &layer.parallaxX, 0.25f);
        ImGui::SliderFloat("Scale", &layer.scale, 0.05f, 4.0f);
        ImGui::SliderFloat("Rotation", &layer.rotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Opacity", &layer.opacity, 0.0f, 1.0f);
        ImGui::InputText("Visibility parameter", visible, sizeof(visible));
        ImGui::DragFloatRange2("Visible range", &layer.visibilityMin, &layer.visibilityMax, 0.05f, -90.0f, 90.0f);
        ImGui::Checkbox("Visible", &layer.visible);

        if (ImGui::TreeNode("Mesh deformation")) {
            ImGui::SliderInt("Mesh columns", &layer.meshColumns, 1, 32);
            ImGui::SliderInt("Mesh rows", &layer.meshRows, 1, 32);
            ImGui::InputText("Warp X parameter", warpX, sizeof(warpX));
            ImGui::InputText("Warp Y parameter", warpY, sizeof(warpY));
            ImGui::DragFloat("Warp X strength", &layer.warpStrengthX, 0.25f, -500.0f, 500.0f);
            ImGui::DragFloat("Warp Y strength", &layer.warpStrengthY, 0.25f, -500.0f, 500.0f);
            ImGui::InputText("Parent deformer", deformer, sizeof(deformer));
            ImGui::InputText("Clip mask layer", maskLayer, sizeof(maskLayer));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Spring physics")) {
            ImGui::Checkbox("Physics enabled", &layer.physicsEnabled);
            ImGui::InputText("Physics parameter", physicsParameter, sizeof(physicsParameter));
            ImGui::DragFloat2("Physics strength", &layer.physicsStrengthX, 0.25f, -500.0f, 500.0f);
            ImGui::DragFloat("Stiffness", &layer.physicsStiffness, 0.25f, 0.0f, 250.0f);
            ImGui::DragFloat("Damping", &layer.physicsDamping, 0.1f, 0.0f, 50.0f);
            ImGui::DragFloat("Gravity", &layer.physicsGravity, 0.1f, -100.0f, 100.0f);
            ImGui::TreePop();
        }

        layer.name = name;
        layer.imagePath = image;
        layer.visibilityParameter = visible;
        layer.warpParameterX = warpX;
        layer.warpParameterY = warpY;
        layer.deformer = deformer;
        layer.maskLayer = maskLayer;
        layer.physicsParameter = physicsParameter;

        if (i > 0 && ImGui::Button("Up")) {
            std::swap(live2DRig.layers[i], live2DRig.layers[i - 1]);
            rebuildAvatarRuntime();
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();
        if (i + 1 < live2DRig.layers.size() && ImGui::Button("Down")) {
            std::swap(live2DRig.layers[i], live2DRig.layers[i + 1]);
            rebuildAvatarRuntime();
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            live2DRig.layers.erase(live2DRig.layers.begin() + static_cast<std::ptrdiff_t>(i));
            rebuildAvatarRuntime();
            ImGui::PopID();
            break;
        }
        ImGui::Separator();
        ImGui::PopID();
    }
    if(ImGui::Button("Reload layer preview")) rebuildAvatarRuntime();
    if (ImGui::Button("Add deformer")) {
        Live2DDeformer deformer;
        deformer.name = "Deformer " + std::to_string(live2DRig.deformers.size() + 1);
        live2DRig.deformers.push_back(deformer);
    }
    for (size_t i = 0; i < live2DRig.deformers.size(); ++i) {
        Live2DDeformer& value = live2DRig.deformers[i];
        ImGui::PushID(static_cast<int>(2000 + i));
        char name[64]{}, parent[64]{}, txParameter[64]{}, tyParameter[64]{}, rotationParameter[64]{};
        copyStringToBuffer(value.name, name, sizeof(name));
        copyStringToBuffer(value.parent, parent, sizeof(parent));
        copyStringToBuffer(value.translateXParameter, txParameter, sizeof(txParameter));
        copyStringToBuffer(value.translateYParameter, tyParameter, sizeof(tyParameter));
        copyStringToBuffer(value.rotationParameter, rotationParameter, sizeof(rotationParameter));
        if (ImGui::TreeNode(name[0] ? name : "Unnamed deformer")) {
            ImGui::InputText("Name", name, sizeof(name));
            ImGui::InputText("Parent", parent, sizeof(parent));
            ImGui::SliderFloat2("Deformer pivot", &value.pivotX, 0.0f, 1.0f);
            ImGui::DragFloat2("Offset", &value.offsetX, 0.25f);
            ImGui::DragFloat("Deformer rotation", &value.rotation, 0.1f, -180.0f, 180.0f);
            ImGui::DragFloat2("Deformer scale", &value.scaleX, 0.01f, 0.01f, 10.0f);
            ImGui::InputText("Translate X parameter", txParameter, sizeof(txParameter));
            ImGui::InputText("Translate Y parameter", tyParameter, sizeof(tyParameter));
            ImGui::InputText("Rotation parameter", rotationParameter, sizeof(rotationParameter));
            ImGui::DragFloat("Translate X amount", &value.translateXAmount, 0.25f);
            ImGui::DragFloat("Translate Y amount", &value.translateYAmount, 0.25f);
            ImGui::DragFloat("Rotation amount", &value.rotationAmount, 0.1f);
            ImGui::TreePop();
        }
        value.name = name;
        value.parent = parent;
        value.translateXParameter = txParameter;
        value.translateYParameter = tyParameter;
        value.rotationParameter = rotationParameter;
        if (ImGui::Button("Remove deformer")) {
            live2DRig.deformers.erase(live2DRig.deformers.begin() + static_cast<std::ptrdiff_t>(i));
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
    }
    ImGui::Separator();

    if (ImGui::Button("Face template")) {
        live2DRig.bindings = std::array<Live2DBinding, 6>{
            Live2DBinding{ "ParamAngleX", "headYaw", "Linear", -30.0f, 30.0f, 0.35f, false },
            Live2DBinding{ "ParamAngleY", "headPitch", "Ease", -30.0f, 30.0f, 0.35f, true },
            Live2DBinding{ "ParamAngleZ", "headRoll", "Linear", -30.0f, 30.0f, 0.35f, false },
            Live2DBinding{ "ParamMouthOpenY", "mouthOpen", "Responsive", 0.0f, 1.0f, 0.2f, false },
            Live2DBinding{ "ParamEyeLOpen", "leftEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false },
            Live2DBinding{ "ParamEyeROpen", "rightEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false }
        };
    }
    ImGui::SameLine();
    if (ImGui::Button("2.5D parallax template")) {
        live2DRig.bindings = std::array<Live2DBinding, 6>{
            Live2DBinding{ "LayerFaceX", "headYaw", "Ease", -18.0f, 18.0f, 0.45f, false },
            Live2DBinding{ "LayerHairX", "headYaw", "Ease", -26.0f, 26.0f, 0.55f, false },
            Live2DBinding{ "LayerBodyX", "headYaw", "Linear", -9.0f, 9.0f, 0.5f, false },
            Live2DBinding{ "ParamMouthOpenY", "mouthOpen", "Responsive", 0.0f, 1.0f, 0.2f, false },
            Live2DBinding{ "ParamEyeLOpen", "leftEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false },
            Live2DBinding{ "ParamEyeROpen", "rightEyeOpen", "Linear", 0.0f, 1.0f, 0.15f, false }
        };
    }

    const char* discoveredParameters[] = {
        "ParamAngleX", "ParamAngleY", "ParamAngleZ", "ParamBodyAngleX",
        "ParamMouthOpenY", "ParamMouthForm", "ParamEyeLOpen", "ParamEyeROpen",
        "ParamEyeBallX", "ParamEyeBallY", "ParamBrowLY", "ParamBrowRY", "ParamBodyAngleY",
        "LayerFaceX", "LayerHairX", "LayerBodyX"
    };
    const char* trackingSources[] = {
        "headYaw", "headPitch", "headRoll", "mouthOpen", "smile",
        "leftEyeOpen", "rightEyeOpen", "voiceMouthOpen", "handPresence",
        "eyeGazeX", "eyeGazeY", "browLeft", "browRight", "bodyAngleX", "bodyAngleY"
    };
    const char* curveNames[] = { "Linear", "Ease", "Responsive", "Snap" };

    for (size_t i = 0; i < live2DRig.bindings.size(); ++i) {
        Live2DBinding& binding = live2DRig.bindings[i];
        copyStringToBuffer(binding.parameter, live2DParameterBuffers[i].data(), live2DParameterBuffers[i].size());
        copyStringToBuffer(binding.source, live2DSourceBuffers[i].data(), live2DSourceBuffers[i].size());
        copyStringToBuffer(binding.curve, live2DCurveBuffers[i].data(), live2DCurveBuffers[i].size());

        ImGui::PushID(static_cast<int>(i));
        ImGui::InputText("Parameter", live2DParameterBuffers[i].data(), live2DParameterBuffers[i].size());
        ImGui::SameLine();
        if (ImGui::BeginCombo("Known params", binding.parameter.c_str())) {
            for (const char* parameter : discoveredParameters) {
                if (ImGui::Selectable(parameter)) {
                    copyStringToBuffer(parameter, live2DParameterBuffers[i].data(), live2DParameterBuffers[i].size());
                }
            }
            ImGui::EndCombo();
        }
        ImGui::InputText("Source", live2DSourceBuffers[i].data(), live2DSourceBuffers[i].size());
        ImGui::SameLine();
        if (ImGui::BeginCombo("Tracking source", binding.source.c_str())) {
            for (const char* source : trackingSources) {
                if (ImGui::Selectable(source)) {
                    copyStringToBuffer(source, live2DSourceBuffers[i].data(), live2DSourceBuffers[i].size());
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Curve", binding.curve.c_str())) {
            for (const char* curve : curveNames) {
                if (ImGui::Selectable(curve)) {
                    copyStringToBuffer(curve, live2DCurveBuffers[i].data(), live2DCurveBuffers[i].size());
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SliderFloat("Min", &binding.minValue, -90.0f, 90.0f, "%.2f");
        ImGui::SliderFloat("Max", &binding.maxValue, -90.0f, 90.0f, "%.2f");
        ImGui::SliderFloat("Smoothing", &binding.smoothing, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Invert", &binding.inverted);
        binding.parameter = live2DParameterBuffers[i].data();
        binding.source = live2DSourceBuffers[i].data();
        binding.curve = live2DCurveBuffers[i].data();
        ImGui::Separator();
        ImGui::PopID();
    }
}

void VEditor::showModel3DRigPanel()
{
    if (activeRigType != AvatarRigType::Model3D) return;

    ImGui::Spacing();
    ImGui::Text("3D Model Rig");
    ImGui::InputText("Search bones/morphs",rigSearchBuffer.data(),rigSearchBuffer.size());
    const std::string search=rigSearchBuffer.data();
    if(auto* runtime=dynamic_cast<Model3DRuntime*>(avatarRuntime.get())){auto bones=runtime->boneNames();if(ImGui::BeginCombo("Discovered head bone",model3DRig.headBone.c_str())){for(const auto& n:bones)if(search.empty()||n.find(search)!=std::string::npos)if(ImGui::Selectable(n.c_str()))copyStringToBuffer(n,headBoneBuffer.data(),headBoneBuffer.size());ImGui::EndCombo();}auto morphs=runtime->morphTargetNames();if(ImGui::BeginCombo("Discovered morph",model3DRig.jawBlendshape.c_str())){for(const auto& n:morphs)if(search.empty()||n.find(search)!=std::string::npos)if(ImGui::Selectable(n.c_str()))copyStringToBuffer(n,jawBlendshapeBuffer.data(),jawBlendshapeBuffer.size());ImGui::EndCombo();}for(const auto& issue:runtime->validateBindings())ImGui::TextColored(ImVec4(1,.45f,.3f,1),"%s",issue.c_str());}
    if(ImGui::Button("Apply VRM humanoid names")){copyStringToBuffer("head",headBoneBuffer.data(),headBoneBuffer.size());copyStringToBuffer("neck",neckBoneBuffer.data(),neckBoneBuffer.size());copyStringToBuffer("chest",chestBoneBuffer.data(),chestBoneBuffer.size());copyStringToBuffer("Fcl_MTH_A",jawBlendshapeBuffer.data(),jawBlendshapeBuffer.size());copyStringToBuffer("Fcl_EYE_Close_L",blinkLeftBlendshapeBuffer.data(),blinkLeftBlendshapeBuffer.size());copyStringToBuffer("Fcl_EYE_Close_R",blinkRightBlendshapeBuffer.data(),blinkRightBlendshapeBuffer.size());copyStringToBuffer("Fcl_MTH_Fun",smileBlendshapeBuffer.data(),smileBlendshapeBuffer.size());}
    ImGui::Separator();
    ImGui::InputText("Model", modelPathBuffer.data(), modelPathBuffer.size());
    ImGui::SameLine();
    if (ImGui::Button("Browse##Model3D")) {
        const std::string selected = openFileDialog("Select 3D avatar", "3D models (*.obj;*.fbx;*.glb;*.gltf)\0*.obj;*.fbx;*.glb;*.gltf\0All files (*.*)\0*.*\0");
        if (!selected.empty()) copyStringToBuffer(selected, modelPathBuffer.data(), modelPathBuffer.size());
    }
    ImGui::InputText("Head bone", headBoneBuffer.data(), headBoneBuffer.size());
    ImGui::InputText("Neck bone", neckBoneBuffer.data(), neckBoneBuffer.size());
    ImGui::InputText("Chest bone", chestBoneBuffer.data(), chestBoneBuffer.size());
    ImGui::InputText("Jaw blendshape", jawBlendshapeBuffer.data(), jawBlendshapeBuffer.size());
    ImGui::InputText("Left blink", blinkLeftBlendshapeBuffer.data(), blinkLeftBlendshapeBuffer.size());
    ImGui::InputText("Right blink", blinkRightBlendshapeBuffer.data(), blinkRightBlendshapeBuffer.size());
    ImGui::InputText("Smile", smileBlendshapeBuffer.data(), smileBlendshapeBuffer.size());
    ImGui::SliderFloat("Head follow", &model3DRig.headFollow, 0.0f, 1.5f);
    ImGui::SliderFloat("Neck follow", &model3DRig.neckFollow, 0.0f, 1.0f);
    ImGui::SliderFloat("Chest follow", &model3DRig.chestFollow, 0.0f, 1.0f);
    model3DRig.modelPath = modelPathBuffer.data();
    model3DRig.headBone = headBoneBuffer.data();
    model3DRig.neckBone = neckBoneBuffer.data();
    model3DRig.chestBone = chestBoneBuffer.data();
    model3DRig.jawBlendshape = jawBlendshapeBuffer.data();
    model3DRig.blinkLeftBlendshape = blinkLeftBlendshapeBuffer.data();
    model3DRig.blinkRightBlendshape = blinkRightBlendshapeBuffer.data();
    model3DRig.smileBlendshape = smileBlendshapeBuffer.data();
    ImGui::TextWrapped("OBJ geometry is available through Moss now. FBX/GLB skeletons and morph targets activate when Moss exposes those loader results.");
}

void VEditor::showRigOutputPanel()
{
    ImGui::Spacing();
    ImGui::Text("Avatar Output");
    ImGui::Separator();

    if (activeRigType == AvatarRigType::PNGTuber) {
        ImGui::Text("Sprite mouth: %s", avatarPose.png.useOpenMouthSprite ? "open" : "closed");
        ImGui::Text("Sprite blink: %s", avatarPose.png.useBlinkSprite ? "blink" : "open");
        ImGui::Text("Active PNG: %s", activePngSpriteName(pngSlots, avatarPose.png).c_str());
        ImGui::Text("Offset %.1f %.1f", avatarPose.png.offsetX, avatarPose.png.offsetY);
        ImGui::Text("Bounce %.1f  Squish %.2f", avatarPose.png.bounce, avatarPose.png.squish);
    } else if (activeRigType == AvatarRigType::Live2D) {
        ImGui::Text("ParamAngleX %.1f", avatarPose.live2D.paramAngleX);
        ImGui::Text("ParamAngleY %.1f", avatarPose.live2D.paramAngleY);
        ImGui::Text("ParamAngleZ %.1f", avatarPose.live2D.paramAngleZ);
        ImGui::Text("ParamMouthOpenY %.2f", avatarPose.live2D.paramMouthOpenY);
        ImGui::Text("ParamEye L %.2f R %.2f", avatarPose.live2D.paramEyeLOpen, avatarPose.live2D.paramEyeROpen);
    } else {
        ImGui::Text("Head yaw %.1f pitch %.1f roll %.1f", avatarPose.model3D.headYaw, avatarPose.model3D.headPitch, avatarPose.model3D.headRoll);
        ImGui::Text("Blend jawOpen %.2f", avatarPose.model3D.jawOpen);
        ImGui::Text("Blend blink L %.2f R %.2f", avatarPose.model3D.blinkLeft, avatarPose.model3D.blinkRight);
        ImGui::Text("Blend smile %.2f hands %.2f", avatarPose.model3D.smile, avatarPose.model3D.handPresence);
    }
}

void VEditor::showConsole()
{
    ImGui::Spacing();
    ImGui::Text("Console");
    ImGui::Separator();
    ImGui::TextWrapped("Moss runtime active. Avatar rendering modules can now be moved across incrementally.");
}

void VEditor::refreshCameraDevices()
{
    cameraDevices = CameraTrackingOpenCV::listAvailableDevices();
    if (!cameraDevices.empty() && std::none_of(cameraDevices.begin(), cameraDevices.end(), [this](const CameraDeviceInfo& device) {
        return device.index == cameraIndex;
    })) {
        cameraIndex = cameraDevices.front().index;
    }
}

void VEditor::applyCameraCalibration()
{
    opencvTracker.setCalibration(cameraCalibration);
    cameraCalibration = opencvTracker.getCalibration();
}

void VEditor::refreshCalibrationProfiles()
{
    calibrationProfiles.clear();

    try {
        const std::filesystem::path folder = std::filesystem::path(projectFolder) / "Calibration";
        if (!std::filesystem::exists(folder)) {
            return;
        }

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(folder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vcal") {
                calibrationProfiles.push_back(entry.path().stem().string());
            }
        }
        std::sort(calibrationProfiles.begin(), calibrationProfiles.end());
    } catch (const std::exception&) {
        calibrationProfiles.clear();
    }
}

void VEditor::loadPNGTuberTextures()
{
    const std::array<std::string, 5> paths = {
        pngSlots.baseImage,
        pngSlots.mouthOpenImage,
        pngSlots.mouthClosedImage,
        pngSlots.blinkImage,
        pngSlots.expressionImage
    };

    for (size_t i = 0; i < paths.size(); ++i) {
        const std::string resolved = resolveProjectPath(projectFolder, paths[i]);
        if (resolved.empty()) {
            continue;
        }

        if (pngTextures[i].path != resolved) {
            MossTextureLoader::loadFromFile(resolved, pngTextures[i]);
        }
    }
}

void VEditor::destroyPNGTuberTextures()
{
    for (MossTexture& texture : pngTextures) {
        MossTextureLoader::destroy(texture);
    }
}

MossTexture* VEditor::activePNGTuberTexture()
{
    const int index = activePngTextureIndex(pngSlots, avatarPose.png);
    if (index < 0 || index >= static_cast<int>(pngTextures.size())) {
        return nullptr;
    }

    MossTexture& texture = pngTextures[static_cast<size_t>(index)];
    if (texture.id == 0) {
        loadPNGTuberTextures();
    }

    return texture.id == 0 ? nullptr : &texture;
}

void VEditor::copyProjectStringsToBuffers()
{
    copyStringToBuffer(faceCascadePath, faceCascadePathBuffer.data(), faceCascadePathBuffer.size());
    copyStringToBuffer(lbfModelPath, lbfModelPathBuffer.data(), lbfModelPathBuffer.size());
    copyStringToBuffer(projectFolder, projectFolderBuffer.data(), projectFolderBuffer.size());
    if (calibrationPresetPathBuffer[0] == '\0') {
        copyStringToBuffer((std::filesystem::path(projectFolder) / "Calibration" / "Default.vcal").string(), calibrationPresetPathBuffer.data(), calibrationPresetPathBuffer.size());
    }
    if (calibrationProfileNameBuffer[0] == '\0') {
        copyStringToBuffer("Default", calibrationProfileNameBuffer.data(), calibrationProfileNameBuffer.size());
    }
    if (projectPathBuffer[0] == '\0') {
        copyStringToBuffer("Projects/UntitledAvatar/project.vavatar", projectPathBuffer.data(), projectPathBuffer.size());
    }
    copyStringToBuffer(pngSlots.baseImage, pngBaseBuffer.data(), pngBaseBuffer.size());
    copyStringToBuffer(pngSlots.mouthOpenImage, pngMouthOpenBuffer.data(), pngMouthOpenBuffer.size());
    copyStringToBuffer(pngSlots.mouthClosedImage, pngMouthClosedBuffer.data(), pngMouthClosedBuffer.size());
    copyStringToBuffer(pngSlots.blinkImage, pngBlinkBuffer.data(), pngBlinkBuffer.size());
    copyStringToBuffer(pngSlots.expressionImage, pngExpressionBuffer.data(), pngExpressionBuffer.size());
    copyStringToBuffer(model3DRig.modelPath, modelPathBuffer.data(), modelPathBuffer.size());
    copyStringToBuffer(model3DRig.headBone, headBoneBuffer.data(), headBoneBuffer.size());
    copyStringToBuffer(model3DRig.neckBone, neckBoneBuffer.data(), neckBoneBuffer.size());
    copyStringToBuffer(model3DRig.chestBone, chestBoneBuffer.data(), chestBoneBuffer.size());
    copyStringToBuffer(model3DRig.jawBlendshape, jawBlendshapeBuffer.data(), jawBlendshapeBuffer.size());
    copyStringToBuffer(model3DRig.blinkLeftBlendshape, blinkLeftBlendshapeBuffer.data(), blinkLeftBlendshapeBuffer.size());
    copyStringToBuffer(model3DRig.blinkRightBlendshape, blinkRightBlendshapeBuffer.data(), blinkRightBlendshapeBuffer.size());
    copyStringToBuffer(model3DRig.smileBlendshape, smileBlendshapeBuffer.data(), smileBlendshapeBuffer.size());
}

bool VEditor::saveProject(const std::string& path) const
{
    if (path.empty()) return false;

    ProjectDocument document;
    document.set("project.folder", projectFolder);
    document.set("avatarType", std::to_string(static_cast<int>(activeRigType)));
    document.set("png.base", makeRelativePath(projectFolder, pngSlots.baseImage));
    document.set("png.mouthOpen", makeRelativePath(projectFolder, pngSlots.mouthOpenImage));
    document.set("png.mouthClosed", makeRelativePath(projectFolder, pngSlots.mouthClosedImage));
    document.set("png.blink", makeRelativePath(projectFolder, pngSlots.blinkImage));
    document.set("png.expression", makeRelativePath(projectFolder, pngSlots.expressionImage));
    PNGTuberEffectProject::save(document, pngEffects);

    document.set("voice.threshold", std::to_string(voiceThreshold));
    document.set("voice.mouthResponse", std::to_string(voiceMouthResponse));
    document.set("tracking.voiceEnabled", voiceTrackingEnabled ? "1" : "0");
    document.set("tracking.cameraEnabled", cameraTrackingEnabled ? "1" : "0");
    document.set("tracking.simulationEnabled", simulateCameraTracking ? "1" : "0");
    document.set("tracking.openCVEnabled", useRealCameraTracking ? "1" : "0");
    document.set("tracking.faceEnabled", cameraTracking.get_face_tracking() ? "1" : "0");
    document.set("tracking.headEnabled", cameraTracking.get_head_tracking() ? "1" : "0");
    document.set("tracking.blinkEnabled", cameraTracking.get_blink_tracking() ? "1" : "0");
    document.set("tracking.handsEnabled", cameraTracking.get_hand_tracking() ? "1" : "0");
    document.set("tracking.upperBodyEnabled", cameraTracking.get_upperbody_tracking() ? "1" : "0");

    document.set("camera.index", std::to_string(cameraIndex));
    document.set("camera.faceCascade", makeRelativePath(projectFolder, faceCascadePath));
    document.set("camera.landmarkModel", makeRelativePath(projectFolder, lbfModelPath));
    AvatarProjectSettings::saveCameraCalibration(document, cameraCalibration);

    const auto encodePath = [this](const std::string& value) {
        return makeRelativePath(projectFolder, value);
    };
    AvatarProjectSettings::saveModel3D(document, model3DRig, encodePath);
    AvatarProjectSettings::saveLive2D(document, live2DRig, encodePath);
    return document.save(path);
}

bool VEditor::loadProject(const std::string& path)
{
    ProjectDocument document;
    if (!document.load(path)) return false;

    projectDocument = document;
    const std::string base = std::filesystem::path(path).parent_path().string();
    projectFolder = document.get("project.folder", base);
    activeRigType = static_cast<AvatarRigType>(
        std::clamp(std::atoi(document.get("avatarType", "0").c_str()), 0, 2));

    const auto decodePath = [this](const std::string& value) {
        return resolveProjectPath(projectFolder, value);
    };
    pngSlots.baseImage = decodePath(document.get("png.base"));
    pngSlots.mouthOpenImage = decodePath(document.get("png.mouthOpen"));
    pngSlots.mouthClosedImage = decodePath(document.get("png.mouthClosed"));
    pngSlots.blinkImage = decodePath(document.get("png.blink"));
    pngSlots.expressionImage = decodePath(document.get("png.expression"));
    PNGTuberEffectProject::load(document, pngEffects);

    voiceThreshold = AvatarProjectSettings::number(document, "voice.threshold", voiceThreshold);
    voiceMouthResponse = AvatarProjectSettings::number(document, "voice.mouthResponse", voiceMouthResponse);
    voiceTrackingEnabled = AvatarProjectSettings::boolean(document, "tracking.voiceEnabled", voiceTrackingEnabled);
    cameraTrackingEnabled = AvatarProjectSettings::boolean(document, "tracking.cameraEnabled", cameraTrackingEnabled);
    simulateCameraTracking = AvatarProjectSettings::boolean(document, "tracking.simulationEnabled", simulateCameraTracking);
    useRealCameraTracking = AvatarProjectSettings::boolean(document, "tracking.openCVEnabled", useRealCameraTracking);
    cameraTracking.set_face_tracking(AvatarProjectSettings::boolean(document, "tracking.faceEnabled", cameraTracking.get_face_tracking()));
    cameraTracking.set_head_tracking(AvatarProjectSettings::boolean(document, "tracking.headEnabled", cameraTracking.get_head_tracking()));
    cameraTracking.set_blink_tracking(AvatarProjectSettings::boolean(document, "tracking.blinkEnabled", cameraTracking.get_blink_tracking()));
    cameraTracking.set_hand_tracking(AvatarProjectSettings::boolean(document, "tracking.handsEnabled", cameraTracking.get_hand_tracking()));
    cameraTracking.set_upperbody_tracking(AvatarProjectSettings::boolean(document, "tracking.upperBodyEnabled", cameraTracking.get_upperbody_tracking()));

    cameraIndex = std::max(0, AvatarProjectSettings::integer(document, "camera.index", cameraIndex));
    faceCascadePath = decodePath(document.get("camera.faceCascade"));
    lbfModelPath = decodePath(document.get("camera.landmarkModel"));
    AvatarProjectSettings::loadCameraCalibration(document, cameraCalibration);
    AvatarProjectSettings::loadModel3D(document, model3DRig, decodePath);
    AvatarProjectSettings::loadLive2D(document, live2DRig, decodePath);

    missingAssetIssues = document.missingAssets(projectFolder);
    copyProjectStringsToBuffers();
    applyCameraCalibration();
    rebuildAvatarRuntime();
    return true;
}

bool VEditor::saveCalibrationPreset(const std::string& path) const
{
    if (path.empty()) {
        return false;
    }

    const std::filesystem::path presetPath(path);
    if (!presetPath.parent_path().empty()) {
        std::filesystem::create_directories(presetPath.parent_path());
    }

    std::ofstream file(presetPath);
    if (!file) {
        return false;
    }

    file << "version=1\n";
    file << "voice.threshold=" << voiceThreshold << "\n";
    file << "voice.mouthResponse=" << voiceMouthResponse << "\n";
    file << "camera.headYawOffset=" << cameraCalibration.headYawOffset << "\n";
    file << "camera.headPitchOffset=" << cameraCalibration.headPitchOffset << "\n";
    file << "camera.headRollOffset=" << cameraCalibration.headRollOffset << "\n";
    file << "camera.headYawScale=" << cameraCalibration.headYawScale << std::endl;
    file << "camera.headPitchScale=" << cameraCalibration.headPitchScale << std::endl;
    file << "camera.headRollScale=" << cameraCalibration.headRollScale << std::endl;
    file << "camera.blinkThreshold=" << cameraCalibration.blinkThreshold << "\n";
    file << "camera.mouthClosed=" << cameraCalibration.mouthClosed << "\n";
    file << "camera.mouthOpenRange=" << cameraCalibration.mouthOpenRange << "\n";
    file << "camera.smileThreshold=" << cameraCalibration.smileThreshold << "\n";
    file << "camera.smoothing=" << cameraCalibration.smoothing << "\n";
    return true;
}

bool VEditor::loadCalibrationPreset(const std::string& path)
{
    if (path.empty()) {
        return false;
    }

    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        const std::string value = valueAfterEquals(line);
        if (lineStartsWith(line, "voice.threshold=")) voiceThreshold = readFloatValue(value, voiceThreshold);
        else if (lineStartsWith(line, "voice.mouthResponse=")) voiceMouthResponse = readFloatValue(value, voiceMouthResponse);
        else if (lineStartsWith(line, "camera.headYawOffset=")) cameraCalibration.headYawOffset = readFloatValue(value, cameraCalibration.headYawOffset);
        else if (lineStartsWith(line, "camera.headPitchOffset=")) cameraCalibration.headPitchOffset = readFloatValue(value, cameraCalibration.headPitchOffset);
        else if (lineStartsWith(line, "camera.headRollOffset=")) cameraCalibration.headRollOffset = readFloatValue(value, cameraCalibration.headRollOffset);
        else if (lineStartsWith(line, "camera.headYawScale=")) cameraCalibration.headYawScale = readFloatValue(value, cameraCalibration.headYawScale);
        else if (lineStartsWith(line, "camera.headPitchScale=")) cameraCalibration.headPitchScale = readFloatValue(value, cameraCalibration.headPitchScale);
        else if (lineStartsWith(line, "camera.headRollScale=")) cameraCalibration.headRollScale = readFloatValue(value, cameraCalibration.headRollScale);
        else if (lineStartsWith(line, "camera.blinkThreshold=")) cameraCalibration.blinkThreshold = readFloatValue(value, cameraCalibration.blinkThreshold);
        else if (lineStartsWith(line, "camera.mouthClosed=")) cameraCalibration.mouthClosed = readFloatValue(value, cameraCalibration.mouthClosed);
        else if (lineStartsWith(line, "camera.mouthOpenRange=")) cameraCalibration.mouthOpenRange = readFloatValue(value, cameraCalibration.mouthOpenRange);
        else if (lineStartsWith(line, "camera.smileThreshold=")) cameraCalibration.smileThreshold = readFloatValue(value, cameraCalibration.smileThreshold);
        else if (lineStartsWith(line, "camera.smoothing=")) cameraCalibration.smoothing = readFloatValue(value, cameraCalibration.smoothing);
    }

    applyCameraCalibration();
    return true;
}
