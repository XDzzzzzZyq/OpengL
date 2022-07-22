# OpenGL_Test

Opengl renderer development from XDzZyq

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
- [ ] select operation
- [ ] transform handle

- Mesh
  - [x] AVAILABLE
  - [x] smooth normal
  - [ ] dynamic mesh

- Spirit
  - [x] AVAILABLE
  - [ ] batch rendering

- Camera
  - [x] frustum matrix
  - [x] AVAILABLE
  - [ ] Debug outline
  - [x] Camera transform
  - [ ] dynamic parameters

- Debug Line
  - [x] AVAILABLE
- Debug Mesh
  - [ ] AVAILABLE

- Image Plane
  - [ ] AVAILABLE
- Light
  - [x] point light
  - [ ] sun light
  - [ ] spot light
- Environment
  - [ ] AVAILABLE

#### **Render**

- **Algorithms**

  - [x] Z-buffer
  - [x] Fimic convert

  - Blinn-Phong

    - [x] diffuse
    - [x] specular
    - [x] environment
    - [ ] HDR environment
    - [ ] AO
    - [ ] shadow

  - PBR

    - [ ] material system

    - [ ] pbr diffuse
    - [ ] pbr specular

  - Ray tracing ?

- **Advance**

  - mode switch
  - visible switch
  - dynamic exposure  

#### **UI**

- [ ] Imgui parameters input
- [ ] files management

#### **Other**

- [x] objects pool (renderer class)
  - [x] pool rendering
    - [ ] advance speed up

- [x] .obj importer
  - [ ] advance speed up
- [ ] .png importer
  - [ ] texture & material importer

- [x] Event listener
- [x] Timer (struct)