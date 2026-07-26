# Architecture

VAvatar is moving toward a Moss-backed architecture with a shared tracking core and avatar-specific output layers.

## Main Modules

`src/main.cpp` runs the app flow between Hub, Editor, and Stage.

`src/editor` contains the Hub and Editor windows plus the ImGui/Moss window wrapper.

`src/runtime` contains `VStage`, the runtime output window for streaming.

`src/core/app` contains small app-flow result types.

`src/core/avatar` contains shared avatar data and the `AvatarTrackingMapper`.

`src/core/tracking` contains voice and camera tracking.

`src/renderer` contains the current shared texture helper used by Editor and Stage.

`src/Moss` contains the temporary Moss platform shim used while the external Moss framework integration is stabilized.

## Tracking To Avatar Output

Tracking is intentionally avatar-neutral:

```text
AudioTracking
CameraTrackingOpenCV / preview camera data
        |
        v
FaceTrackingState + voice mouth-open
        |
        v
AvatarTrackingMapper
        |
        v
AvatarTrackingPose
```

`AvatarTrackingPose` contains output sections for PNGTuber, Live2D / 2.5D, and 3D. This keeps tracking work reusable across all avatar types.

## Texture Path

`MossTextureLoader` currently loads images through GDI+ into OpenGL textures. This is shared by the Editor preview and VStage runtime. Later, this should move behind the final Moss renderer texture API once Moss exposes texture resources cleanly.

## Project Path

The Editor saves `.vavatar` files and imports selected assets into the project folder:

```text
ProjectFolder/
  project.vavatar
  Assets/
  Tracking/
  Calibration/
```

Paths are stored relative to the project folder when possible.
