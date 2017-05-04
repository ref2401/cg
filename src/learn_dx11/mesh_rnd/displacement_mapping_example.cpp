#include "learn_dx11/mesh_rnd/displacement_mapping_example.h"

#include "cg/data/image.h"


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Displacement_mapping_example -----

Displacement_mapping_example::Displacement_mapping_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	_view_matrix = view_matrix(float3(0, 3, 3), float3::zero);
	_model_matrix = rotation_matrix_ox<float4x4>(-pi_2) * scale_matrix<float4x4>(float3(3));
	//_model_matrix = cg::scale_matrix<mat4>(float3(2));

	init_shaders();
	init_cbuffers();
	init_terrain_textures();
	init_render_states();

	update_projection_matrix(aspect_ratio(rnd_ctx.viewport_size()));
	setup_pvm_matrix();
	setup_pipeline_state();
}

void Displacement_mapping_example::init_cbuffers()
{
	_model_cbuffer = make_cbuffer(_device, sizeof(float4x4));
	float matrix_data[16];
	to_array_column_major_order(_model_matrix, matrix_data);
	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);

	_projection_view_cbuffer = make_cbuffer(_device, sizeof(float4x4));
}

void Displacement_mapping_example::init_render_states()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	// fill_solid rastr state
	D3D11_RASTERIZER_DESC def_raster_desc = {};
	def_raster_desc.FillMode = D3D11_FILL_SOLID;
	def_raster_desc.CullMode = D3D11_CULL_BACK;
	def_raster_desc.FrontCounterClockwise = true;

	hr = _device->CreateRasterizerState(&def_raster_desc, &_default_rasterizer_state.ptr);
	assert(hr == S_OK);

	// wireframe rastr state
	D3D11_RASTERIZER_DESC wire_raster_desc = def_raster_desc;
	wire_raster_desc.FillMode = D3D11_FILL_WIREFRAME;

	hr = _device->CreateRasterizerState(&wire_raster_desc, &_wireframe_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Displacement_mapping_example::init_shaders()
{
	auto hlsl_data = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/learn_dx11/mesh_rnd/displacement_mapping.hlsl");
	
	hlsl_data.vertex_shader_entry_point = "vs_main";
	hlsl_data.hull_shader_entry_point = "hs_main";
	hlsl_data.domain_shader_entry_point = "ds_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
}

void Displacement_mapping_example::init_terrain_textures()
{
	using cg::data::image_2d;
	using cg::data::pixel_format;
	using cg::data::byte_count;

	// displacement map
	image_2d image_displ("../../data/learn_dx11/terrain_displacement_map.png", 1, false);

	D3D11_TEXTURE2D_DESC tex_displ_desc = {};
	tex_displ_desc.Width = image_displ.size.x;
	tex_displ_desc.Height = image_displ.size.y;
	tex_displ_desc.MipLevels = 1;
	tex_displ_desc.ArraySize = 1;
	tex_displ_desc.Format = DXGI_FORMAT_R8_UNORM;
	tex_displ_desc.SampleDesc.Count = 1;
	tex_displ_desc.SampleDesc.Quality = 0;
	tex_displ_desc.Usage = D3D11_USAGE_IMMUTABLE;
	tex_displ_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA tex_displ_data = {};
	tex_displ_data.pSysMem = image_displ.data;
	tex_displ_data.SysMemPitch = UINT(image_displ.size.x * byte_count(image_displ.pixel_format));
	tex_displ_data.SysMemSlicePitch = UINT(byte_count(image_displ));

	HRESULT hr = _device->CreateTexture2D(&tex_displ_desc, &tex_displ_data, &_tex_terrain_displacement_map.ptr);
	assert(hr == S_OK);

	hr = _device->CreateShaderResourceView(_tex_terrain_displacement_map.ptr,
		nullptr, &_tex_terrain_displacement_map_srv.ptr);
	assert(hr == S_OK);

	// normal map
	image_2d image_normal ("../../data/learn_dx11/terrain_normal_map.png", 4, false);

	D3D11_TEXTURE2D_DESC tex_normal_desc = {};
	tex_normal_desc.Width = image_normal.size.x;
	tex_normal_desc.Height = image_normal.size.y;
	tex_normal_desc.MipLevels = 1;
	tex_normal_desc.ArraySize = 1;
	tex_normal_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_normal_desc.SampleDesc.Count = 1;
	tex_normal_desc.SampleDesc.Quality = 0;
	tex_normal_desc.Usage = D3D11_USAGE_IMMUTABLE;
	tex_normal_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA tex_normal_data = {};
	tex_normal_data.pSysMem = image_normal.data;
	tex_normal_data.SysMemPitch = UINT(image_normal.size.x * byte_count(image_normal.pixel_format));
	tex_normal_data.SysMemSlicePitch = UINT(byte_count(image_normal));

	hr = _device->CreateTexture2D(&tex_normal_desc, &tex_normal_data, &_tex_terrain_normal_map.ptr);
	assert(hr == S_OK);

	hr = _device->CreateShaderResourceView(_tex_terrain_normal_map.ptr,
		nullptr, &_tex_terrain_normal_map_srv.ptr);
	assert(hr == S_OK);

	// sampler
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = _device->CreateSamplerState(&sampler_desc, &_linear_sampler.ptr);
	assert(hr == S_OK);
}

void Displacement_mapping_example::on_viewport_resize(const uint2& viewport_size)
{
	update_projection_matrix(aspect_ratio(viewport_size));
	setup_pvm_matrix();
}

void Displacement_mapping_example::render()
{
	static const float4 clear_color = rgba(0xbca8ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->Draw(6, 0);
}

void Displacement_mapping_example::setup_pipeline_state()
{
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_model_cbuffer.ptr);

	// hull
	_device_ctx->HSSetShader(_shader_set.hull_shader(), nullptr, 0);
	// domain
	_device_ctx->DSSetShader(_shader_set.domain_shader(), nullptr, 0);
	_device_ctx->DSSetConstantBuffers(0, 1, &_projection_view_cbuffer.ptr);
	_device_ctx->DSSetShaderResources(0, 1, &_tex_terrain_displacement_map_srv.ptr);
	_device_ctx->DSSetSamplers(0, 1, &_linear_sampler.ptr);

	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);
	_device_ctx->PSSetShaderResources(0, 1, &_tex_terrain_normal_map_srv.ptr);
	_device_ctx->PSSetSamplers(0, 1, &_linear_sampler.ptr);

	//_device_ctx->RSSetState(_default_rasterizer_state.ptr);
	_device_ctx->RSSetState(_wireframe_rasterizer_state.ptr);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Displacement_mapping_example::setup_pvm_matrix()
{
	float matrix_data[16];
	float4x4 proj_view_matrix = _projection_matrix * _view_matrix;
	to_array_column_major_order(proj_view_matrix, matrix_data);

	_device_ctx->UpdateSubresource(_projection_view_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Displacement_mapping_example::update_projection_matrix(float wh_aspect_ratio)
{
	_projection_matrix = perspective_matrix_directx(pi_3, wh_aspect_ratio, 1.0f, 10.0f);
}

} // namespace mesh_rnd
} // namespace learn_dx11
