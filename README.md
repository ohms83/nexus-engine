# Nexus Engine
A (supposedly) cross platform game engine based on Vulkan and OpenGL. This project created as a personal side hobby for improving and refreshing my programming skills. There's no planned feature list nor development timeline. It's done when it's done (and I don't think it's gonna will).
# Development Environment
This project is developped and tested based on the following configurations.
## Hardwares
- Lenovo Ideapad Y50-70.
- Intel® Core™ i7-4710HQ CPU @ 2.50GHz × 8
- NVIDIA GeForce GTX 860M
- RAM 16GB. 
## OSes
- Zorin OS 16 (Ubuntu 20.04 LTS) using apt package manager.
## Tools
- Visual Studio Codes + CMake Tools
- CMake
# Dependencies
The following is a list of dependencies used by this project.
- SDL 2.0
- OpenGL 4.6
- GLM
# Installation
## Linux
### Compiler
Update package database:
```
sudo apt update
```
Install the GNU compiler tools and the GDB debugger:
```
sudo apt-get install build-essential gdb
```
### OpenGL
Most Linux distributions rely on Mesa3D's OpenGL implementation. Run the following command to install the SDKs:
```
sudo apt install libglu1-mesa-dev mesa-common-dev libglew-dev
```
### SDL
SDL usually shipped together with most Linux distros. In case your distros doesn't include it or you want to update, run the following commands:
```
sudo apt install libsdl2-dev libsdl2-2.0-0
```
### Mesa Utils
Mesa Utils is a useful tool for retrieving OpenGL's driver implementation of your machine:
```
sudo apt install mesa-utils
```