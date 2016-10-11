# deferred lighting renderer

## screenshots

## renderer outline
Deferred lighting is devided into three main passes.

1. GBuffer_pass.

	fills rgba32f texture. xyz: normal in the view space. w: material's smoothness property.

2. Lighting_pass.

	fills rgb32f ambient term, diffuse term, specular term textures.
 
3. Material_pass.

	The final pass that combines lighting pass's & shadow pass's results with material properties of every object on the scene.
	fills rgb32f material lighting texture.

Deferred_lighting renderer uses persistent mapped buffer technique for multi-indirect rendering. Draw indirect buffer use tripple buffering technique to metigate synchronization with OpenGL command queue. Draw call index buffer is used for and gl_DrawID simulation. It is a static buffer object that contains ordered sequence of integers [0, batch_size).

## Implementation notes and further improvements
- Single vertex array object. 
- Frame_packet, 2-3 Frame objects.
- Uniform arrays are used heavily, uniform block would have done better. Unifrom array limitations.
- Point lights, spotlights instanced rendering.
- Batch_size deduction.
- light house model note


## roadmap
- teapot model
- scene floor, normal mapping applied. sort of a chess.

- Material pass
-  gamma correction sRGB formats?
	- fix gamma, bring colors to the linear space pow(2.2)
	- before retunring the color return it 0.45 gamma space

- Shadow pass (can I run it once: computing a shadow factor and apply in to lighting computations?)
	- pack shadow factor & ambient occlusion factor in one texture.
	- shadow mapping
	- ambient occlusion

- get_batch_size (check all the shaders and choose one)

- post processing
	- bloom (bockeh)
	- good rays
	- screen space reflection
	- tonemapping, wtite to sRGB texture
		- glEnable() with the GL_FRAMEBUFFER_SRGB

- os messages(events)
	- mouse wheel
	- keyboard
	- fullscreen (F11), max/min window

- compute shaders: indirect buffer, draw call index buffer, uniform buffer object, ...
