# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq

### Build

#### Debian-Based GNU/Linux Distros

```
sudo apt install libglew-dev libglfw3-dev libgl-dev libglm-dev
cmake .
make
```

### Features
 - [x] multi-passes buffering
	- [ ] texture -> renderbuffer?
 - [x] Multi-passes composition
	- [ ] PBR deferred rendering
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

 - [ ] Dynamic passes binding
 - [ ] Dynamic shader value<->texture altering
