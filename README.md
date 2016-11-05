# cg
Implementation of various CG algorithms/techniques using OpenGL 4.5.
Later I would like to do the same using DirectX 11

## Build
Visual Studio Community 2015 is used. The solution is tested only under x86 (Win32) platform.

# Techniques
1. [deferred lighting renderer](/src/technique/deferred_lighting/)

# Wanna implement
1. lighting.
	- physically based lighting
	Walter GGX https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
	https://readyatdawn.sharefile.com/share?#/view/s9dc3fcc9c7b4a118
	- image-based lighting
	- spherical harmonics
	- Volumetric Lighting
	- global illumination lighting solutions. 3D textures.
	- the PBR book (Oren Nauar, Torrence, Anisotropic). mitsuba-renderer.
2. Shadows
	- shadow volume. geometry shader outputs silhouette. rasterizer writes to stencil buffer.
3. light scattering (GPU Pro 5)
4. Reflection
	- Screen space reflection
	- Environment mapping (buddha)
5. SSAO 
	- Horizon-Base Ambient Occlusion +
	- triangle sample kernel in textur space. orient it in the same hemisphere with a texel's normal.
6. AA
	- FXAA, MSAA, MLAA, SMAA
7. Cubemap
8. Terrain
9. Fibers base engine (simulation, visibility check & renderer params submition)
10. point sprites
	- particle system. Depth test also writes particle sprites into stream output.
	Later, material pass uses stream output buffer as it's input, to render particles.
11. wavefront loading
	- compute_tangent_h. different normals. if normal0 = normal1 then tangetn_h0 = tangent_h1
	- shared vertices
	- several models (objects) in a single .obj file
	- crytek sponza 
	- light house

# TODO
0. move stuff to the github wiki.
0. main loop. pump_sys_messages is an update routine as well. move into the update loop.
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
10. math
	- Plane <n, d>
	- Rect_3d
	- Frustum owns planes. view_space_{near/far/left/right}_plane(), view_space_{near/far/left/right}_rect_3d()
	- float{2/3/4} to float[2/3/4] array
11. Data_bundle_loader. feed it with filenames and call load. Data_bundle_loader will use several threads to load all the data in parallel.
	Returns future with Data_bundle.
	For each requested data element the bundle has the element or an exception.
	Each thread pulls the next filename when it's done with its current job.
12. Deferred_lighting_renderer viewpoint rotation is broken if viewpoint's target is not float3::zero. 
	Possible fix. Oy rotation axis must take target's position into account.
13. delete copy/move assignment operator where needed.
14. Mark struct/class final if it's not meant to be inherited. e.g. float{2/3/4}
15. set_uniform overload funcs instead of function template specs. (set_uniform_array too if it's possible)
16. deferred_lighting_renderer specular term is visible at the opposite side of the teapot
