#include "learn_dx11/mesh_rnd/vertex_skinning_example.h"

#include "cg/data/shader.h"
#include "learn_dx11/base/model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::mat4;


namespace learn_dx11 {
namespace mesh_rnd {

Vertex_skinning_example::Vertex_skinning_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{
	init_cbuffers();
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	
	_view_matrix = cg::view_matrix(float3(0, 1, 10.0f), float3::zero);
	update_projection_matrix(rnd_ctx.pipeline_state().viewport_size().aspect_ratio());
	setup_projection_view_matrices();
	setup_pipeline_state();
}

void Vertex_skinning_example::init_cbuffers()
{
	_scene_cbuffer = make_cbuffer(_device, sizeof(mat4));
	_model_cbuffer = make_cbuffer(_device, sizeof(mat4));

	float matrix_data[16];
	_model_matrix = cg::rotation_matrix_oy<mat4>(cg::pi);
	to_array_column_major_order(_model_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Vertex_skinning_example::init_geometry()
{
	Bob_lamp_md5_model model;
	_draw_count = model.bone_positions().size();

	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = _draw_count * sizeof(float3);
	vb_desc.Usage = D3D11_USAGE_DEFAULT;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.bone_positions().data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	D3D11_INPUT_ELEMENT_DESC layout_descs[1] = {
		{ "V_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _device->CreateInputLayout(layout_descs, 1, 
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
	assert(hr == S_OK);
}

void Vertex_skinning_example::init_shaders()
{
	auto hlsl_data = cg::data::load_hlsl_shader_set_data("../data/learn_dx11/mesh_rnd/vertex_skinning.hlsl");
	hlsl_data.vertex_shader_entry_point = "vs_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
}

void Vertex_skinning_example::update_projection_matrix(float wh_aspect_ratio)
{
	_projection_matrix = cg::perspective_matrix_directx(cg::pi_3, wh_aspect_ratio, 1.0f, 100.0f);
}

void Vertex_skinning_example::setup_pipeline_state()
{
	size_t offset = 0;
	size_t stride = sizeof(float3);
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &stride, &offset);
	//_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_scene_cbuffer.ptr);
	_device_ctx->VSSetConstantBuffers(1, 1, &_model_cbuffer.ptr);

	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Vertex_skinning_example::setup_projection_view_matrices()
{
	float matrix_data[16];
	mat4 proj_view_matrix = _projection_matrix * _view_matrix;
	to_array_column_major_order(proj_view_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_scene_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Vertex_skinning_example::on_viewport_resize(const cg::uint2& viewport_size)
{
	update_projection_matrix(viewport_size.aspect_ratio());
	setup_projection_view_matrices();
}

void Vertex_skinning_example::render()
{
	static const float4 clear_color = cg::rgba(0xffffffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->Draw(_draw_count, 0);
}

void Vertex_skinning_example::update()
{
}

} // namespace mesh_rnd
} // namespace learn_dx11