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
	_view_matrix = cg::view_matrix(float3(0, 0, 3), float3::zero);
	_model_matrix = mat4::identity;

	init_shaders();
	init_cbuffers();

	update_projection_matrix(rnd_ctx.pipeline_state().viewport_size().aspect_ratio());
	setup_projection_view_matrix();
	setup_pipeline_state();
}

void Displacement_mapping_example::init_cbuffers()
{
	_model_cbuffer = make_cbuffer(_device, sizeof(mat4));
	float matrix_data[16];
	to_array_column_major_order(_model_matrix, matrix_data);
	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);

	_projection_view_cbuffer = make_cbuffer(_device, sizeof(mat4));
}

void Displacement_mapping_example::init_shaders()
{
	auto hlsl_data = cg::data::load_hlsl_shader_set_data(
		"../data/learn_dx11/mesh_rnd/displacement_mapping.hlsl");
	
	hlsl_data.vertex_shader_entry_point = "vs_main";
	//hlsl_data.hull_shader_entry_point = "hs_main";
	//hlsl_data.domain_shader_entry_point = "ds_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
}

void Displacement_mapping_example::on_viewport_resize(const uint2& viewport_size)
{
	update_projection_matrix(viewport_size.aspect_ratio());
	setup_projection_view_matrix();
}

void Displacement_mapping_example::render()
{
	static const float4 clear_color = cg::rgba(0xbca8ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->Draw(3, 0);
}

void Displacement_mapping_example::setup_pipeline_state()
{
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_model_cbuffer.ptr);

	// hull
	// domain
	//_device_ctx->VSSetConstantBuffers(1, 1, &_projection_view_cbuffer.ptr);

	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	// wireframe rasterizer state
	D3D11_RASTERIZER_DESC rastr_state_desc = {};
	_pipeline_state.rasterizer_state()->GetDesc(&rastr_state_desc);
	rastr_state_desc.FillMode = D3D11_FILL_WIREFRAME;
	setup_rasterizer_state(rastr_state_desc);
	
	HRESULT hr = _device->CreateRasterizerState(&rastr_state_desc, &_wireframe_rasterizer_state.ptr);
	assert(hr == S_OK);
	_device_ctx->RSSetState(_wireframe_rasterizer_state.ptr);

	hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Displacement_mapping_example::setup_projection_view_matrix()
{
	float matrix_data[16];
	mat4 proj_view_matrix = _projection_matrix * _view_matrix;
	to_array_column_major_order(proj_view_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_projection_view_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Displacement_mapping_example::update_projection_matrix(float wh_aspect_ratio)
{
	_projection_matrix = cg::perspective_matrix_directx(cg::pi_3, wh_aspect_ratio, 1.0f, 10.0f);
}

} // namespace mesh_rnd
} // namespace learn_dx11
