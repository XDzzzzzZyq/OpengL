# OpenGL_Test

##### Opengl renderer development from XDzZyq

# RECORD

https://space.bilibili.com/302469604/channel/collectiondetail?sid=572334

# TUTORIALs 

https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-

# FEATUREs

#### **Objects**

- [x] transform matrix
- [x] invert transform matrix
- [x] unique ID generator
- [x] object name generator
- [ ] select operation
- [ ] transform handle
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
- [ ] Debug Points
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
  - [ ] outLine shader
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