# Features

An overview of what the OpenGL Renderer supports. Items marked ✅ are complete; items marked 🔲 are planned or in progress.

---

## Scene Objects

### Transforms & Selection

- ✅ Basic transformation (position, rotation, scale)
  - ✅ Parenting
  - ✅ Transform handles (gizmos)
- ✅ Unique ID generator
- ✅ Select operation

### Object Types

- ✅ Mesh
  - ✅ Smooth normals
  - ✅ Material
  - 🔲 Mesh editing
- ✅ Sprite
  - 🔲 Batch rendering
- ✅ Camera
  - ✅ Frustum matrix
  - ✅ Camera transform
  - ✅ Adjustable parameters
  - 🔲 Debug outline
- ✅ Debug Line
- ✅ Debug Points
  - ✅ Multi-point push
  - ✅ Dynamic color
- 🔲 Debug Mesh
- 🔲 Image Plane
- ✅ Light
  - ✅ Point light
  - ✅ Sun light
  - ✅ Spot light
  - ✅ Area light
- ✅ Environment
  - ✅ IBL
  - 🔲 Z-Fog
  - 🔲 Atmosphere shading
- ✅ Post Processing
  - ✅ Screen-space effects
  - ✅ Compute shader support
  - 🔲 Field effects
- ✅ Signed Distance Field
  - 🔲 Outline
  - 🔲 Transform

---

## Rendering Pipeline

### Core

- ✅ Z-buffer
- ✅ Dynamic framebuffer
- ✅ Multi-pass composition (deferred rendering)
- ✅ Filmic tone mapping
- ✅ Gamma correction
- ✅ Selection highlight
- 🔲 Stencil buffer

### Anti-Aliasing

- ✅ FXAA (Fast Approximate Anti-Aliasing)
- 🔲 MSAA
- 🔲 TAA

### Ambient Occlusion

- ✅ SSAO (Screen-Space Ambient Occlusion)
- 🔲 GTAO
- 🔲 HBAO
- 🔲 Denoising

### PBR

- ✅ Material system
- ✅ PBR lighting — point, sun, spot, and area lights
- ✅ PBR IBL — diffuse irradiance and specular reflections
- 🔲 Normal map
- 🔲 Bump map

### Shadows

- ✅ Shadow mapping
- ✅ Soft shadows
  - ✅ SDF soft shadow
  - ✅ Variance Soft Shadow Mapping (VSSM)
  - 🔲 Moment Soft Shadow Mapping (MSSM)
  - 🔲 SDF shadow improvement

### Ray Tracing / Screen-Space

- ✅ Screen Space Reflections (SSR)
- ✅ SDF Screen Space Reflection
- ✅ Stochastic Screen Space Reflection
- 🔲 Path tracing

---

## Shaders

- ✅ Vertex & Fragment shaders
- ✅ Compute shaders
- ✅ Geometry shaders

### Vertex / Fragment Shader Features

- ✅ GLSL structure analysis (parser)
  - ✅ Structure parser
  - ✅ Material params from built-in lib
  - 🔲 Advanced analysis
  - 🔲 Full built-in library
- ✅ Storage buffer
- ✅ Uniform buffer
- ✅ Dynamic compilation
- ✅ Dynamic uniforms
- 🔲 Node system

### Compute Shader Features

- ✅ Texture computation (equirectangular ↔ cube map)
- ✅ Rendering pipeline compute passes (SSAO, FXAA, shadow mapping)
- ✅ Shader editor support
- 🔲 Structure parser for compute shaders

### Geometry Shader Features

- ✅ SDF construction

---

## Editor Interface

- ✅ Viewport — interactive 3D view with camera orbit, pan, and zoom
- ✅ Outliner — scene hierarchy tree with object type icons
- ✅ Transform panel — position, rotation, and scale inputs for selected objects
- ✅ Material viewer — PBR material properties with live preview
- ✅ Render config viewer — adjust rendering settings in real time
- ✅ Shader editor — multi-tab GLSL editor with syntax highlighting and live recompilation
  - ✅ Structure editor
  - ✅ Code editor
  - 🔲 Node editor
- ✅ Parameters panel
- ✅ Menu layer
- ✅ Gizmos (transform handles)
- ✅ Docking layout
- ✅ Multi-viewport support
- 🔲 Theme system
- 🔲 Tools layer
- ✅ Files management
  - ✅ Unified asset cache (AssetManager)
  - ✅ Dedicated file I/O layer (MeshIO, TextureIO, ShaderIO)
- 🔲 Node system

---

## Advanced / Other

- ✅ Render mode switch (averaging mode)
- ✅ Dynamic exposure
- ✅ Objects pool (Scene Manager) with pool rendering
  - 🔲 Render filter
  - 🔲 Acceleration structure
- ✅ Model importer
  - ✅ Built-in mesh data
  - ✅ `.obj` importer
  - 🔲 Advanced speed-up
- ✅ Texture importer
  - ✅ `.png` importer
  - ✅ `.hdr` importer
  - 🔲 Multi-texture importer
- ✅ Event listener (global events, global randoms)
- ✅ Timer
- 🔲 CUDA acceleration

---

## GPU Resource Management

- ✅ Explicit RAII ownership of all GPU resources.
- ✅ VertexBuffer, IndexBuffer, UniformBuffer, StorageBuffer, FrameBuffer abstractions.
- ✅ Deterministic resource cleanup.
- ✅ Unified asset cache (AssetManager) — replaces per-class static maps in MeshLib, TextureLib, ComputeShader.
- ✅ File I/O layer (MeshIO, TextureIO, ShaderIO) — zero OpenGL dependency; separates file access from GPU state.

## Build & Platform

- C++20, OpenGL 4.6.
- Windows x64 (Visual Studio / msbuild).
- Continuous integration via GitHub Actions.
