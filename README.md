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
- unform arrays  & tripple buffering
- (working on) loading .tga, simple Material (diffuse texture)

## roadmap
- texturing, materials
	batch_size = min(
	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	GL_MAX_VERTEX_UNIFORM_COMPONENTS / unifrom_struct_component_count);
- cube/ball/buddha meshes
- timestep
- Static_vertex_spec. I need several vaos to perform different passes. Each pass takes only subset of all vertex attribs defined in Static_vertex_spec.

- deferred lighting (opengl/direct 3d)
	- hemispheric ambient light 
	- directional light
	- point light
	- spotlight
	- any light
- on window resize
- Shadow pass (can I run it once: computing a shadow factor and apply in to lighting computations?)
	- shadow mapping
	- ambient occlusion
- normal mapping
	- (Geometry shader)Vertex format. Get rid of tanget_h. Calc tangent space in the geometry shader.
- uniform arrays -> uniform block (because the number of shader uniform array components is limited)
- post processing
	- bloom (bockeh)
	- good rays
- mouse, keyboard
	- fullscreen (F11), max/min window
	- camera controls
- compute shaders: indirect buffer, draw call index buffer, uniform buffer object, ...
- crytek sponza 
