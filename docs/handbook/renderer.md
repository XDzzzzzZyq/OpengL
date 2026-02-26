# Renderer

The Renderer layer provides pure rendering services. It depends on immutable `Scene` data and `Context` input, and focuses on OpenGL resources and rendering flow. The Renderer must not contain application logic.

## Responsibilities

- Create and manage GPU resources (textures, buffers, shaders).
- Organize rendering flow based on `Scene` and `RenderConfig`.
- Stay isolated from Editor and UI.

## Constraints

- Must not depend on Editor or UI.
- Must not mutate application-level state.
- Resources are released explicitly via RAII.

## Related Directories

- `src/render/`: Rendering and resource management.
- `src/render/shaders/`: Shader wrappers.
- `res/shaders/`: Shader assets.
