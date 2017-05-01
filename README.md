Implementation of various CG algorithms/techniques using OpenGL 4.5 & DirectX 11.

## Clone, init & build (x86 only)
Required tools
- DirectX SDK (install it before Visual Studio)
- Vulkan SDK
- Visual Studio Community 2017
- CMake
- Git
- Make sure that cmake, git & devenv commands are available from the command line.

Clone the repository as usual and then execute `init.cmd` script. The script creates all the required folders, uploads submodules and builds all the dependencies. After the script has successfully completed you can open the project in VS2015 ```src/cg.sln```. Don't forget to switch configuration to x86.

## Techniques (new ones go first)
1. [Parallax Occlusion Mapping](https://github.com/ref2401/cg/wiki/Parallax-Occlusion-Mapping) (DirectX 11)
2. [Fur Simulation](https://github.com/ref2401/cg/wiki/Fur-Simulation) (OpenGL 4.5)
3. [Learn DirectX 11](https://github.com/ref2401/cg/wiki/Learn-DirectX-11) (DirectX 11)
4. [Deferred Lighting Renderer](https://github.com/ref2401/cg/wiki/Deferred-Lighting-Renderer) (OpenGL 4.5)

## Meshes
- http://graphics.cs.williams.edu/data/meshes.xml
- http://www.cc.gatech.edu/projects/large_models/
- http://www.katsbits.com/download/models/

## Notes
1. [Want to Implement List](https://github.com/ref2401/cg/wiki#wanna-implement)
2. [Todo List](https://github.com/ref2401/cg/wiki#todo) 
