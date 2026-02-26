# Renderer

The Renderer layer provides pure rendering services. It depends on immutable {{cls("Scene")}} data and {{cls("Context")}} input, and focuses on OpenGL resources and rendering flow. The Renderer must not contain application logic.

## Responsibilities

- Create and manage GPU resources ({{cls("Texture")}}, {{cls("Buffers")}}, {{cls("Shaders")}}).
- Organize rendering flow based on {{cls("Scene")}} and {{cls("RenderConfigs")}}.
- Stay isolated from Editor and UI.

## Constraints

- Must not depend on Editor or UI.
- Must not mutate application-level state.
- Resources are released explicitly via RAII.

## Related Directories

- {{file("src/render/")}}: Rendering and resource management.
- {{file("src/render/shaders/")}}: Shader wrappers.
- {{file("res/shaders/")}}: Shader assets.
