# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq

### Build

```
git submodule init
git submodule update
```

#### Debian-Based GNU/Linux Distros

```
sudo apt install libglew-dev libgl-dev
cmake .
make
```

#### Windows

Install Visual Studio 2022.

Download [GLEW binaries](https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download). Add the following to your `PATH`:

```
[path-to-glew]\glew-2.1.0\bin
[path-to-glew]\glew-2.1.0\bin\Release\Win32
```

Open Developer Command Prompt for VS 2022 and `cd` into the project directory.

```
cmake .
msbuild
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
