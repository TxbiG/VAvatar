# VAvatar Roadmap

VAvatar is one Moss-backed VTuber application with a shared tracking pipeline and three avatar runtimes: PNGTuber, custom Live2D/2.5D, and 3D. Tracking stays avatar-neutral; each runtime maps the same pose into its own rendering controls.

## Current Status

| Phase | Status | Result |
| --- | --- | --- |
| 1. Foundation and Moss migration | Implemented | One Moss window lifecycle, Moss audio/platform integration, unified ImGui Moss platform backend, and no GLFW ownership. |
| 2. Shared tracking core | Implemented | Neutral voice/camera state, continuous face channels, smoothing, confidence, disconnect recovery, and hand-landmark structures. |
| 3. Avatar mapping | Implemented | Shared tracking maps into PNGTuber state, Live2D parameters, and 3D bone/morph targets. |
| 4. PNGTuber | Implemented | Asset slots, state effects, expressions, texture switching, calibration, Editor preview, and VStage runtime. |
| 5. Custom Live2D/2.5D | Implemented | Ordered layers, bindings, pivots, depth, opacity, parallax, mesh warping, masks, hierarchical deformers, physics, and shared Editor/VStage rendering. |
| 6. 3D avatars | In progress | Runtime contract, hierarchy/bone/morph data, searchable bindings, VRM suggestions, validation, pose mapping, and profile persistence are present. Actual FBX/glTF/GLB import and complete mesh/material rendering remain. |
| 7. Calibration and profiles | Implemented | Voice/camera calibration and named per-avatar profiles persist in schema-v3 projects. |
| 8. Runtime/streaming | Implemented MVP | Transparent/chroma output, always-on-top, borderless capture mode, FPS limiting, statistics, hideable overlay, and device recovery. |
| 9. Project and asset pipeline | Implemented | Versioned JSON schema, legacy migration, relative imports, missing-asset diagnostics/repair, validation, and shared Editor/VStage persistence. |
| 10. Testing and release | Pending toolchain | Static checks and a build-check script exist; compiler-backed validation, long-session testing, examples, and packaging remain. |

## Implemented Architecture

```text
Microphone + Camera
        |
        v
Neutral TrackingFrame
        |
        v
AvatarTrackingMapper
        |
        v
IAvatarRuntime
  load / unload / applyTrackingPose / update / render / getStatus
        |
        +-- PNGTuberRuntime
        +-- Live2DRuntime
        +-- Model3DRuntime
```

Editor and VStage both use this runtime contract and the same project serialization helpers. Avatar behavior is no longer duplicated between preview and stage mode.

## Completed Feature Set

### Moss and ImGui

- VAvatar links to the exported `Moss::Moss` target.
- `App` owns a `Moss_Window*`; no `GLFWwindow` or GLFW backend remains.
- `imgui_impl_moss` supplies display state, timing, focus, keyboard, modifiers, mouse buttons, pointer position, wheel events, Unicode text input, and clipboard access.
- The OpenGL ImGui backend remains only for drawing ImGui through the active Moss OpenGL context.
- Moss exposes native always-on-top and borderless window controls used by VStage.

### Tracking and mapping

- Continuous microphone level, speaking state, threshold calibration, and mouth response.
- Camera selection, face detection, landmarks, head rotation, continuous eye openness, gaze, brows, mouth-open, mouth-form, smile, shoulders/body orientation, and hand landmark structures.
- Temporal smoothing, confidence values, calibration offsets/ranges, and device-disconnect status.
- Shared PNGTuber, Live2D/2.5D, and 3D pose outputs.

Some camera channels are inferred from conventional OpenCV landmarks and remain quality-dependent on the camera, model files, lighting, and visibility. This is a tracking-quality limitation rather than a missing runtime channel.

### Avatar runtimes

- PNGTuber image states for idle/talking/blink/expression plus per-state bounce, squish, frequency, amplitude, rotation, drag, and colour effects.
- Custom Live2D/2.5D layer import and ordering, parameter bindings, layer switching, mesh deformation, named masks, deformers, and spring physics.
- 3D rig discovery/validation interfaces for nodes, bones, inverse bind data, animations, and named morph targets, with APIs for bone transforms, morph weights, destruction, and load errors.
- Searchable bone/morph selectors, common VRM-name suggestions, missing-binding warnings, and tracking-channel connection status.

The custom Live2D runtime is not a Cubism SDK loader. Official Cubism model loading, Cubism-specific masks/deformers, and Cubism physics remain an optional licensed integration.

### Projects and streaming

- Schema-v3 structured JSON projects with safe legacy migration.
- Relative asset paths, project asset imports, missing-file diagnostics and replacement actions.
- Persistence for tracking toggles, calibration, PNGTuber effects, 3D follow weights/bindings, and Live2D layers, masks, mesh, deformers, physics, visibility, and parameter bindings.
- Project validation before VStage.
- Transparent/chroma backgrounds, capture-friendly borderless output, always-on-top, FPS limits/statistics, hideable controls, and microphone/camera recovery.

## Remaining Core Work

These are the items required to call the complete three-format product release-ready:

1. Implement the deferred FBX/glTF/GLB importer and connect imported meshes, materials, textures, node hierarchy, skinning, animations, and morph targets to `Model3DRuntime`.
2. Run `scripts/check-build.ps1` from a Visual Studio Developer PowerShell or CI runner with a C++ compiler installed; fix any compiler-specific integration errors.
3. Perform a manual long-session checklist for PNGTuber, 2.5D, 3D, project migration/repair, device disconnects, and capture modes.
4. Add example projects and produce the first zipped or installed release package.

## Optional Follow-ups

- Official Live2D Cubism SDK integration, subject to SDK licensing and distribution rules.
- Native Moss texture-resource replacement for the remaining shared OpenGL texture helper once the required Moss texture API is available.
- Higher-quality tracking backends for dense face mesh and robust 3D hand pose.
- Calibration profile rename, duplicate, and delete conveniences.

## Definition of Done

VAvatar reaches the full roadmap finish line when all three runtime formats can load real assets, the build passes in a supported Windows compiler environment, projects survive migration and missing-asset repair, and a full capture session completes without lifecycle or device-recovery failures.