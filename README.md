# Multiple Passes Rendering branch

##### Deferred rendering development from XDzZyq

### Build

#### Debian-Based GNU/Linux Distros

```bash
$ sudo apt install libglew-dev libglfw3-dev libgl-dev libglm-dev
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
