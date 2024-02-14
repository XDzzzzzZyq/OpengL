# OpenGL_Test

##### OpenGL based renderer development by XDzZyq



# RECORDs

[合集·__Test_OpenGL__](https://space.bilibili.com/302469604/channel/collectiondetail?sid=572334)



# TUTORIALs 

[OpenGL - YouTube](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
[OpenGL Tutorials - YouTube](https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-)

[Learn OpenGL, extensive tutorial resource for learning Modern OpenGL](https://learnopengl.com/)

[OpenGL API Documentation](https://docs.gl/)



# FEATUREs

### **Objects**

- [x] basic transformation
  - [x] parenting
  - [x] transform handles
- [x] unique ID generator
- [x] select operation
- [x] Mesh
  - [x] smooth normal
  - [ ] mesh editing
  - [x] Material
- [x] Spirit
  - [ ] batch rendering
- [x] Camera
  - [x] frustum matrix
  - [ ] Debug outline
  - [x] Camera transform
  - [x] adjustable parameters
- [x] Debug Line
- [ ] Debug Mesh
- [x] Debug Points
  - [x] multi-points push
  - [x] dynamic color
- [ ] Image Plane
- [x] Light
  - [x] point light
  - [x] sun light
  - [x] spot light
  - [x] area light
- [x] Environment
  - [x] IBL
  - [ ] z-Fog
  - [ ] atmosphere shading
- [x] Post Processing
  - [x] Screen Space PPS
  - [ ] Field PPS
  - [x] Compute Shader supports
- [x] Signed Distance Field   
  - [ ] outline
  - [ ] transform

### **Rendering**

  - [x] Z-buffer
  - [x] Dynamic frame buffer
  - [ ] Stencil-buffer
  - [x] Filmic convert
  - [x] Gamma convert
  - [x] Selection highlight
 

#### **Anti Aliasing**

  - [ ] MSAA
  - [x] FXAA
  - [ ] TAA

#### **Ambient Occlusion**
  - [x] SSAO
  - [ ] GTAO
  - [ ] HBAO

  - [ ] Denoising

#### **PBR**
  - [x] Material system
  - [x] PBR Lighting
    - [x] point light
    - [x] sun light
    - [x] spot light
    - [x] area light
  - [x] PBR IBL
    - [x] diffuse
    - [x] specular
       
  - [ ] Normal map
  - [ ] Bump map
       
#### **Shadow**      
  - [x] Shadow
    - [x] shadow mapping
    - [x] soft shadow
      - [x] SDF soft shadow
        - [ ] improvement
      - [ ] Variance Soft Shadowmapping
      - [ ] Momenet Soft Shadowmapping

#### **Ray Tracing**
  - [x] Screen Space Reflections
  - [x] SDF Screen Space Reflection
  - [x] Stochastic Screen Space Reflection

  - [ ] Path Tracing

- DL style transfer ?



### Shaders

  - Vertex Shader & Fragment Shader

  - Compute Shader

  - Geometry Shader
  
- [x] Multi-pass composition (deferred rendering)

  **for Vertex Shader & Fragment Shader**

  - [x] GLSL interpreter (structure analysis)
    - [x] Structure Parser
    - [ ] Build-in Lib
        - [x] Material params
    - [ ] Advanced analysis

  - [x] Buffering

    - [x] Storage Buffer
    - [x] Uniform Buffer

  - [x] dynamic compiling

  - [x] dynamic uniform

    - [ ] node system

  **for Compute Shader**

  - [x] Texture computation
    - [x] Equirtangular Map ←→ Cube Map
  - [x] Rendering Pipeline
    - [x] SSAO
    - [x] FXAA
    - [x] Shadow mapping
  - [ ] Shader Editor supports

  **for Geometry Shader**

  - [x] SDF construct

## **Advanced**

  - render mode switch
    - [x] Averaging mode 
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
    - [x] Outliner Layer
    - [x] Shader Editor Layer
        - [x] Structure Editor
        - [x] Code Editor
        - [ ] Node Editor
    - [x] Transform(Property) Layer
    - [x] Material Layer
- [x] ImGui parameters flow

- [x] Gizmos

- [ ] files management
- [ ] node system

#### **Other**

- [x] objects pool (Scene Manager)
  - [x] pool rendering
    - [ ] filter
    - [ ] acceleration structure
- [x] model importer
  - [x] build-in mesh data
  - [x] .obj importer
  - [ ] advance speed up
- [x] texture importer
  - [x] .png importer
  - [x] .hdr importer
  - [ ] multi-texture importer
- [x] Event listener
    - [x] Advanced (global event)
    - [x] Advanced (global randoms)
    - [ ] Advanced (...)
- [x] Timer (struct)

- [ ] CUDA acceleration
