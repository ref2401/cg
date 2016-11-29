#include "learn_dx11/mesh_rnd/displacement_mapping_example.h"

using cg::float3;
using cg::float4;
using cg::mat4;
using cg::uint2;


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Displacement_mapping_example -----

Displacement_mapping_example::Displacement_mapping_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{
	_view_matrix = cg::view_matrix(float3(0, 2, 8.0f), float3::zero);
	_model_matrix = mat4::identity;

	init_shaders();
	setup_pipeline_state();
}

void Displacement_mapping_example::init_shaders()
{
	auto hlsl_data = cg::data::load_hlsl_shader_set_data(
		"../data/learn_dx11/mesh_rnd/displacement_mapping.hlsl");
	
	hlsl_data.vertex_shader_entry_point = "vs_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
}

void Displacement_mapping_example::on_viewport_resize(const uint2& viewport_size)
{
}

void Displacement_mapping_example::render()
{
	static const float4 clear_color = cg::rgba(0xbca8ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->Draw(1, 0);
}

void Displacement_mapping_example::setup_pipeline_state()
{
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);

	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Displacement_mapping_example::update_projection_matrix(float wh_aspect_ratio)
{
	_projection_matrix = cg::perspective_matrix_directx(cg::pi_3, wh_aspect_ratio, 1.0f, 100.0f);
}

} // namespace mesh_rnd
} // namespace learn_dx11
