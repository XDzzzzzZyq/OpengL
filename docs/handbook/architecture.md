# Architecture

This project follows a three-layer architecture: [Renderer](renderer.md), [Editor](editor.md), and UI. 
They communicate only through the {{cls("Context")}} and event system to keep explicit data flow and avoid direct coupling.

## Design Principles

- Renderer remains stateless and focuses on GPU resources and rendering.
- Editor owns application logic and scene mutations through controllers.
- UI is presentation-only; state changes go through controllers or events.
- GPU resources are owned explicitly by Renderer and released deterministically.
- Cross-layer communication is only through {{cls("Context")}} and the event system.

## Directory Layout

- {{file("src/render/")}}: Rendering and GPU resource management.
- {{file("src/editor/")}}: Editor logic, controllers, and events.
- {{file("src/UI/")}}: ImGui UI layer.
- {{file("src/scene/")}}: Scene objects and data structures.
- {{file("src/core/")}}: Core utilities — {{cls("AssetManager")}} and the ID system.
- {{file("src/fileIO/")}}: File I/O layer — `MeshIO`, `TextureIO`, `ShaderIO`.
- {{file("res/")}}: Assets and shaders.
- {{file("test/")}}: Unit tests.

## File Management

Asset caching and file I/O are handled by two dedicated systems that sit below the Renderer layer.

- **{{cls("AssetManager")}}** (`src/core/`) — type-keyed `shared_ptr` cache. Domain libraries (`MeshLib`, `TextureLib`, `ComputeShader`) load and retrieve assets through it. Higher-level code (Editor, Application) never calls `AssetManager::Clear<T>()` directly; each library exposes its own named Reset wrapper (`ResetMeshLib`, `ResetTexLib`, `ResetComputeLib`).
- **fileIO layer** (`src/fileIO/`) — stateless namespaces (`MeshIO`, `TextureIO`, `ShaderIO`) with zero OpenGL dependency. They own all file-system operations. GPU classes delegate file loading and saving here and have no file-format knowledge themselves.

See [File Management](file-management.md) for the full reference.
