# VAvatar Documentation

VAvatar is being shaped into one VTuber app for PNGTuber, Live2D / 2.5D, and 3D workflows. The shared idea is simple: the app captures voice and camera tracking once, converts that into a neutral tracking state, then maps that state into the selected avatar type.

## Start Here

- [How the app works](AppGuide.md)
- [Architecture](Architecture.md)
- [Tracking](Tracking.md)
- [Projects and assets](ProjectsAndAssets.md)
- [Project structure](ProjectStructure.md)
- [Build and development](BuildAndDevelopment.md)
- [Moss conversion notes](MossConversion.md)
- [Roadmap](Roadmap.md)

## Current App Flow

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

## Core Pipeline

```text
Voice Tracking + Camera Tracking
        |
        v
Neutral Tracking State
        |
        v
AvatarTrackingMapper
        |
        +-- PNGTuber sprite switching and 2D motion
        +-- Live2D / 2.5D parameters
        +-- 3D head/blendshape values
```

## What Works Now

- Moss-style app lifecycle with `VHub`, `VEditor`, and `VStage`.
- WASAPI voice tracking with level, speaking, and mouth-open output.
- Optional OpenCV camera tracking for face, head, blink, smile, and mouth-open output.
- Shared mapping for PNGTuber, Live2D / 2.5D, and 3D output values.
- PNGTuber asset slots with shared texture loading in the Editor and Stage.
- Simple `.vavatar` project save/load with asset import into project folders.
- Calibration preset save/load using `.vcal` files.
- Early Live2D / 2.5D binding editor.
