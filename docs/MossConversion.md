# VAvatar Moss Conversion

This pass moves VAvatar's application lifecycle from direct GLFW ownership to a Moss-style platform layer:

- `App` now creates windows through `Moss_CreateWindow`.
- Frame polling, close state, context activation, vsync, and buffer swapping go through Moss-named APIs.
- VHub and VEditor are usable Moss-backed ImGui windows.
- The old non-compiling editor implementation was replaced with a small editor shell so avatar systems can be migrated incrementally.

VAvatar consumes the framework directly through `add_subdirectory(${MOSS_FRAMEWORK_DIR})` and links to `Moss::Moss`. Mesh, model, surface, GPU buffer, shader, and platform ownership therefore stay in Moss rather than being duplicated in VAvatar.

Recommended next migrations:

1. Move the temporary PNGTuber OpenGL texture adapter onto Moss renderer textures once the required ImGui texture bridge is exposed.
2. Replace the local camera list with `Moss_GetCameras`.
3. Replace the placeholder editor scene view with a Moss renderer/subviewport.

## Tracking

Voice tracking now uses a continuous WASAPI capture thread and exposes:

- raw microphone RMS level
- smoothed level
- speaking/quiet state
- threshold control
- normalized mouth-open value

Camera tracking now has a `FaceTrackingState` model for face presence, head rotation, blink state, smile, mouth-open, and hand count. The editor and VStage can drive that state from either preview simulation data or the optional OpenCV backend in `src/core/tracking/Camera/OpenCV`.

The OpenCV backend looks for:

```text
res/data/haarcascade_frontalface_default.xml
res/data/lbfmodel.yaml
```

If OpenCV or those model files are unavailable, VAvatar keeps using preview/simulation tracking so the app can still be developed and demonstrated.

Recommended tracking steps:

1. Add UI/file-pickers for selecting OpenCV cascade and landmark model files.
2. Add camera device listing, ideally through `Moss_GetCameras`.
3. Add a Moss microphone adapter behind `AudioTracking` once the Moss microphone public API is consistent across headers and platform implementation.
4. Add hand tracking from a dedicated tracker instead of the current neutral hand-count placeholder.

## Avatar Rig Mapping

Tracking is now routed through `AvatarTrackingMapper`, which keeps the app as one tracker feeding three avatar styles:

- PNGTuber: sprite mouth/blink switching, bounce, squish, tilt, and 2D offsets.
- Live2D / 2.5D: `ParamAngleX`, `ParamAngleY`, `ParamAngleZ`, `ParamMouthOpenY`, eye open values, mouth form, and body angle.
- 3D VTuber: head rotation, jaw-open blendshape, blink blendshapes, smile blendshape, and hand presence.

The editor's avatar type selector uses the same camera and voice input for all three modes. This means the OpenCV camera tracker only needs to produce one neutral `FaceTrackingState`, then each avatar type can interpret it through its own output pose.

## App Lifecycle

The app now uses explicit windows and transitions:

- `VHub`: create/select a VTuber project, then open the editor or go live.
- `VEditor`: authoring environment for avatar setup, tracking, rig mapping, and preview.
- `VStage`: runtime/streaming output window for the tracked avatar.

`VStage` replaces the old `Viewport` class concept. The editor can still have a scene-view panel, but the streamable output mode is now named separately so the app flow is easier to reason about.

Current transition model:

```text
VHub
  |-- Open Editor --> VEditor
  |-- Go Live -----> VStage
  |-- Exit --------> Quit

VEditor
  |-- Back to Hub -> VHub
  |-- Go Live ----> VStage
  |-- Exit -------> Quit

VStage
  |-- Back to Editor -> VEditor
  |-- Exit ----------> Quit
```

`VStage` is intended to become the transparent or capture-friendly streaming surface. It should eventually render only the tracked avatar and keep controls optional.
