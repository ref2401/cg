Implementation of various CG algorithms/techniques using OpenGL 4.5 & DirectX 11.

## Clone, init & build
Required tools
- Visual Studio Community 2015
- CMake

Clone the repository as usual and then execute init.cmd script. The script creates all the required folders, uploads submodules and builds all the dependencies. After the script has successfully completed you can open the project in VS2015 ```src/cg.sln```.

Only Debug mode works at the moment. The reason is that extern/Assimp does not build under Release mode. Also the projects is tested only under x86 (Win32) platform.

## Techniques (new ones go first)
1. [Learn DirectX 11](https://github.com/ref2401/cg/wiki/Learn-DirectX-11) (DirectX 11)
2. [Deferred Lighting Renderer](https://github.com/ref2401/cg/wiki/Deferred-Lighting-Renderer) (OpenGL 4.5)

## Meshes
- http://graphics.cs.williams.edu/data/meshes.xml
- http://www.cc.gatech.edu/projects/large_models/
- http://www.katsbits.com/download/models/

## Notes
1. [Want to Implement List](https://github.com/ref2401/cg/wiki#wanna-implement)
2. [Todo List](https://github.com/ref2401/cg/wiki#todo)

Due to refactoring `cg/data/mesh` and `cg/file/file_mesh` contain both old and new implementations of the `interleaved mesh data` concept and wavefront(.obj) file loading functions. Classes and funcs with suffix `_old` will be in the code base for a while for the sake of backward compatibility. 

After the [learn direct x 11](https://github.com/ref2401/cg/wiki/Learn-DirectX-11) project is done:
- `cg/rnd/opengl` module will be refactored.
- `Shader_program_source_code` & `load_shader_program_source` will be deleted from the code base.
