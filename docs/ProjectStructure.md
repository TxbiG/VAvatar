# Project Structure

This is the intended high-level structure of the repository.

```text
VAvatar/
  assets/      App assets such as icons.
  docs/        Project documentation and roadmap.
  external/    Third-party headers and bundled dependencies.
  res/         Runtime data and default tracking/model assets.
  scripts/     Local developer scripts.
  src/         Application source.
  test/        Tests and experiments.
```

## Source Layout

```text
src/
  core/
    app/       App flow result types.
    avatar/    Shared avatar structs and tracking mapper.
    tracking/  Audio and camera tracking.
  editor/      Hub, Editor, and ImGui/Moss app wrapper.
  runtime/     VStage streaming/runtime window.
  renderer/    Shared rendering helpers.
```

## Moss-Owned Rendering Types

Mesh, model, surface, GPU buffer, shader, and general renderer abstractions come from Moss. VAvatar only keeps app-specific rendering adapters in `src/renderer`; duplicate legacy implementations are intentionally not stored here.

## Generated Folders

Build output should stay out of the source root where possible. Local build folders are ignored by Git:

```text
build/
build-*/
out/
```

The cleanup pass moves extra local build folders under `build/_local/` when possible.
