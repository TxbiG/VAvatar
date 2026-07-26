# Projects And Assets

VAvatar projects use a versioned JSON `.vavatar` document. Legacy key/value projects are migrated automatically when opened. Missing assets are reported in the Editor and can be repaired in place.

## Project Folder

The intended project layout is:

```text
ProjectFolder/
  project.vavatar
  Assets/
    base.png
    mouth-open.png
    mouth-closed.png
    blink.png
  Tracking/
    haarcascade_frontalface_default.xml
    lbfmodel.yaml
  Calibration/
    Default.vcal
```

When saving, the Editor imports selected PNG assets into `Assets/` and tracking model files into `Tracking/`. The project file stores relative paths when possible.

## PNGTuber Slots

PNGTuber projects can use:

- base image
- mouth open image
- mouth closed image
- blink image
- expression image

The mapped PNGTuber pose decides which slot is active:

- blink slot wins when blink is active
- mouth-open slot wins when speaking/open
- mouth-closed slot is used when available
- base image is the fallback

## Calibration Presets

Calibration presets are `.vcal` files. They store voice and camera calibration values so a creator can reuse settings across sessions or avatars.

## Live2D / 2.5D Bindings

The Editor has a shared Live2D / 2.5D binding table used by both Editor and VStage. Each row has:

- parameter name
- tracking source
- curve type
- min/max output range
- smoothing
- invert toggle

The binding editor now evaluates tracking sources through linear, eased, responsive, or snap curves with per-binding smoothing. Evaluated parameter state is shared by the Editor and VStage runtime and persists in the project file.

## 3D Rig Profiles

3D projects store the model path, head/neck/chest bone names, jaw/blink/smile blendshape names, and all follow weights. VAvatar evaluates these controls from the shared tracking pose. Moss exposes the skeletal and morph runtime contract; FBX/glTF/GLB file import remains the deferred loader milestone.

## Project Schema v3

Schema v3 persists camera offsets, scales, thresholds and smoothing; tracking channel toggles; PNGTuber effects; 3D follow weights; and every Live2D layer, mesh, mask, deformer, physics and binding field. Legacy key/value projects and JSON v2 projects migrate automatically when loaded. Unknown future schema versions are rejected with a clear loading error.

## Custom Live2D / 2.5D Deformation

Each image layer supports a configurable grid mesh (up to 32 by 32 cells), X/Y parameter-driven warp, attachment to a named hierarchical deformer, clipping to a named mask layer, and damped spring physics. Deformers support parent chains, pivots, translation, scale, rotation and parameter-driven offsets. These features describe VAvatar's custom layered runtime; proprietary Cubism model files still require the official Cubism SDK integration path.