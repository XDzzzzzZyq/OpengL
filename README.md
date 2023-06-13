# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq

### Build

Execute the following to update the git submodules:
```bash
$ git submodule init
$ git submodule update
```

#### Debian-Based GNU/Linux Distros

Install the external dependencies:
```bash
$ sudo apt install libglew-dev libgl-dev
$ cmake .
$ make
```
##### Unable to locate package xxx-dev

```bash
$ sudo apt update
```

##### unrecognized command line option ‘-std=c++20’

```bash
$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test
$ sudo apt update
$ sudo apt-get install gcc-11 g++-11

$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 20 --slave /usr/bin/g++ g++ /usr/bin/g++-11
$ sudo update-alternatives --config gcc
```

#### Windows

Install Visual Studio 2022.

Download [GLEW binaries](https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download) and add the following to your `PATH`:
```
[path-to-glew]\glew-2.1.0\include
```

Then add the following to `PATH` if building for 32-bit:
```
[path-to-glew]\glew-2.1.0\bin\Release\Win32
```

Or the following if building for 64-bit
```
[path-to-glew]\glew-2.1.0\bin\Release\x64
```

Open Developer Command Prompt for VS 2022, `cd` into the project directory, and execute the following command:
```bash
$ cmake .
$ msbuild
```

Or, you can also open `CMakeLists.txt` as a CMake project in Visual Studio 2022 with CMake integration installed.

### Features

 - [x] multi-passes buffering
 - [x] Multi-passes processing
	- [x] FXAA
	- [x] SSAO
	- [x] Simple Convolution
 - [x] Multi-passes composition
	- [ ] PBR deferred rendering
		- [ ] PBR direct Lighting
			- [x] Point light
			- [x] Sun light
			- [x] Spot light
			- [x] Area light (currently only support pure color convex polygon)
		- [x] PBR IBL

- [x] advanced StorageBuffer
	
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
