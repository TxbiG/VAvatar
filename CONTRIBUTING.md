# Contributing to VAvatar

Thank you for contributing to **VAvatar**, a VTuber software tool for 2D and 3D avatar workflows.

VAvatar is a cross-platform C++ application with tracking, avatar, rendering, and user-interface functionality. Contributions should prioritise reliable real-time behaviour, clear interfaces, portability, and straightforward user workflows.

## Before You Start

For significant changes to tracking architecture, avatar formats, rendering, or public interfaces, open an issue before substantial implementation.

Please search existing issues and pull requests first.

## Development Requirements

- Git
- CMake
- C++17-capable compiler
- Platform SDKs required by the feature being changed
- Suitable camera/microphone or test assets for tracking-related work

## Building

```bash
git clone https://github.com/TxbiG/VAvatar.git
cd VAvatar

cmake -S . -B build
cmake --build build
```

Use the repository's existing platform/build documentation when additional dependencies are required.

## Repository Structure

- `src/` — application source
- `docs/` — documentation
- `assets/icons/` — application icons
- `res/` — resources
- `external/include/` — external headers/dependencies
- `test/PNG/` — PNG testing assets
- `.github/workflows/` — CI

## Areas for Contribution

Useful contributions include:

- Camera and face tracking
- Body tracking
- Voice/audio tracking
- PNGTuber workflows
- Live2D/2.5D support
- 3D avatar support
- Rendering
- Animation and parameter mapping
- UI/UX
- Input
- Asset loading and validation
- Performance
- Platform support
- Tests and documentation
- Build and CI improvements

## Tracking Architecture

Keep the following concerns separated where practical:

1. Input capture
2. Neutral tracking data
3. Avatar-specific parameter mapping
4. Rendering/output

Avoid coupling one tracking source directly to one avatar format unless the feature genuinely requires it.

## Avatar and Asset Changes

When adding a format or asset workflow:

- Document the supported format/version.
- Do not commit copyrighted user assets without permission.
- Prefer small, redistributable test assets.
- Validate malformed or incomplete data where practical.

## Real-Time Performance

Tracking and rendering commonly run on frame-sensitive paths.

Avoid unnecessary allocations, blocking operations, or expensive work on every frame. For performance-focused changes, provide before/after measurements where practical.

## Testing

Test relevant combinations of:

- Tracking input
- Avatar type
- Rendering path
- Window/display configuration
- Camera/microphone availability

For tracking bugs, provide a reproducible description without requiring contributors to obtain private recordings or personal data.

## Commit Messages

Recommended prefixes:

```text
feat: add camera tracking parameter
fix: prevent invalid avatar parameter access
docs: document Live2D mapping
test: add tracking regression case
perf: reduce per-frame allocations
refactor: separate avatar mapping layer
build: update CMake configuration
```

## Pull Requests

Include:

- What changed
- Why it changed
- How it was tested
- Affected avatar formats/platforms
- Screenshots or recordings for visible UI changes when useful
- Performance measurements for performance-sensitive work

## Reporting Bugs

Include:

- VAvatar commit/version
- Operating system
- Compiler/build configuration
- Avatar type
- Tracking input used
- Reproduction steps
- Expected behaviour
- Actual behaviour
- Logs, screenshots, or minimal test assets where useful

## Privacy and Security

VAvatar can process camera, microphone, tracking, and avatar data. Never include private recordings, credentials, personal information, or other sensitive material in public issues or pull requests.

Report security-sensitive issues privately when public disclosure could put users at risk.

## Licence

VAvatar is distributed under the **MIT License**. Contributions should be compatible with the repository's licence and applicable third-party licence requirements.

Thank you for contributing to VAvatar.
