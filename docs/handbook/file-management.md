# File Management System

The file management system separates three concerns that were previously tangled together: **asset caching**, **file I/O**, and **GPU resource creation**.

```
File System ──► FileIO layer ──► GPU class (MeshData / Texture / Shaders)
                                        │
                                        ▼
                              AssetManager (shared-ptr cache)
                                        │
                                        ▼
                            Domain lib ResetXxxLib() ──► Application
```

---

## AssetManager — unified asset cache

{{file("src/core/AssetManager.h")}} is a header-only, type-keyed `shared_ptr` cache. It replaces the per-class static maps that were previously scattered across `MeshLib`, `TextureLib`, and `ComputeShader`.

### Design

- Per-type registries are backed by function-local statics, avoiding the static-initialization-order problem.
- Resources are stored as `shared_ptr`; callers share ownership via the returned handle.
- `Clear<T>()` drops the manager's references; externally held `shared_ptr`s remain valid.
- `Load<T>()` accepts a factory callable to keep asset-creation logic in the owning layer.

### API

```cpp
// Load or retrieve a cached MeshData
auto mesh = AssetManager::Load<MeshData>("monkey.obj", []() {
    auto m = std::make_shared<MeshData>();
    m->LoadObj("monkey.obj");
    return m;
});

// Retrieve without loading (returns nullptr if not cached)
auto tex = AssetManager::Get<Texture>("albedo.png");

// Register a pre-built asset (e.g. procedural noise texture)
AssetManager::Register<Texture>("noise", noise_ptr);

// Query presence without obtaining a handle
bool hit = AssetManager::Contains<MeshData>("monkey.obj");

// Evict all cached entries of a type (external shared_ptrs stay valid)
AssetManager::Clear<Texture>();
```

### Domain-level Reset wrappers

`AssetManager::Clear<T>()` is a **core-level operation**. Higher-level code must not call it directly. Each domain library exposes its own named wrapper:

| Library | Reset method | What it clears |
|---|---|---|
| {{cls("TextureLib")}} | `TextureLib::ResetTexLib()` | `AssetManager::Clear<Texture>()` |
| {{cls("ComputeShader")}} | `ComputeShader::ResetComputeLib()` | `AssetManager::Clear<ComputeShader>()` + `config_list` |
| `MeshLib` | `MeshLib::ResetMeshLib()` | `AssetManager::Clear<MeshData>()` |

`Application::~Application()` calls these three wrappers in sequence to release all cached assets before the OpenGL context is destroyed.

---

## FileIO layer — `src/fileIO/`

The `src/fileIO/` directory contains stateless IO classes with **no OpenGL dependency**. They own all file-system operations; GPU classes own GL state and have no file-format knowledge.

### ShaderIO

{{file("src/fileIO/ShaderIO.h")}} / {{file("src/fileIO/ShaderIO.cpp")}}

Reads and writes GLSL source files.

```cpp
// Read a shader source file
std::string src = ShaderIO::Read("res/shaders/pps/FXAA.comp");

// Write modified GLSL back to disk
ShaderIO::Write("out/modified.comp", src);
```

`Shaders::ReadShaderFile` delegates to `ShaderIO::Read`; path-construction logic stays in `Shaders`.

### TextureIO

{{file("src/fileIO/TextureIO.h")}} / {{file("src/fileIO/TextureIO.cpp")}}

Loads and saves CPU-side image data (LDR `uint8_t` or HDR `float`) via stb_image / stb_image_write. No OpenGL types involved.

```cpp
// Load raw pixels — no GL context required
TextureIO::ImageData img = TextureIO::Load("res/tex/albedo.png");
// img.width, img.height, img.is_hdr, img.pixels / img.pixels_hdr

// Save HDR frame captured from GPU
TextureIO::SaveHDR("result/frame.hdr", w, h, hdr_pixels.data());

// Save LDR screenshot
TextureIO::SavePNG("result/screenshot.png", w, h, rgba_pixels.data());
```

`Texture`'s file-loading constructor calls `TextureIO::Load` and then uploads to the GPU. `Texture::SaveTexture` downloads from the GPU and then delegates the file write here.

#### `ImageData` struct

| Field | Type | Description |
|---|---|---|
| `width` | `int` | Image width in pixels |
| `height` | `int` | Image height in pixels |
| `channels` | `int` | Channel count in the source file |
| `is_hdr` | `bool` | `true` → HDR (float); `false` → LDR (uint8) |
| `pixels` | `vector<uint8_t>` | RGBA LDR data (populated when `!is_hdr`) |
| `pixels_hdr` | `vector<float>` | RGBA HDR data (populated when `is_hdr`) |

### MeshIO

{{file("src/fileIO/MeshIO.h")}} / {{file("src/fileIO/MeshIO.cpp")}}

Parses Wavefront OBJ files into an interleaved vertex buffer (`MeshData::ByteArray`) without any OpenGL calls.

```cpp
// Parse an OBJ file
MeshData::ByteArray data = MeshIO::Read("res/obj/monkey.obj");
// data.data_array is ready for glBufferData
// data.center is the geometric centre of the mesh
```

Vertex layout — 11 floats per vertex:

```
[ x,  y,  z ]   position
[ u,  v ]        UV coordinates
[ nx, ny, nz ]   flat face normal
[ snx, sny, snz] smooth normal (averaged over shared vertices when smooth=true)
```

`MeshData::LoadObj` delegates to `MeshIO::Read`; GPU buffer creation stays in `MeshData`.

---

## Architectural invariants

| Rule | Rationale |
|---|---|
| IO classes have zero OpenGL dependency | Enables use in unit tests, offline tools, and serialization pipelines without a GL context |
| GPU classes have no file-format knowledge | GPU code changes independently of file format changes |
| `AssetManager::Clear<T>()` is only called from domain Reset wrappers | Keeps core operations internal; application code uses named library methods |
| External `shared_ptr`s survive `Clear<T>()` | Resources can outlive the cache; no dangling-pointer hazards |

---

## Adding support for a new asset format

1. Add a new source parser in `src/fileIO/` (e.g. `GltfIO.h`). No GL headers allowed.
2. Have the GPU class delegate loading to the new IO class.
3. If the asset needs caching, call `AssetManager::Load<T>(key, factory)` from the domain lib.
4. Add a `ResetXxxLib()` wrapper that calls `AssetManager::Clear<T>()`.
5. Call the new Reset wrapper from `Application::~Application()`.

---

## Related locations

| Path | Contents |
|---|---|
| {{file("src/core/AssetManager.h")}} | Unified type-keyed cache (header-only) |
| {{file("src/fileIO/ShaderIO.h")}} | GLSL file reader / writer |
| {{file("src/fileIO/TextureIO.h")}} | Image file loader / saver (stb_image) |
| {{file("src/fileIO/MeshIO.h")}} | OBJ mesh parser |
| {{file("src/render/Texture.h")}} | GPU texture — delegates file I/O to TextureIO |
| {{file("src/render/MeshData.h")}} | GPU mesh — delegates file I/O to MeshIO |
| {{file("src/render/shaders/ComputeShader.h")}} | Compute shader — cache managed via AssetManager |
| {{file("test/renderer_test/test_assets.cpp")}} | GL-free unit tests for AssetManager |
