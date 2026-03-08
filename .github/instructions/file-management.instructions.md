# File Management System

## Overview

The file management system separates three concerns that were previously tangled together:

1. **Asset caching** — `AssetManager` (`src/core/AssetManager.h`)
2. **File I/O** — `MeshIO`, `TextureIO`, `ShaderIO` (`src/fileIO/`)
3. **GPU resource creation** — `MeshData`, `Texture`, `ComputeShader` (`src/render/`)

Each concern has clear ownership boundaries and explicit dependencies.

---

## AssetManager

### Location
- `src/core/AssetManager.h` — header-only, no compilation unit required

### Purpose
Centralised type-keyed `shared_ptr` cache. Replaces the per-class static maps that were previously in `MeshLib`, `TextureLib`, and `ComputeShader`.

### Key invariants
- Per-type registries use function-local statics; no static-initialization-order problem.
- `Load<T>(key, factory)` — returns cached instance if present; otherwise calls the factory, caches the result, and returns it.
- `Get<T>(key)` — returns `nullptr` if not cached; never calls a factory.
- `Register<T>(key, ptr)` — inserts or overwrites a cache entry (used for procedural assets).
- `Contains<T>(key)` — non-allocating presence check.
- `Clear<T>()` — drops the manager's `shared_ptr`s; externally held handles remain valid.

### Usage pattern (inside domain libs only)
```cpp
auto mesh = AssetManager::Load<MeshData>("monkey.obj", [path]() {
    auto m = std::make_shared<MeshData>();
    m->LoadObj("monkey.obj");
    return m;
});
```

### Who may call AssetManager directly
- `MeshLib`, `TextureLib`, `ComputeShader` (render layer).
- **Application and Editor MUST NOT call `AssetManager::Clear<T>()` directly.**
  They must call the domain-level Reset wrappers instead (see below).

---

## Domain-level Reset wrappers

Each domain library exposes a named Reset method. These are the **only** sanctioned way to evict assets from outside the render layer.

| Library | Method | Clears |
|---|---|---|
| `TextureLib` | `TextureLib::ResetTexLib()` | `AssetManager::Clear<Texture>()` |
| `ComputeShader` | `ComputeShader::ResetComputeLib()` | `AssetManager::Clear<ComputeShader>()` + `config_list` |
| `MeshLib` | `MeshLib::ResetMeshLib()` | `AssetManager::Clear<MeshData>()` |

`Application::~Application()` calls these three wrappers. No other code outside the render layer should touch `AssetManager::Clear<T>()`.

---

## FileIO layer

### Location
`src/fileIO/` — three stateless namespaces, no OpenGL dependency.

### Invariants
- IO classes may not include any OpenGL headers.
- IO classes may not own GPU state.
- GPU classes (`Texture`, `MeshData`, `Shaders`) delegate all file-system operations here; they contain no file-format knowledge.
- IO classes return CPU-side data only (`std::string`, `std::vector<uint8_t>`, `std::vector<float>`, `MeshData::ByteArray`).

### ShaderIO (`src/fileIO/ShaderIO.h`)
```cpp
std::string code = ShaderIO::Read(path);  // reads GLSL source
ShaderIO::Write(path, code);              // writes GLSL source
```
- `Shaders::ReadShaderFile` delegates to `ShaderIO::Read`.
- Path construction / extension selection stays in `Shaders`.

### TextureIO (`src/fileIO/TextureIO.h`)
```cpp
TextureIO::ImageData img = TextureIO::Load(path); // LDR or HDR depending on extension
TextureIO::SaveHDR(path, w, h, float_pixels);
TextureIO::SavePNG(path, w, h, uint8_pixels);
```
- `Texture` file-loading constructor: calls `TextureIO::Load`, then uploads to GPU.
- `Texture::SaveTexture`: downloads from GPU, then calls `TextureIO::SaveHDR` / `SavePNG`.

### MeshIO (`src/fileIO/MeshIO.h`)
```cpp
MeshData::ByteArray data = MeshIO::Read(path, smooth);
```
- `MeshData::LoadObj` delegates here.
- GPU buffer creation stays in `MeshData`.

---

## Extending the system

### Adding a new file format (e.g. glTF)
1. Create `src/fileIO/GltfIO.h` / `.cpp`. No GL headers.
2. Define a namespace `GltfIO` with a `Read(path) → SomeData` function.
3. Have the GPU class delegate loading here.
4. If caching is needed, call `AssetManager::Load<T>(key, factory)` from the domain lib.
5. Add a `ResetXxxLib()` wrapper and call it from `Application::~Application()`.

### Adding a new asset type to AssetManager
- No registration is required. `AssetManager` is fully templated.
- Just call `AssetManager::Load<MyType>(key, factory)` from the owning library.
- Expose `MyLib::ResetMyLib()` that wraps `AssetManager::Clear<MyType>()`.

---

## Architectural boundaries

### ✅ MAY:
- Domain libs (`MeshLib`, `TextureLib`, `ComputeShader`) call `AssetManager` directly.
- GPU classes call into `src/fileIO/` for file operations.
- `Application::~Application()` calls domain Reset wrappers.

### ❌ MUST NOT:
- Editor or Application call `AssetManager::Clear<T>()` directly.
- IO classes include `<GL/glew.h>`, `<GLFW/glfw3.h>`, or any other GPU header.
- GPU classes parse file formats inline (must delegate to `src/fileIO/`).
- Domain libs expose raw `AssetManager` operations as part of their public API.

---

## Related files

| File | Role |
|---|---|
| `src/core/AssetManager.h` | Unified type-keyed cache (header-only) |
| `src/fileIO/ShaderIO.h` / `.cpp` | GLSL file reader / writer |
| `src/fileIO/TextureIO.h` / `.cpp` | Image file loader / saver (stb_image) |
| `src/fileIO/MeshIO.h` / `.cpp` | OBJ mesh parser |
| `src/render/Texture.h` / `Texture.cpp` | GPU texture — file I/O delegated to TextureIO |
| `src/render/MeshData.h` / `MeshData.cpp` | GPU mesh — file I/O delegated to MeshIO |
| `src/render/shaders/ComputeShader.h` / `.cpp` | Compute shader — cache managed via AssetManager |
| `src/app/Application.cpp` | Calls the three domain Reset wrappers in destructor |
| `test/renderer_test/test_assets.cpp` | GL-free unit tests for AssetManager |
