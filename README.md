<div align="center"><img width=100 height=100 src="https://github.com/TxbiG/VAvatar/blob/main/assets/icons/VAvatar.ico"/></div>

# VAvatar

VAvatar is an open-source VTuber app for PNGTuber, Live2D / 2.5D, and 3D VTuber workflows.

The current architecture is built around one shared tracking pipeline. Voice and camera tracking create neutral tracking data, and the app maps that data into the selected avatar type.

## Current Status

VAvatar is in active incremental development. The current app has:

- Moss-style app lifecycle.
- Hub, Editor, and Stage windows.
- PNGTuber asset slots and texture switching in the Editor and Stage.
- WASAPI voice tracking.
- Optional OpenCV camera tracking.
- Shared tracking mapper for PNGTuber, Live2D / 2.5D, and 3D output values.
- Simple `.vavatar` project save/load.
- Project asset import into project folders.
- Calibration preset save/load.
- Custom Live2D / 2.5D grid deformation, masks, hierarchical deformers, spring physics, and binding editor.

## Documentation

Start with the documentation index:

- [Documentation index](docs/README.md)
- [How the app works](docs/AppGuide.md)
- [Architecture](docs/Architecture.md)
- [Tracking](docs/Tracking.md)
- [Projects and assets](docs/ProjectsAndAssets.md)
- [Project structure](docs/ProjectStructure.md)
- [Build and development](docs/BuildAndDevelopment.md)
- [Roadmap](docs/Roadmap.md)

## Build

VAvatar uses C++17 and CMake.

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Or run the build-check helper:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/check-build.ps1
```

OpenCV camera tracking is optional. If OpenCV is not found by CMake, VAvatar can still use preview/simulation tracking.
