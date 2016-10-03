# deferred lighting steps
Deferred lighting is devided into three passes:
1. GBuffer_pass
input:
	vertex_attribs: position, normal, tex_coord, tangent_h
	model_matrix
	normal_matrix (normal, tangent, bitangent)
	tex_normal_map_id (sRGB?)
	tex_smoothness_id (sRGB?)

- detach shader

2. Lighting_pass
	- depth_unchanged gl_FragDepth
3. Material_pass

Deferred_lighting renderer uses persistent mapped buffer technique for multi-indirect rendering and gl_DrawID simulation. 
Indirect buffer and draw index buffer use tripple buffering technique to metigate synchronization with OpenGL command queue.



## roadmap

- timestep
- cube/ball/

- Material pass
-  gamma correction sRGB formats?
	- fix gamma, bring colors to the linear space pow(2.2)
	- before retunring the color return it 0.45 gamma space
	
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
	- tonemapping, wtite to sRGB texture
		- glEnable() with the GL_FRAMEBUFFER_SRGB
- mouse, keyboard
	- fullscreen (F11), max/min window
	- camera controls
- compute shaders: indirect buffer, draw call index buffer, uniform buffer object, ...
- crytek sponza 

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