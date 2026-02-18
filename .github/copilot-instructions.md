## High Level Details
This is a C++20 OpenGL 4.6 real-time renderer.

Design goals:
- Strict layer isolation (Renderer ↔ Editor ↔ UI)
- Minimal global state
- Explicit data flow via Context and Event systems
- Renderer remains stateless with respect to application logic
- GPU resources are owned and managed explicitly

The engine is intended for experimentation with real-time rendering algorithms.
Performance and architectural clarity are prioritized over feature completeness.


## Build Instructions
This project depends on Visual Studio Solution. You need `msbuild` to build the renderer. You can refer to `.github/workflows/msbuild.yml`. You can stick to x64 platform. 
To run the testing, you need an OpenGL 4.6 environment to enable all tests. Otherwise, it will only perform a subset of tests. Validation expectations:
- No GL errors during execution
- No resource leaks
- All assertions must pass


## Project Layout
In general, the program contains three layers: Renderer, Editor, UI. These layers are expected to be decoupled and flexible for scaling. 

Renderer:
- Pure rendering service.
- Contains `RenderConfig` and depends on immutable `Scene`.
- Must not depend on Editor or UI.
- Must not mutate application-level state.
- GPU resources are owned explicitly and released deterministically.

Editor:
- Contains application logic and scene mutation.
- Owns Controllers.
- Communicates with Renderer via Context and Events.
- Must not directly manipulate GPU resources.

UI:
- Pure presentation layer.
- Must not directly mutate Renderer.
- All state changes go through Controllers or Events.

Communication:
- Cross-layer communication must go through:
  - Context system (data)
  - Event system (signals)

Direct coupling across layers is forbidden.

```
/src/                   → All C++ code
/src/UI                 → UI Layer based on ImGui
/src/scene/             → Scene objects (Mesh, Light, Camera, ...)
/src/editor/            → Editor Layer (Event Pool, Runtime controller)
/src/render/            → Renderer & GPU resource management
/res/                   → Resources such as icons, textures, meshes
/res/shaders/           → All shader resources
/test/                  → Unit tests
/dep/                   → Dependencies
/.github/               → CI workflows and Copilot instructions
/OpenGL.sln             → Visual Studio Solution
/README.md              → Readme for the project introduction
```

## PR Review

When reviewing pull requests:
- Review as a senior engineer performing a production-level code review.
- Focus on correctness, architecture, maintainability, and performance.
- Identify hidden coupling, ownership violations, and unclear responsibilities.
- Point out potential memory, lifetime, threading, or state bugs.
- Question unnecessary abstractions or premature generalization.
- Suggest concrete refactors when design smells appear.
- Avoid praise unless something is exceptionally well designed.
- Be direct and precise.
- If something is ambiguous, explicitly state what is unclear.

## Required Before Commit

- Code builds in Debug and Release (x64).
- No new warnings.
- All tests pass.
- No cross-layer dependency violations.
- No new OpenGL state leaks.

