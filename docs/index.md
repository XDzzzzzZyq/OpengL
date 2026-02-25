# OpenGL Renderer Documentation

This project is a C++20 OpenGL 4.6 real-time renderer focused on experimentation with rendering algorithms. Performance and architectural clarity are prioritized over feature completeness.

## Design Goals

- Strict layer isolation (Renderer ↔ Editor ↔ UI)
- Minimal global state
- Explicit data flow via `Context` and event systems
- Renderer remains stateless with respect to application logic
- GPU resources are owned and managed explicitly

## Layers

- Renderer: Pure rendering service depending on immutable `Scene` and `RenderConfig`.
- Editor: Application logic and scene mutation through controllers.
- UI: Presentation-only layer driven through controllers or events.

## Build & Test

- Build with Visual Studio `msbuild` (see `.github/workflows/msbuild.yml`, x64).
- Tests require an OpenGL 4.6 environment for full coverage.
- Validation expectations: no GL errors, no resource leaks, all assertions pass.

## Project Layout

- `src/`: C++ source code
- `src/UI/`: ImGui UI layer
- `src/scene/`: Scene objects (Mesh, Light, Camera, ...)
- `src/editor/`: Editor layer (events, controllers)
- `src/render/`: Renderer and GPU resource management
- `res/`: Assets and shaders
- `res/shaders/`: Shader assets
- `test/`: Unit tests
- `dep/`: Dependencies
- `.github/`: CI workflows and Copilot instructions
- `OpenGL.sln`: Visual Studio solution
