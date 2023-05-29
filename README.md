# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq

### Build

```bash
$ git submodule init
$ git submodule update
```

#### Debian-Based GNU/Linux Distros

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

Download [GLEW binaries](https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download). Add the following to your `PATH`:

```
[path-to-glew]\glew-2.1.0\bin
[path-to-glew]\glew-2.1.0\bin\Release\Win32
```

Open Developer Command Prompt for VS 2022 and `cd` into the project directory.

```bash
$ cmake .
$ msbuild
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
