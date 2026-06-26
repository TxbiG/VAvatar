<div align="center">
    <img src="assets/icons/VAvatar.ico" width="128" alt="VAvatar Logo">

# VAvatar
**An open-source, cross-platform VTuber application.**
</div>

## Overview

VAvatar is a modern, open-source VTuber application built in C++17 with a focus on performance, portability, and flexibility.

The goal is to provide a single application capable of supporting multiple avatar types and tracking methods while remaining lightweight and fully cross-platform.

Supporting PNGTubers, Live2D-style avatars, and 3D avatars with modern graphics APIs.

> **Project Status**
>
> VAvatar is currently under active incremental development. Features and APIs may change as development progresses.

---

## Features

### Rendering

* OpenGL
* Vulkan
* DirectX 12
* Metal

### Platforms

* Windows
* Linux
* macOS

### Avatar Support

* PNGTubers
* 2D Realism
* 3D VTubers

### Tracking

* Audio-based lip sync
* Webcam face tracking

---

## Planned Features

* Physics and collision system
* Blendshape animation
* Expression editor
* Scene editor
* Plugin system
* OBS integration
* Stream overlays
* YouTube alerts
* Twitch alerts
* Kick alerts

---

## Screenshots

> Coming soon.

---

## Examples

> Example projects and sample avatars will be added in future releases.

---

## Documentation

Documentation is currently being written.

Future documentation will include:

* Getting Started
* Building from Source
* Avatar Creation
* Tracking Configuration
* Rendering Backends
* Plugin Development

---

# Building

## Requirements

* CMake 3.20+
* C++17 compatible compiler

Clone the repository:

```bash
git clone https://github.com/TxbiG/VAvatar.git
cd VAvatar
```

Build:

```bash
mkdir build
cd build

cmake ..

# Debug
cmake --build . --config Debug

# Release
cmake --build . --config Release
```

---

## Roadmap

* [ ] Physics and collision support
* [ ] Webcam tracking improvements
* [ ] Audio tracking improvements
* [ ] Blendshape animation
* [ ] Live2D support
* [ ] Plugin API
* [ ] OBS integration
* [ ] YouTube API alerts
* [ ] Twitch API alerts
* [ ] Kick API alerts

---

## Contributing

Contributions are welcome!

Feel free to:

* Open an issue
* Submit a pull request
* Suggest new features
* Report bugs

---

## License

This project is licensed under the MIT License.
