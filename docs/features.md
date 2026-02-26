# Features

An overview of what the OpenGL Renderer supports.

---

## Rendering Pipeline

- **Deferred shading** — G-buffer pass for positions, normals, albedo, metallic/roughness.
- **PBR lighting** — Physically-based BRDF with support for point, sun, spot, and area lights.
- **Image-based lighting (IBL)** — HDR environment maps for diffuse irradiance and specular reflections.
- **Soft shadows** — SDF-accelerated soft shadow rendering.

## Post-Processing

- **SSAO** — Screen-space ambient occlusion.
- **SSR** — Screen-space reflections.
- **FXAA** — Fast approximate anti-aliasing.
- **Tone mapping** — Filmic tone mapping curve.
- **Gamma correction** — sRGB output.

## Editor Interface

- **Viewport** — Interactive 3D view with camera orbit, pan, and zoom.
- **Outliner** — Scene hierarchy tree with object type icons.
- **Transform panel** — Position, rotation, and scale inputs for selected objects.
- **Material viewer** — PBR material properties with live preview.
- **Render config viewer** — Adjust rendering settings in real time.
- **Shader editor** — Multi-tab GLSL editor with syntax highlighting and live recompilation.

## Scene Objects

- Meshes, cameras, lights (point, sun, spot, area), environments, and empty objects.
- Parent-child transform hierarchy.
- Unique ID system for stable object references.

## GPU Resource Management

- Explicit RAII ownership of all GPU resources.
- VertexBuffer, IndexBuffer, UniformBuffer, StorageBuffer, FrameBuffer abstractions.
- Deterministic resource cleanup.

## Build & Platform

- C++20, OpenGL 4.6.
- Windows x64 (Visual Studio / msbuild).
- Continuous integration via GitHub Actions.
