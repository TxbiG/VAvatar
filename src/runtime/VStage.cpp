#include "VStage.h"
#include "../core/avatar/PNGTuberEffectsProject.h"
#include "../core/avatar/AvatarProjectSettings.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <thread>

namespace {
std::string value(const std::string& s){auto p=s.find('=');return p==std::string::npos?std::string{}:s.substr(p+1);}
bool key(const std::string& s,const std::string& k){return s.rfind(k,0)==0;}
std::string asset(const std::string& f,const std::string& s){if(s.empty())return {};std::filesystem::path p(s);return(p.is_absolute()||f.empty())?p.string():(std::filesystem::path(f)/p).string();}
float number(const std::string&s,float d){try{return std::stof(s);}catch(...){return d;}}
}

StageResult VStage::init(){
 if(!app.init("VAvatar Stage",960,540))return StageResult::Exit;live2DRig.resetBindings();loadProject(projectPath);rebuildAvatarRuntime();
 while(app.beginFrame(0,0,0,0)){auto start=std::chrono::steady_clock::now();updateTrackingPreview();updateAvatarPose();renderStage();if(showControls)renderControlOverlay();app.endFrame();float e=std::chrono::duration<float>(std::chrono::steady_clock::now()-start).count();runtimeFps=e>0?1/e:0;float target=targetFps>0?1.0f/targetFps:0;if(target>e)std::this_thread::sleep_for(std::chrono::duration<float>(target-e));}
 audioTracking.stop();opencvTracker.stop();avatarRuntime.reset();return result;
}
void VStage::updateTrackingPreview(){
 if(voiceTrackingEnabled&&!audioTracking.isRunning())audioTracking.start();else if(!voiceTrackingEnabled&&audioTracking.isRunning())audioTracking.stop();audioTracking.setThreshold(voiceThreshold);
 if(cameraTrackingEnabled&&useRealCameraTracking){if(!opencvTracker.isRunning())opencvTracker.start(cameraIndex,faceCascadePath,lbfModelPath);auto f=opencvTracker.getState();if(f.foundPerson){cameraTracking.updateFromFace(f.headYaw,f.headPitch,f.headRoll,std::max(audioTracking.getMouthOpen(),f.mouthOpen),f.leftBlink,f.rightBlink,f.smiling);cameraTracking.updateContinuousFace(f.leftEyeOpen,f.rightEyeOpen,f.smile,f.mouthForm,f.eyeGazeX,f.eyeGazeY,f.browLeft,f.browRight);cameraTracking.updateBody(f.shoulderYaw,f.shoulderPitch);cameraTracking.updateHands(f.detectedHands);cameraTracking.updateHandLandmarks(f.handLandmarks.data(),f.handLandmarkCount);}else cameraTracking.clearFrame();}
 else if(cameraTrackingEnabled&&simulateCameraTracking){if(opencvTracker.isRunning())opencvTracker.stop();float t=float(ImGui::GetTime()),eye=std::fmod(t,4)<.15f?0.0f:1.0f;cameraTracking.updateFromFace(std::sin(t*.8f)*18,std::sin(t*.5f)*8,std::sin(t*.35f)*5,std::max(audioTracking.getMouthOpen(),(std::sin(t*3)+1)*.08f),eye==0,eye==0,std::sin(t*.6f)>.55f);cameraTracking.updateContinuousFace(eye,eye,(std::sin(t*.6f)+1)*.5f,0,std::sin(t*.4f)*.4f,std::sin(t*.3f)*.3f,0,0);}
 else{if(opencvTracker.isRunning())opencvTracker.stop();cameraTracking.clearFrame();}
}
void VStage::updateAvatarPose(){TrackingInput in{cameraTracking.getState(),voiceTrackingEnabled?std::clamp(audioTracking.getMouthOpen()*voiceMouthResponse,0.0f,1.0f):0};avatarPose=trackingMapper.map(activeRigType,in);avatarPose.png.useExpressionSprite=pngEffects.expressionEnabled&&avatarPose.png.expressionAmount>=pngEffects.expressionThreshold;if(avatarRuntime){avatarRuntime->applyTrackingPose(avatarPose);avatarRuntime->update(std::max(ImGui::GetIO().DeltaTime,1.0f/240));}}
void VStage::renderStage(){auto vp=ImGui::GetMainViewport();ImGui::SetNextWindowPos(vp->WorkPos);ImGui::SetNextWindowSize(vp->WorkSize);ImGui::Begin("VStageOutput",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBackground);ImVec2 mn=ImGui::GetWindowPos(),s=ImGui::GetWindowSize(),mx(mn.x+s.x,mn.y+s.y);if(chromaKeyEnabled)ImGui::GetWindowDrawList()->AddRectFilled(mn,mx,ImGui::ColorConvertFloat4ToU32(ImVec4(chromaKeyColor[0],chromaKeyColor[1],chromaKeyColor[2],chromaKeyColor[3])));if(avatarRuntime)avatarRuntime->render({ImGui::GetWindowDrawList(),mn,mx});ImGui::End();}
void VStage::renderControlOverlay(){ImGui::SetNextWindowPos(ImVec2(12,12),ImGuiCond_Always);ImGui::Begin("Stage Controls",nullptr,ImGuiWindowFlags_AlwaysAutoResize);if(ImGui::Button("Back to Editor")){result=StageResult::BackToEditor;app.close();}ImGui::SameLine();if(ImGui::Button("Exit")){result=StageResult::Exit;app.close();}ImGui::Checkbox("Voice",&voiceTrackingEnabled);ImGui::Checkbox("Camera",&cameraTrackingEnabled);ImGui::Checkbox("Preview tracking",&simulateCameraTracking);ImGui::Checkbox("Use OpenCV camera",&useRealCameraTracking);if(ImGui::Checkbox("Always on top",&alwaysOnTop)) Moss_SetWindowAlwaysOnTop(app.window,alwaysOnTop);if(ImGui::Checkbox("Borderless",&borderless)) Moss_SetWindowBorderless(app.window,borderless);ImGui::Checkbox("Chroma key",&chromaKeyEnabled);if(chromaKeyEnabled)ImGui::ColorEdit4("Key color",chromaKeyColor);ImGui::SliderInt("FPS limit",&targetFps,15,240);ImGui::Text("FPS %.1f",runtimeFps);if(avatarRuntime)ImGui::TextWrapped("Avatar: %s",avatarRuntime->getStatus().c_str());if(ImGui::Button("Reload project")){loadProject(projectPath);rebuildAvatarRuntime();}ImGui::SameLine();if(ImGui::Button("Save runtime settings"))saveRuntimeSettings();ImGui::Text("Camera: %s",opencvTracker.getStatus().c_str());ImGui::End();}
void VStage::rebuildAvatarRuntime(){avatarRuntime=CreateAvatarRuntime(activeRigType);if(auto*p=dynamic_cast<PNGTuberRuntime*>(avatarRuntime.get())){p->slots=pngSlots;p->effects=pngEffects;}if(auto*l=dynamic_cast<Live2DRuntime*>(avatarRuntime.get()))l->rig=live2DRig;if(auto*m=dynamic_cast<Model3DRuntime*>(avatarRuntime.get()))m->rig=model3DRig;avatarRuntime->load(projectFolder);}
void VStage::loadProject(const std::string& filePath)
{
    ProjectDocument document;
    if (!document.load(filePath)) return;

    projectPath = filePath;
    const std::string base = std::filesystem::path(filePath).parent_path().string();
    projectFolder = document.get("project.folder", base);
    activeRigType = static_cast<AvatarRigType>(
        std::clamp(AvatarProjectSettings::integer(document, "avatarType", 0), 0, 2));

    const auto decodePath = [this](const std::string& value) {
        return asset(projectFolder, value);
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
    opencvTracker.setCalibration(cameraCalibration);

    AvatarProjectSettings::loadModel3D(document, model3DRig, decodePath);
    AvatarProjectSettings::loadLive2D(document, live2DRig, decodePath);

    targetFps = std::clamp(AvatarProjectSettings::integer(document, "stream.targetFps", targetFps), 15, 240);
    chromaKeyEnabled = AvatarProjectSettings::boolean(document, "stream.chromaKeyEnabled", chromaKeyEnabled);
    showControls = AvatarProjectSettings::boolean(document, "stream.showControls", showControls);
    alwaysOnTop = AvatarProjectSettings::boolean(document, "stream.alwaysOnTop", alwaysOnTop);
    borderless = AvatarProjectSettings::boolean(document, "stream.borderless", borderless);
    chromaKeyColor[0] = AvatarProjectSettings::number(document, "stream.chroma.r", chromaKeyColor[0]);
    chromaKeyColor[1] = AvatarProjectSettings::number(document, "stream.chroma.g", chromaKeyColor[1]);
    chromaKeyColor[2] = AvatarProjectSettings::number(document, "stream.chroma.b", chromaKeyColor[2]);
    chromaKeyColor[3] = AvatarProjectSettings::number(document, "stream.chroma.a", chromaKeyColor[3]);
    if (app.window) {
        Moss_SetWindowAlwaysOnTop(app.window, alwaysOnTop);
        Moss_SetWindowBorderless(app.window, borderless);
    }
}

bool VStage::saveRuntimeSettings()
{
    ProjectDocument document;
    if (!document.load(projectPath)) return false;
    document.set("tracking.voiceEnabled", voiceTrackingEnabled ? "1" : "0");
    document.set("tracking.cameraEnabled", cameraTrackingEnabled ? "1" : "0");
    document.set("tracking.simulationEnabled", simulateCameraTracking ? "1" : "0");
    document.set("tracking.openCVEnabled", useRealCameraTracking ? "1" : "0");
    document.set("stream.targetFps", std::to_string(targetFps));
    document.set("stream.chromaKeyEnabled", chromaKeyEnabled ? "1" : "0");
    document.set("stream.chroma.r", std::to_string(chromaKeyColor[0]));
    document.set("stream.chroma.g", std::to_string(chromaKeyColor[1]));
    document.set("stream.chroma.b", std::to_string(chromaKeyColor[2]));
    document.set("stream.chroma.a", std::to_string(chromaKeyColor[3]));
    document.set("stream.alwaysOnTop", alwaysOnTop ? "1" : "0");
    document.set("stream.borderless", borderless ? "1" : "0");
    document.set("stream.showControls", showControls ? "1" : "0");
    return document.save(projectPath);
}
void VStage::loadPNGTuberTextures(){}
void VStage::destroyPNGTuberTextures(){for(auto&t:pngTextures)MossTextureLoader::destroy(t);}
MossTexture* VStage::activePNGTuberTexture(){return nullptr;}
