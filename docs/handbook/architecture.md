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
- {{file("res/")}}: Assets and shaders.
- {{file("test/")}}: Unit tests.
