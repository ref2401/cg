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
5.1. compute_tangent_h. different normals. if normal0 = normal1 then tangetn_h0 = tangent_h1
5.1. shared vertices
5.2. several models (objects) in a single .obj file
5.3. crytek sponza 
5.4. light house
6. light scattering (GPU Pro 5)
7. Screen space reflection

# TODO
1. Static_vertex_spec owns 2 Static_buffers
2. Static_vertex_spec default ctor
3. profile: memory, perf (VS profiler, anything else)
4. try to move Win_app::run iplemtation into Application. You will need _mouse, _window, _rnd_ctx, pump_sys_messages
5. Sampler_config holds opengl value. Rename it to Sampler_params. Sampler_config should own {Min/Max}_filter, Wrap_mode values.
6. Tangent space debugging. Draw tangent space for each vertex. Also draw the final normal exctracted from normal-map
7. By_line_iterator unittest: start reading file with it0, move the iterator to it1 and finish reading the file.
8. os messages(events)
	- Fix mouse camera rotation. It's too sloppy.
	- mouse wheel
	- keyboard
	- fullscreen (F11), max/min window
9. cg/rnd/utility/filter.h
	- Compute kernel and pass it to Filter_shader_program. There shoudl be only one shader that takes kernel as uniform array.
	- Gaussian kernel. Sharp/mid/soft peak
