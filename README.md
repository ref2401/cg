# cg
Implementation of various CG algorithms/techniques using OpenGL 4.5.
Later I would like to do the same using DirectX 11


# deferred lighing steps
## done
- IGame impl, clear screen
- Single vertex/fragment shader -> one pixel. SetPixelSize(px)
- Static_vertex_spec_builder, DE_cmd, DE_base_vertex_params
- Static_vertex_spec, DE_indirect_params, sync, MultiDrawIndirect
- draw index simulation
-> unform arrays  & tripple buffering

## roadmap
- uniform array
- tripple buffering
- cube/ball/buddha meshes
- on window resize
- timestep
- texturing
- Static_vertex_spec. I need several vaos to perform different passes. Each pass takes only subset of all vertex attribs defined in Static_vertex_spec.

- deferred lighting (opengl/direct 3d)
	- hemispheric ambient light 
	- directional light
	- point light
	- spotlight
	- any light
- shadow mapping
- ambient occlusion
- normal mapping
	- (Geometry shader)Vertex format. Get rid of tanget_h. Calc tangent space in the geometry shader.
- post processing
	- bloom (bockeh)
	- good rays
- mouse, keyboard
	- fullscreen (F11), max/min window
	- camera controls
- uniform arrays -> uniform block (because the number of shader uniform array components is limited)
- compute shaders: indirect buffer, draw call index buffer, uniform buffer object, ...
- crytek sponza 
