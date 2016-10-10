# cg
Implementation of various CG algorithms/techniques using OpenGL 4.5.
Later I would like to do the same using DirectX 11

# Techniques
1. [deferred lighting renderer](/src/technique/deferred_lighting/)

# Wanna implement
1. Fibers base engine (simulation, visibility check & renderer params submition
2. lighting
2.1 physically based lighting
	Walter GGX https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
	https://readyatdawn.sharefile.com/share?#/view/s9dc3fcc9c7b4a118
2.2. image-based lighting
2.3. spherical harmonics
3. anisotropic brdf
4. point sprites
5. wavefront loading
5.1. shared vertices
5.2. several models (objects) in a single .obj file

# TODO
1. Static_vertex_spec owns 2 Static_buffers
2. Static_vertex_spec default ctor
3. profile: memory, perf (VS profiler, anything else)
4. try to move Win_app::run iplemtation into Application. You will need _mouse, _window, _rnd_ctx, pump_sys_messages
5. Texture_2d, Texture_2d_immut take Sampler_config as ctor argument.

