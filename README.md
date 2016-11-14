# cg
Implementation of various CG algorithms/techniques using OpenGL 4.5 & DirectX 11.

## Build
Visual Studio Community 2015 is used. The solution is tested only under x86 (Win32) platform.

## Techniques
1. [Deferred Lighting Renderer](https://github.com/ref2401/cg/wiki/Deferred-Lighting-Renderer) (OpenGL 4.5)
2. [Learn DirectX 11](https://github.com/ref2401/cg/wiki/Learn-DirectX-11) (DirectX 11)

## Notes
1. [Want to Implement List](https://github.com/ref2401/cg/wiki#wanna-implement)
2. [Todo List](https://github.com/ref2401/cg/wiki#todo)

Due to refactoring `cg/data/mesh` and `cg/file/file_mesh` contain both old and new implementations of the `interleaved mesh data` concept and wavefront(.obj) file loading functions. Classes and funcs with suffix `_old` will be in the code base for a while for the sake of backward compatibility. 

`cg/rnd/opengl` module will be refactored after the [learn direct x 11](https://github.com/ref2401/cg/wiki/Learn-DirectX-11) project is done.
