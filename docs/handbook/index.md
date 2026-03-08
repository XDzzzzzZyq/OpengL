# Developer Handbook

This section contains technical documentation for contributors and developers working on the OpenGL Renderer codebase.

---

## Contents

- [**Architecture**](architecture.md) — Three-layer design and communication patterns.
- [**Renderer Layer**](renderer.md) — GPU resources, rendering pipeline, and constraints.
- [**Editor Layer**](editor.md) — Application logic, controllers, and the event system.
- [**File Management**](file-management.md) — AssetManager cache, fileIO layer, and Reset conventions.
- [**API Reference**](../Renderer/classes) — Auto-generated class documentation.

---

## Quick Reference

### Build

```
msbuild /m /p:Configuration=Debug,Platform=x64 OpenGL.sln
msbuild /m /p:Configuration=Release,Platform=x64 OpenGL.sln
```

### Test

Test binaries are output to {{file("test/renderer_test/build/")}}.  
An OpenGL 4.6 environment is required for full coverage.  
Expectations: no GL errors, no resource leaks, all assertions pass.

### Project Layout

| Path | Contents |
|---|---|
| {{file("src/render/")}} | Renderer and GPU resource management |
| {{file("src/editor/")}} | Editor logic, controllers, events |
| {{file("src/UI/")}} | ImGui UI layer |
| {{file("src/scene/")}} | Scene objects (Mesh, Light, Camera, …) |
| {{file("src/core/")}} | Core utilities (AssetManager, ID system) |
| {{file("src/fileIO/")}} | File I/O layer (MeshIO, TextureIO, ShaderIO) |
| {{file("res/shaders/")}} | Shader assets |
| {{file("test/")}} | Unit tests |
| {{file("dep/")}} | Third-party dependencies |

---

## Code Style

- **Language**: C++20
- **Braces**: Allman style
- **Indentation**: Tabs
- **Naming**: PascalCase for types and public methods; member prefixes (`r_`, `o_`, `is_`, `tex_`) match local conventions
- **Ownership**: RAII throughout; no separate `Init()`/`Terminate()` on resource-owning classes
- **Const correctness**: `const` on all non-mutating methods; Renderer takes `const Context&`
- **Error handling**: `GLERRTEST`/`GLDEBUG` macros; early return on invalid state
