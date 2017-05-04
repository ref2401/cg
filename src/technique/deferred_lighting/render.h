#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_

#include <array>
#include <limits>
#include <vector>
#include "cg/base/base.h"
#include "cg/base/math.h"
#include "cg/data/image.h"
#include "cg/data/shader.h"
#include "cg/rnd/opengl/opengl.h"
#include "technique/deferred_lighting/filter_shader.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render_pass_shader.h"


namespace deferred_lighting {

class Gbuffer final {
public:

	Gbuffer(const uint2& viewport_size,
		const Vertex_attrib_layout& vertex_attrib_layout,
		const cg::data::Model_geometry_data<cg::data::vertex_attribs::p_tc>& rect_1x1_mesh_data);

	Gbuffer(const Gbuffer& gbuffer) = delete;

	Gbuffer(Gbuffer&& gbuffer) = delete;

	~Gbuffer() noexcept = default;


	// Auxiliary depth32f buffer that can be used in an rendering pass.
	// The renderbuffer is used in shadow_map_pass for depth test.
	cg::rnd::opengl::Renderbuffer& aux_depth_renderbuffer() noexcept
	{
		return _aux_depth_renderbuffer;
	}

	// Rect 1x1 (a square) is usually used to perform a full screen pass.
	const DE_base_vertex_params& aux_geometry_rect_1x1_params() const noexcept
	{
		return _aux_geometry_rect_1x1_params;
	}

	// Returns the vertex array object's id that must be bound before 
	// auxilary geometry (rect_1x1, ...other...) is renderer.
	GLuint aux_geometry_vao_id() const noexcept
	{
		return _aux_geometry_vertex_spec.vao_id();
	}
	
	// Default bilinear sampler (clamp_to_edge).
	const cg::rnd::opengl::Sampler& bilinear_sampler() const noexcept
	{
		return _bilinear_sampler;
	}

	// Default nearest sampler (clamp_to_edge).
	const cg::rnd::opengl::Sampler& nearest_sampler() const noexcept
	{
		return _nearest_sampler;
	}

	// Resizes all the render target textures.
	void resize(const uint2& viewport_size) noexcept;

	// Auxiliary texture that can be used by any render pass.
	// The texture is not supposed to transfer rendering results from pass to pass.
	// At the begining of every pass assume that texture's contents is undefined.
	cg::rnd::opengl::Texture_2d& tex_aux_render_target() noexcept
	{
		return _tex_aux_render_target;
	}

	// Lighting_pass's render target texture.
	// xyz components contain ambient radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_ambient_term() noexcept
	{
		return _tex_lighting_ambient_term;
	}

	// Lighting_pass's render target texture.
	// xyz components contain diffuse radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_diffuse_term() noexcept
	{
		return _tex_lighting_diffuse_term;
	}

	// Lighting_pass's render target texture.
	// xyz components contain specular radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_specular_term() noexcept
	{
		return _tex_lighting_specular_term;
	}

	// Material_lighting_pass's render target texture.
	// xyz components contain fully lit material color.
	// The computations are made in HDR.
	cg::rnd::opengl::Texture_2d& tex_material_lighting_result() noexcept
	{
		return _tex_material_lighting_result;
	}

	// Gbuffer_pass's render target texture.
	// xyz components contain normal in the view space.
	// w component contains material.smoothness parameter.
	cg::rnd::opengl::Texture_2d& tex_nds() noexcept
	{
		return _tex_nds;
	}

	// tex_shadow_occlusion_map is render target for Shadow_map_pass & Ambient_occlusion_pass
	// xy components contain linear depth and squared depth values in the directional light's space.
	// xy components are always positive in spite of depth values are negative in the light's space.
	// z component contains ambient occlusion factor.
	cg::rnd::opengl::Texture_2d& tex_shadow_map() noexcept
	{
		return _tex_shadow_map;
	}

	cg::rnd::opengl::Texture_2d& tex_ssao_map() noexcept
	{
		return _tex_ssao_map;
	}

	cg::rnd::opengl::Texture_2d& tex_ssao_map_aux() noexcept
	{
		return _tex_ssao_map_aux;
	}

	const Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _vertex_attrib_layout;
	}

	// The current size of all the render target textures.
	const uint2& viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:
	const Vertex_attrib_layout _vertex_attrib_layout;
	cg::rnd::opengl::Sampler _bilinear_sampler;
	cg::rnd::opengl::Sampler _nearest_sampler;
	Static_vertex_spec _aux_geometry_vertex_spec;
	DE_base_vertex_params _aux_geometry_rect_1x1_params;
	uint2 _viewport_size;
	cg::rnd::opengl::Renderbuffer _aux_depth_renderbuffer;
	cg::rnd::opengl::Texture_2d _tex_aux_render_target;
	cg::rnd::opengl::Texture_2d _tex_lighting_ambient_term;
	cg::rnd::opengl::Texture_2d _tex_lighting_diffuse_term;
	cg::rnd::opengl::Texture_2d _tex_lighting_specular_term;
	cg::rnd::opengl::Texture_2d _tex_material_lighting_result;
	cg::rnd::opengl::Texture_2d _tex_nds;
	cg::rnd::opengl::Texture_2d _tex_shadow_map;
	cg::rnd::opengl::Texture_2d _tex_ssao_map;
	cg::rnd::opengl::Texture_2d _tex_ssao_map_aux; //  the texture is used to filter ssao map
};

class Gbuffer_pass final {
public:

	Gbuffer_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code);

	Gbuffer_pass(const Gbuffer_pass& pass) = delete;

	Gbuffer_pass(Gbuffer_pass&& pass) = delete;

	~Gbuffer_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const float4x4& projection_matrix, const float4x4& view_matrix) noexcept;

	// Clears renderer states after rendring using this pass.
	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count, 
		const std::vector<float>& uniform_array_model_matrix,
		const std::vector<float>& uniform_array_smoothness,
		const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept;

private:
	const float _clear_value_nds[4] = { 0, 0, std::numeric_limits<float>::lowest(), 0 };
	const float _clear_value_depth_map = 1.0f;
	cg::rnd::opengl::Framebuffer _fbo;
	Gbuffer_pass_shader_program _prog;
	Gbuffer& _gbuffer;
};

class Lighting_pass final {
public:

	Lighting_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& dir_source_code);

	Lighting_pass(const Lighting_pass&) = delete;

	Lighting_pass(Lighting_pass&& pass) = delete;

	~Lighting_pass() noexcept = default;


	void begin() noexcept;

	void end() noexcept;

	void perform_directional_light_pass(const std::array<float3, 4>& far_plane_coords, 
		const Directional_light_params& dir_light) noexcept;

private:
	const float4 _clear_value_color = float4::zero;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Lighting_pass_dir_shader_program _dir_prog;
};

class Material_lighting_pass final {
public:

	Material_lighting_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code);

	Material_lighting_pass(const Material_lighting_pass&) = delete;

	Material_lighting_pass(Material_lighting_pass&& pass) = delete;

	~Material_lighting_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const float4x4& projection_matrix, const float4x4& view_matrix,
		const Directional_light_params& dir_light) noexcept;

	// Clears renderer states after rendring using this pass.
	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
		const std::vector<float>& uniform_array_model_matrix,
		const std::vector<GLuint>& uniform_array_tex_diffuse_rgb,
		const std::vector<GLuint>& uniform_array_tex_specular_intensity) noexcept;

private:
	const float4 _clear_value_color = -float4::unit_xyzw;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Material_lighting_pass_shader_program _prog;
};

class Shadow_map_pass final {
public:

	Shadow_map_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code);

	Shadow_map_pass(const Shadow_map_pass&) = delete;

	Shadow_map_pass(Shadow_map_pass&&) = delete;

	~Shadow_map_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const Directional_light_params& dir_light) noexcept;

	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
		const std::vector<float>& uniform_array_model_matrix) noexcept;

private:
	// Filters Gbuffer.tex_shadow_map
	void filter_shadow_map() noexcept;

	const float _clear_value_shadow_occlusion[4] = {
		std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0, 0
	};
	const float _clear_value_depth = 1.0f;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Shadow_map_pass_shader_program _prog;
	Filter_shader_program _filter_shader_program;
};

// Ssao pass uses 8 sample rays (sample kernel size = 8) and 8 random normals that are used
// to randomly rotate each sample ray.
class Ssao_pass final {
public:

	Ssao_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code);

	Ssao_pass(const Ssao_pass&) = delete;

	Ssao_pass(Ssao_pass&&) = delete;

	~Ssao_pass() noexcept = default;


	void perform() noexcept;

private:
	void filter_ssao_map() noexcept;

	const size_t sample_ray_count = 8;
	const size_t random_normal_count = 8;
	const float4 _clear_value_ssao_map;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Ssao_pass_shader_program _prog;
	// [0.. 7] sample rays, [8 .. 15] random normals
	const std::vector<float3> _sample_rays;
	Filter_shader_program _filter_shader_program;
};

class Tone_mapping_pass final {
public:

	Tone_mapping_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code);

	Tone_mapping_pass(const Tone_mapping_pass&) = delete;

	Tone_mapping_pass(Tone_mapping_pass&&) = delete;

	~Tone_mapping_pass() noexcept = default;


	void perform() noexcept;

private:
	const float4 _clear_value_color = rgba(0x9f9dcaff);

	Gbuffer& _gbuffer;
	Tone_mapping_pass_shader_program _prog;
};

struct Renderer_config final {

	Vertex_attrib_layout vertex_attrib_layout;
	uint2 viewport_size;
	cg::data::Model_geometry_data<cg::data::vertex_attribs::p_tc> rect_1x1_mesh_data;
	cg::data::Glsl_program_desc gbuffer_pass_code;
	cg::data::Glsl_program_desc lighting_pass_dir_code;
	cg::data::Glsl_program_desc material_lighting_pass_code;
	cg::data::Glsl_program_desc shadow_map_pass_code;
	cg::data::Glsl_program_desc ssao_pass_code;
	cg::data::Glsl_program_desc tone_mapping_pass_code;
};

class Renderer final {
public:

	explicit Renderer(const Renderer_config& config);

	Renderer(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;

	~Renderer() noexcept = default;


	void render(const Frame& frame) noexcept;

	void resize_viewport(const uint2& size) noexcept;

	const Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _gbuffer.vertex_attrib_layout();
	}

private:

	void perform_gbuffer_pass(const Frame& frame) noexcept;

	void perform_lighting_pass(const Frame& frame) noexcept;

	void perform_material_lighting_pass(const Frame& frame) noexcept;

	void perform_shadow_map_pass(const Frame& frame) noexcept;

	Gbuffer _gbuffer;
	Gbuffer_pass _gbuffer_pass;
	Lighting_pass _lighting_pass;
	Shadow_map_pass _shadow_map_pass;
	Ssao_pass _ssao_pass;
	Material_lighting_pass _material_lighting_pass;
	Tone_mapping_pass _tone_mapping_pass;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
