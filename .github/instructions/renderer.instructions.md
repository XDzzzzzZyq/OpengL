# Renderer System

## Overview

The Renderer is a **pure rendering service** that consumes immutable Scene data and produces rendered frames. It must remain stateless with respect to application logic.

## Location
- `src/render/Renderer.h` - Main renderer class
- `src/render/RenderConfigs.h` - Render settings
- `src/render/buffer/` - GPU buffer abstractions
- `src/render/shaders/` - Shader management

## Core Responsibilities

1. **Framebuffer Management**
   - Create and resize framebuffers dynamically
   - Manage render targets for deferred rendering
   - Handle viewport resolution changes

2. **Scene Rendering**
   - Traverse Scene objects via Context
   - Submit geometry to GPU
   - Execute multi-pass rendering pipeline
   - Apply post-processing effects

3. **GPU Resource Ownership**
   - Create and destroy OpenGL objects
   - Manage vertex buffers, index buffers, textures
   - Handle shader compilation and linking
   - Ensure deterministic cleanup

4. **Lighting and Shading**
   - PBR material evaluation
   - Multi-light rendering (point, sun, spot, area)
   - IBL (Image-Based Lighting)
   - Shadow mapping with soft shadows

## Data Flow

```
Context (immutable Scene) → Renderer → GPU → FrameBuffer
                ↑
         RenderConfigs (settings)
```

**Input:**
- `Context` - Provides read-only access to:
  - Active Camera (view/projection matrices)
  - Active Environment (IBL textures)
  - Scene objects (Mesh, Light, etc.)
- `RenderConfigs` - User-configurable settings:
  - Resolution
  - Anti-aliasing (FXAA)
  - Ambient occlusion (SSAO)
  - Shadow quality
  - Exposure

**Output:**
- Rendered frame in `FrameBuffer`
- Accessible via `GetFrameBufferPtr()`

## Key Methods

### Initialization
```cpp
void Init(EventPool& evt);
```
- Subscribes to viewport resize events
- Initializes framebuffers
- Loads default shaders

### Rendering
```cpp
void Render(const Context& ctx, bool rend = true, bool buff = true);
```
- `ctx` - Immutable scene context
- `rend` - Enable rasterization pass
- `buff` - Enable buffer outputs
- Executes full rendering pipeline

### Frame Management
```cpp
void NewFrame();
```
- Clears buffers
- Resets per-frame state
- Prepares for next render

### Utilities
```cpp
void Reset();                         // Reset renderer state
void ConstructSDF(const Context& ctx); // Build SDF for soft shadows
void ScreenShot();                    // Save current frame to file
```

## Architectural Boundaries

### ✅ Renderer MAY:
- Read Scene data via Context (const access)
- Subscribe to Events for configuration changes
- Own GPU resources (buffers, textures, shaders)
- Emit performance metrics or warnings

### ❌ Renderer MUST NOT:
- Mutate Scene objects
- Depend on Editor or UI layers
- Store application-level state
- Directly call UI or Editor functions

## Current State and TODOs

The Renderer is undergoing a refactor to achieve true statelessness:

**Legacy State (to be removed):**
- `r_frame_width`, `r_frame_height` → Move to RenderContext
- `r_render_result` → Managed externally
- `r_buffer_list` → Centralize buffer management
- `r_render_icons`, `r_is_preview` → Move to EditorContext
- `r_light_data` → Should be computed on-demand
- `r_config` → Already accessible via RenderContext

**Target Architecture:**
- Renderer receives all state via `Render(ctx, config)`
- No mutable state between frames
- Pure function: `(Context, Config) → FrameBuffer`

## GPU Resource Ownership

### Buffer Types
- **VertexBuffer** - Vertex attribute data (positions, normals, UVs)
- **IndexBuffer** - Triangle indices
- **UniformBuffer** - Per-draw shader uniforms
- **StorageBuffer** - Large read/write data (lights, compute results)
- **FrameBuffer** - Render targets with color/depth attachments
- **RenderBuffer** - Non-texture attachments (depth/stencil)

### Lifetime Management
- Resources created in `Init()` or on-demand
- RAII ensures cleanup in destructors
- Explicit `Reset()` for manual cleanup
- No GPU resource leaks tolerated

## Rendering Pipeline Stages

1. **Geometry Pass** (Deferred)
   - G-Buffer output: positions, normals, albedo, metallic/roughness
   - Depth buffer for occlusion

2. **Lighting Pass**
   - Read G-Buffer
   - Accumulate lighting from all sources
   - Apply PBR BRDF

3. **IBL Pass**
   - Sample environment maps
   - Diffuse irradiance + specular reflections

4. **Post-Processing**
   - SSAO (Screen-Space Ambient Occlusion)
   - SSR (Screen-Space Reflections)
   - Shadow composition
   - Anti-aliasing (FXAA)
   - Tone mapping (Filmic)

5. **Composite to Final**
   - Combine all passes
   - Gamma correction
   - Output to viewport framebuffer

## Performance Guidelines

- Minimize state changes (batch by material/shader)
- Use instancing for repeated geometry
- Cull objects outside frustum
- Lazy-update GPU buffers (only on change)
- Profile GPU time per pass
- Target 16ms frame budget (60 FPS)

## Error Handling

- Check OpenGL errors after major operations
- Validate framebuffer completeness
- Log shader compilation failures
- Assert on precondition violations
- Graceful degradation on unsupported features
