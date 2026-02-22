# Architecture Overview

## System Design Philosophy

This is a C++20 OpenGL 4.6 real-time renderer designed for experimentation with modern rendering algorithms. The architecture prioritizes:

- **Strict layer isolation** - Renderer ↔ Editor ↔ UI boundaries must not be violated
- **Minimal global state** - State is explicit and localized
- **Explicit data flow** - Communication via Context and Event systems only
- **Stateless rendering** - Renderer does not own application-level state
- **Deterministic resource management** - GPU resources have explicit ownership

## Three-Layer Architecture

```
┌─────────────────────────────────────────────────┐
│                   UI Layer                       │
│  (ImGui-based presentation, panels, menus)      │
└───────────────────┬─────────────────────────────┘
                    │ Events & Context
                    ▼
┌─────────────────────────────────────────────────┐
│                Editor Layer                      │
│  (Application logic, controllers, scene state)  │
└───────────────────┬─────────────────────────────┘
                    │ Context (read-only Scene)
                    ▼
┌─────────────────────────────────────────────────┐
│              Renderer Layer                      │
│  (Pure rendering service, GPU resources)        │
└─────────────────────────────────────────────────┘
```

### Layer Responsibilities

**Renderer Layer** (`src/render/`)
- Pure rendering service with no application logic
- Owns GPU resources (buffers, framebuffers, shaders, textures)
- Reads immutable Scene data via Context
- Consumes RenderConfigs for settings
- Must NOT mutate application state
- Must NOT depend on Editor or UI layers

**Editor Layer** (`src/editor/`)
- Contains application logic and scene mutation
- Owns Controllers (Camera, Shader, Viewport)
- Manages SelectionManager and Input handling
- Emits and subscribes to Events via EventPool
- Communicates with Renderer via Context
- Must NOT directly manipulate GPU resources

**UI Layer** (`src/UI/`)
- Pure presentation layer using ImGui
- Displays data from Context
- Sends user actions as Events or Controller calls
- Must NOT directly access Renderer internals
- Must NOT mutate scene state directly

### Communication Protocols

**Context System** (Data)
- `SceneContext` - Read-only view of active scene
- `EditorContext` - Editor state (selections, etc.)
- `RenderContext` - Render settings (RenderConfigs)
- Data flows downward: UI reads, Editor mutates, Renderer consumes

**Event System** (Signals)
- Type-safe EventPool for decoupled messaging
- Events propagate across layers
- Subscribers react to state changes
- Examples: CameraEvents, ViewportEvents, MaterialEvents

## Core Systems

### Scene Graph
- Hierarchical scene objects with Transform components
- Types: Camera, Light, Mesh, Environment, Empty, Debug objects
- Unique ID system for object identity
- Parent-child relationships for transforms

### Rendering Pipeline
- Deferred rendering with multi-pass composition
- Framebuffer management for intermediate results
- PBR lighting with IBL support
- Post-processing via compute shaders
- Shader compilation and dynamic uniforms

### GPU Resource Management
- Explicit ownership of OpenGL resources
- RAII wrappers: VertexBuffer, IndexBuffer, UniformBuffer, StorageBuffer
- FrameBuffer for render targets
- Texture and Material systems
- Resources released deterministically in destructors

### Shader System
- GLSL structure analysis and parsing
- Dynamic compilation and reloading
- Shader library for reusable code
- RenderShader (vertex + fragment)
- ComputeShader for GPGPU tasks
- ShaderEditor UI for live editing

## Design Constraints

### Architectural Invariants
1. **No cross-layer direct coupling** - Use Context/Events only
2. **Renderer is stateless** - Application state lives in Editor
3. **Immutable scene reads** - Renderer sees const Scene data
4. **Explicit GPU ownership** - No shared_ptr to GPU resources across layers
5. **Event-driven updates** - State changes propagate via EventPool

### Naming Conventions
- Classes: PascalCase (e.g., `FrameBuffer`, `CameraController`)
- Members: prefixes are common (e.g., `r_` for renderer, `o_` for object, `is_` for flags, `tex_` for textures). Match local file usage.
- Functions: PascalCase for public API; follow surrounding conventions for internals.
- Constants: UPPER_SNAKE_CASE when present; match local file usage.
- Private members: Leading underscore used for some internals (e.g., `_BuildinPPS`); match local style.

### Formatting / Includes
- Indentation: tabs are common.
- Braces: Allman style in headers and cpp files.
- Includes: local headers before system; group local, third-party (GL/GLFW/GLM), STL; use `#pragma once`.

### Ownership / Error Handling
- Prefer RAII for GPU resources; use `std::unique_ptr`/`std::shared_ptr` for explicit ownership.
- Avoid owning raw pointers; use raw pointers for non-owning references only.
- Use `GLERRTEST` / `GLDEBUG` where available; early return on invalid state.

### Extension Points
- New scene object types inherit from ObjectID
- New UI panels extend ImguiLayer
- New events added to EventPool via template subscription
- New shaders added to ShaderLib
- New post-processing effects via ComputeShader

## Performance Considerations

- Real-time rendering targets 60+ FPS
- GPU resources are pooled and reused where possible
- Batch rendering for sprites and debug geometry
- Frustum culling (TODO: acceleration structures)
- Minimize CPU-GPU data transfers

## Future Architecture Evolution

The codebase contains TODOs for architectural improvements:
- Remove stateful Renderer members (ongoing refactor)
- Move editor-specific flags to EditorContext
- Implement acceleration structures for scene culling
- Enhance shader node system
- Add path tracing support
