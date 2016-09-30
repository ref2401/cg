# deferred lighting steps
## done
- IGame impl, clear screen
- Single vertex/fragment shader -> one pixel. SetPixelSize(px)
- Static_vertex_spec_builder, DE_cmd, DE_base_vertex_params
- Static_vertex_spec, DE_indirect_params, sync, MultiDrawIndirect
- draw index simulation
- unform arrays  & tripple buffering
- loading .tga, simple Material (diffuse texture)
- each renderable has it's own diffuse texture if
- draw batch size = texture unit count / material texture count
- (working on) Deferred lighting first pass
	- Deferred lighting renderer outline (Passes, vaos, ...)
	- Static_vertex_spec. I need several vaos to perform different passes. Each pass takes only subset of all vertex attribs defined in Static_vertex_spec.

## roadmap

- timestep
- cube/ball/buddha meshes
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
	- screen space reflection
- mouse, keyboard
	- fullscreen (F11), max/min window
	- camera controls
- compute shaders: indirect buffer, draw call index buffer, uniform buffer object, ...
- crytek sponza 
