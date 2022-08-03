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
- [x] unique ID generator
- [x] object name generator
- [ ] select operation

- [x] Mesh
  - [x] smooth normal
  - [ ] dynamic mesh
- [x] Spirit
  - [ ] batch rendering
- [x] Camera
  - [x] frustum matrix
  - [ ] Debug outline
  - [x] Camera transform
  - [ ] dynamic parameters
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
  - [ ] Stencil-buffer
  - [x] Fimic convert
  - [ ] Gamma convert
  - [ ] outline shader
  - [ ] FXAA
- [ ] multi-pass composition
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
- **Shader**
  - [ ] dynamic compiling
    - [ ] node system
  - [ ] shader storage buffer
- **Advance**
- mode switch
  - visible switch
  - dynamic exposure  

#### **UI**
- [x] Basic debug
- [ ] Docking (1/2)
- [x] Multi-viewports
- [x] UI manager
- [ ] Imgui parameters input
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
- [ ] .hdr importer
  - [ ] multi-texture importer
- [x] Event listener
- [x] Timer (struct)