# deferred lighting renderer
camera rotation...
scene description

## screenshots

## renderer outline
Deferred lighting is devided into three main passes.

1. GBuffer_pass.

	fills rgba32f texture. 
	xy: contain encoded normal (view space). 
	z: depth (view space).
	w: material's smoothness property.

2. Lighting_pass. 

	fills rgb32f ambient term, diffuse term, specular term textures.
	if (smoothness <= 0.01) discard;

3. Ambient_occlusion_pass.

4. Shadow_map_pass.

	Variance Shadow Mapping.
	Gaussian filter.
	fills rg32f components of the shadow occlusion map texture with depth and squared depth. Depth values are in the directionnal light's space.
	Red & green components are always positive in spite of depth values are negative in the light's space.
 
5. Material_pass.

	The final pass that combines lighting pass's & shadow pass's results with material properties of every object on the scene.
	fills rgb32f material lighting texture.

6. Tone mapping

Deferred_lighting renderer uses persistent mapped buffer technique for multi-indirect rendering. Draw indirect buffer use tripple buffering technique to metigate synchronization with OpenGL command queue. Draw call index buffer is used for and gl_DrawID simulation. It is a static buffer object that contains ordered sequence of integers [0, batch_size).

## Implementation notes and further improvements
- Single vertex array object. 
- Frame_packet, 2-3 Frame objects.
- Uniform arrays are used heavily, uniform block would have done better. Unifrom array limitations.
- Gbuffer render targets as 2D texture array.
- Point lights, spotlights instanced rendering.
- Batch_size deduction.
- light house model note
- Encode/decode normal
	- Material alpha mask (opaque, transparent)


## roadmap
- Format uniform in all the shaders.
- get_batch_size (check all the shaders and choose one)


- restore 60 fps frame rate. (ssas_pass)
