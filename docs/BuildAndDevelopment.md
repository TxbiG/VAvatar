# Build And Development

VAvatar is a C++17 CMake project.

## Normal Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

For Release:

```powershell
cmake --build build --config Release
```

## Build Check Script

Use:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/check-build.ps1
```

The script checks for CMake and a supported C++ build tool. If no compiler is available, it prints a clear message instead of failing mysteriously.

## Toolchain Notes

On Windows, run builds from a Visual Studio Developer PowerShell or install Visual Studio Build Tools / Ninja.

This Codex environment currently has CMake available but does not expose `cl`, `nmake`, Ninja, or Git on PATH, so local build verification may be limited until the developer toolchain is available.

## Optional OpenCV Tracking

OpenCV camera tracking is optional. If OpenCV is not found, VAvatar can still build and use preview/simulation tracking.

For real camera tracking, OpenCV needs the `face` module from OpenCV contrib.
