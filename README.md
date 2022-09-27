# OpenGL_Test

##### Opengl based renderer development from XDzZyq

# RECORD

https://space.bilibili.com/302469604/channel/collectiondetail?sid=572334

# TUTORIALs 

https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-

# FEATUREs

#### **Objects**

- [x] baisc transform
- [ ] transform handle
- [ ] parenting
- [x] unique ID generator
- [x] object name generator
- [x] select operation

- [x] Mesh
  - [x] smooth normal
  - [ ] dynamic mesh
- [x] Spirit
  - [ ] batch rendering
- [x] Camera
  - [x] frustum matrix
  - [ ] Debug outline
  - [x] Camera transform
  - [x] dynamic parameters
- [x] Debug Line
- [ ] Debug Mesh
- [x] Debug Points
  - [x] multi-points push
  - [x] dynamic color
- [ ] Image Plane
- [x] Light
  - [x] point light
  - [ ] sun light
  - [ ] spot light
- [x] Environment
  - [x] HDR texture
  - [ ] z-Fog
  - [ ] atmosphere shading

#### **Render**

- **Algorithms**

  - [x] Z-buffer
  - [x] Dynamic frame buffer
  - [ ] Stencil-buffer
  - [x] Fimic convert
  - [x] Gamma convert
  - [x] HDR rendering
  - [x] outline shader (high-light)
  - [ ] FXAA
- [ ] multi-pass composition (deferred rendering)
  - Blinn-Phong

    - [x] diffuse
    - [x] specular
    - [x] environment
    - [x] IBL environment
    - [x] IBL lighting
      - [ ] roughness blur
    - [ ] AO
    - [ ] shadow
      - [ ] shadow map
      - [ ] soft shadow
  - PBR

    - [ ] material system
  
    - [ ] pbr diffuse
    - [ ] pbr specular
- Ray tracing ?
- DL style transfer ?

- **Shader**
  - [x] GLSL interpreter(sturcture analysis)
    - [ ] Build-in Lib
    - [ ] Advanced analysis
  - [x] dynamic compiling
  - [ ] dynamic uniform
    - [ ] node system
  - [x] shader storage buffer
- **Advance**
- render mode switch
- visible switch
- dynamic exposure  

#### **UI**
- [x] Basic Debug
- [x] Docking
- [x] Multi-viewports
    - [x] Viewport
- [ ] Theme System
- [x] UI manager
    - [x] Parameters Layer
    - [x] Menu Layer
    - [ ] Tools Layer
    - [x] OutLiner Layer
    - [x] Shader Editor Layer
        - [x] Structure Editor
        - [x] Code Editor
        - [ ] Node Editor
    - [ ] Transform(Property) Layer
- [x] Imgui parameters flow

- [ ] files management
- [ ] node system

#### **Other**

- [x] objects pool (renderer class)
  - [x] pool rendering
    - [ ] advance speed up
- [x] model importer
  - [x] .obj importer
  - [ ] advance speed up
- [x] texture importer
  - [x] .png importer
- [x] .hdr importer
  - [ ] multi-texture importer
- [x] Event listener
    - [x] Advanced (globle event)
    - [ ] Advanced (...)
- [x] Timer (struct)