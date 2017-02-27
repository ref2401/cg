#include "learn_dx11/mesh_rnd/vertex_skinning_example.h"

#include <type_traits>
#include "cg/data/shader.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::mat4;


namespace learn_dx11 {
namespace mesh_rnd {

Vertex_skinning_example::Vertex_skinning_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	init_cbuffers();
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	init_render_states();
	
	_view_matrix = cg::view_matrix(float3(0, 2, 8.0f), float3::zero);
	update_projection_matrix(rnd_ctx.viewport_size().aspect_ratio());
	update_projection_view_matrices();

	setup_pipeline_state();
}

void Vertex_skinning_example::init_cbuffers()
{
	_scene_cbuffer = make_cbuffer(_device, sizeof(mat4));
	_model_cbuffer = make_cbuffer(_device, sizeof(mat4));

	float matrix_data[16];
	_model_matrix = cg::translation_matrix(float3(0, -3.4f, 0)) * cg::rotation_matrix_ox<mat4>(-cg::pi_2);
	to_array_column_major_order(_model_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Vertex_skinning_example::init_geometry()
{
	Bob_lamp_md5_model model;
	init_draw_indexed_params(model.mesh_draw_params());
	_model_animation = model.get_model_animation();
	_curr_bone_matrices_data = std::vector<float>(_model_animation->bone_count() * 16, 0.0f);

	// bone matrices buffer
	D3D11_BUFFER_DESC bmb_desc = {};
	bmb_desc.ByteWidth = _model_animation->bone_count() * sizeof(mat4);
	bmb_desc.Usage = D3D11_USAGE_DEFAULT;
	bmb_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bmb_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bmb_desc.StructureByteStride = sizeof(mat4);

	HRESULT hr = _device->CreateBuffer(&bmb_desc, nullptr, &_model_bone_matrices_buffer.ptr);
	assert(hr == S_OK);

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.NumElements = _model_animation->bone_count();

	hr = _device->CreateShaderResourceView(_model_bone_matrices_buffer.ptr, &srv_desc, &_model_bone_matrices_buffer_srv.ptr);
	assert(hr == S_OK);

	// vertex buffer & input layout
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = model.vertex_count() * sizeof(Vertex);
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.vertices().data();

	hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	D3D11_INPUT_ELEMENT_DESC layout_descs[5] = {
		{ "V_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_TEX_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, tex_coord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, offsetof(Vertex, bone_indices), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, bone_weights), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _device->CreateInputLayout(layout_descs, std::extent<decltype(layout_descs)>::value,
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = model.index_count() * sizeof(uint32_t);
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = model.indices().data();

	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);
}

void Vertex_skinning_example::init_draw_indexed_params(
	const std::vector<Mesh_draw_params>& mesh_draw_params)
{
	using cg::data::byte_count;
	using cg::data::channel_count;

	assert(mesh_draw_params.size() > 0);

	_draw_indexed_params = std::vector<Draw_indexed_params>(mesh_draw_params.size());
	for (size_t i = 0; i < mesh_draw_params.size(); ++i) {
		_draw_indexed_params[i].index_count = mesh_draw_params[i].index_count;
		_draw_indexed_params[i].index_offset = mesh_draw_params[i].index_offset;
		_draw_indexed_params[i].base_vertex = mesh_draw_params[i].base_vertex;

		const auto& image = mesh_draw_params[i].diffuse_rgb_image;

		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = image.size().width;
		tex_desc.Height = image.size().height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		
		D3D11_SUBRESOURCE_DATA tex_data = {};
		tex_data.pSysMem = image.data();
		tex_data.SysMemPitch = image.size().width * byte_count(image.pixel_format());
		tex_data.SysMemSlicePitch = image.byte_count();

		HRESULT hr = _device->CreateTexture2D(&tex_desc, &tex_data, 
			&_draw_indexed_params[i]._tex_diffuse_rgb.ptr);
		assert(hr == S_OK);

		hr = _device->CreateShaderResourceView(_draw_indexed_params[i]._tex_diffuse_rgb.ptr,
			nullptr, &_draw_indexed_params[i]._tex_diffuse_rgb_srv.ptr);
		assert(hr == S_OK);
	}

	// sampler
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	HRESULT hr = _device->CreateSamplerState(&sampler_desc, &_linear_sampler_state.ptr);
	assert(hr == S_OK);
}

void Vertex_skinning_example::init_render_states()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = true;

	hr = _device->CreateRasterizerState(&rasterizer_desc, &_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Vertex_skinning_example::init_shaders()
{
	auto hlsl_data = cg::data::load_hlsl_shader_set_desc("../data/learn_dx11/mesh_rnd/vertex_skinning.hlsl");
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
	size_t stride = sizeof(Vertex);
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &stride, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_scene_cbuffer.ptr);
	_device_ctx->VSSetConstantBuffers(1, 1, &_model_cbuffer.ptr);
	_device_ctx->VSSetShaderResources(0, 1, &_model_bone_matrices_buffer_srv.ptr);

	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);
	_device_ctx->PSSetSamplers(0, 1, &_linear_sampler_state.ptr);

	// rasterizer stage
	_device_ctx->RSSetState(_rasterizer_state.ptr);

	// output merger stage
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 1);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Vertex_skinning_example::update_projection_view_matrices()
{
	float matrix_data[16];
	mat4 proj_view_matrix = _projection_matrix * _view_matrix;
	to_array_column_major_order(proj_view_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_scene_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Vertex_skinning_example::on_viewport_resize(const cg::uint2& viewport_size)
{
	update_projection_matrix(viewport_size.aspect_ratio());
	update_projection_view_matrices();
}

void Vertex_skinning_example::render()
{
	static const float4 clear_color = cg::rgba(0xbca8ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	for (const auto& dp : _draw_indexed_params) {
		_device_ctx->PSSetShaderResources(1, 1, &dp._tex_diffuse_rgb_srv.ptr);
		_device_ctx->DrawIndexed(dp.index_count, dp.index_offset, dp.base_vertex);
	}
}

void Vertex_skinning_example::update(float dt_milliseconds)
{
	static float animation_milliseconds = 0;

	if (animation_milliseconds + dt_milliseconds > _model_animation->total_milliseconds()) {
		animation_milliseconds = 0;
	}
	else {
		animation_milliseconds += dt_milliseconds;
	}

	_model_animation->update_bone_matrices(animation_milliseconds, _curr_bone_matrices_data);
	_device_ctx->UpdateSubresource(_model_bone_matrices_buffer.ptr, 0, nullptr,
		_curr_bone_matrices_data.data(), 0, 0);

}

} // namespace mesh_rnd
} // namespace learn_dx11