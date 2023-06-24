# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq



### Features

 - [x] multi-passes buffering
 - [x] Multi-passes processing
	- [x] FXAA
	- [x] SSAO
	- [x] Simple Convolution
 - [x] Multi-passes composition
	- [x] PBR deferred rendering
		- [x] PBR direct Lighting
			- [x] Point light
			- [x] Sun light [@theta-lin](https://github.com/theta-lin)
			- [x] Spot light [@theta-lin](https://github.com/theta-lin)
			- [x] Area light (currently only support pure color convex polygon) [@theta-lin](https://github.com/theta-lin)

			- [x] Shadow Map
		- [x] PBR IBL

- [x] advanced StorageBuffer
- [x] advanced Texture storage
	- [x] Cube map <- Equirectangular map
		- [x] ComputeShader with Cube map
	- [x] Equirectangular map <- Cube map

 - [ ] editing elements
	- [x] outline

 - [x] Post Processing
	- [ ] Processing Chain
	- [ ] Fragment Shader <-> Compute Shader

### UI
 - [ ] Post Processing Flow editor
	- [ ] Node editing
	- [ ] Shader editing

### Advanced

 - [x] Dynamic passes binding
 - [ ] Dynamic shader value<->texture altering
