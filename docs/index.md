# OpenGL Renderer

A modern real-time 3D renderer built with C++20 and OpenGL 4.6.  
Designed for experimenting with physically-based rendering, post-processing effects, and modern GPU techniques.

---

## What is this?

OpenGL Renderer is an interactive rendering engine that lets you load scenes, configure materials, and visualize the results in real time. It features a clean editor interface built on ImGui, a deferred shading pipeline with PBR lighting, and a live shader editor.

---

## Highlights

- **Real-time rendering** — Deferred pipeline with PBR shading, IBL, and tone mapping.
- **Live shader editor** — Edit GLSL shaders and see changes immediately.
- **Post-processing stack** — SSAO, SSR, FXAA, and more.
- **Scene hierarchy** — Outliner, transform gizmos, and material viewer.
- **Soft shadows** — SDF-based soft shadow rendering.

---

## Getting Started

1. Clone the repository and open `OpenGL.sln` in Visual Studio.
2. Build for **x64** in either Debug or Release configuration.
3. Run the executable — a window with the editor and viewport will appear.
4. Load a scene or explore the default scene to get started.

---

## Explore the Docs

- [**Features**](features.md) — What the renderer can do.
- [**Release Notes**](release.md) — Version history and changes.
- [**Developer Handbook**](handbook/index.md) — Architecture, code guidelines, and internals.
- [**API Reference**](Renderer/classes) — Auto-generated class documentation.
