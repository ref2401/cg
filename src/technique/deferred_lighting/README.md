# deferred lighting renderer

## screenshots

## renderer outline
Deferred lighting is devided into three main passes.

1. GBuffer_pass.

	fills rgba32f texture. xyz: normal in the view space. w: material's smoothness property.

2. Shadow_map_pass.

	Variance Shadow Mapping
	fills rg32f components of the shadow map texture with depth and squared depth. Depth values are in the directionnal light's space.
	Red & green components are always positive in spite of depth values are negative in the light's space.

3. Lighting_pass.

	fills rgb32f ambient term, diffuse term, specular term textures.
 
4. Material_pass.

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
- Encode/decode normal
	- Material alpha mask (opaque, transparent)


## roadmap
- teapot model

- Material pass
-  gamma correction sRGB formats?
	- fix gamma, bring colors to the linear space pow(2.2)
	- before retunring the color return it 0.45 gamma space

- Shadow pass (can I run it once: computing a shadow factor and apply in to lighting computations?)
	- pack shadow factor & ambient occlusion factor in one texture.
	- shadow mapping
	- ambient occlusion

- get_batch_size (check all the shaders and choose one)

- camera coration. Horz mouse offset always rotates around float3::unit_y; 

- post processing
	- tonemapping, wtite to sRGB texture
		- glEnable() with the GL_FRAMEBUFFER_SRGB
