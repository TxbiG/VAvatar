# How VAvatar Works

VAvatar is organized around three windows.

## Hub

`VHub` is the starting place. Its job is to create or select a VTuber project and then send the user either into the Editor or straight into the Stage runtime.

## Editor

`VEditor` is the authoring workspace. This is where the user chooses an avatar type, configures tracking, edits PNGTuber slots, tests Live2D / 2.5D bindings, and saves `.vavatar` projects.

The Editor currently includes:

- avatar type selection for PNGTuber, Live2D, and 3D modes
- voice tracking controls
- camera tracking controls
- OpenCV model-file selection
- camera calibration sliders
- PNGTuber image slots
- project folder and project file controls
- calibration preset controls
- Live2D / 2.5D binding rows
- a scene preview

## Stage

`VStage` is the runtime window intended for streaming. It renders with a transparent clear color and keeps controls optional. For PNGTuber projects, it loads the same PNG slots used by the Editor and switches textures from the mapped tracking pose.

## Avatar Modes

PNGTuber mode switches image slots for mouth and blink states, then applies simple bounce, offset, and squish values.

Live2D / 2.5D mode exposes head, mouth, continuous eye, gaze, brow, and body parameters. Its editor configures ordered layers, grid warping, clip masks, hierarchical deformers, spring physics, visibility, and tracking bindings; Editor and VStage evaluate the same rig.

3D mode currently produces neutral head rotation, jaw-open, blink, smile, and hand-presence values that can later be bound to bones and blendshapes.

### PNGTuber state effects

Silent and talking states each have independent minimum/maximum values for two-axis bounce, squish, frequency, amplitude, rotational motion, drag, and color tint. The runtime blends talking values from the live mouth signal and applies idle motion while silent.

Expression effects are optional. When enabled, the tracked smile/expression value crosses the configured threshold, the expression image and its effect state override the normal silent/talking state. These settings are stored in the project and evaluated by the same runtime in Editor and VStage.