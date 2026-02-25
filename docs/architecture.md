# Architecture

This project follows a three-layer architecture: [Renderer](architecture/renderer.md), [Editor](architecture/editor.md), and UI. They communicate only through the [`Context`](http://127.0.0.1:8000/XDzzzzzZyq/OpengL/Renderer/class_context/) and event system to keep explicit data flow and avoid direct coupling.

## Design Principles

- Renderer remains stateless and focuses on GPU resources and rendering.
- Editor owns application logic and scene mutations through controllers.
- UI is presentation-only; state changes go through controllers or events.
- GPU resources are owned explicitly by Renderer and released deterministically.
- Cross-layer communication is only through [`Context`](http://127.0.0.1:8000/XDzzzzzZyq/OpengL/Renderer/class_context/) and the event system.

## Directory Layout

- `src/render/`: Rendering and GPU resource management.
- `src/editor/`: Editor logic, controllers, and events.
- `src/UI/`: ImGui UI layer.
- `src/scene/`: Scene objects and data structures.
- `res/`: Assets and shaders.
- `test/`: Unit tests.
