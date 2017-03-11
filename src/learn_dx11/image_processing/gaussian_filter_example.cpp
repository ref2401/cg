#include "learn_dx11/image_processing/gaussian_filter_example.h"

#include "cg/data/image.h"
#include "cg/data/shader.h"

using namespace cg;
using namespace cg::data;


namespace learn_dx11 {
namespace image_processing {

// ----- Gaussian_filter_example -----

Gaussian_filter_example::Gaussian_filter_example(Render_context& rnd_ctx)
	: Example(rnd_ctx)
{
	_model_matrix_0 = ts_matrix(float3(-0.43f, 0.0f, 0.0f), float3(0.4f));
	_model_matrix_1 = ts_matrix(float3(0.43f, 0.0f, 0.0f), float3(0.4f));

	_matrix_cbuffer = make_cbuffer(_device, sizeof(mat4));
	init_pipeline_state();
	init_shaders();
	init_textures();
	update_pvm_matrix(rnd_ctx.viewport_size().aspect_ratio());
	
	perform_filtering();
	
	setup_pipeline_state();
}

void Gaussian_filter_example::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	// rastr state
	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_NONE;
	rastr_desc.FrontCounterClockwise = true;
	hr = _device->CreateRasterizerState(&rastr_desc, &_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Gaussian_filter_example::init_shaders()
{
	auto compute_desc = cg::data::load_hlsl_compute_desc("../data/learn_dx11/image_processing/gaussian_filter.compute.hlsl");
	compute_desc.compute_shader_entry_point = "cs_main";
	_gaussian_filter_compute = Hlsl_compute(_device, compute_desc);

	auto hlsl_desc = cg::data::load_hlsl_shader_set_desc("../data/learn_dx11/image_processing/render_image.hlsl");
	hlsl_desc.vertex_shader_entry_point = "vs_main";
	hlsl_desc.pixel_shader_entry_point = "ps_main";
	_render_image_shader = Hlsl_shader_set(_device, hlsl_desc);
}

void Gaussian_filter_example::init_textures()
{
	Image_2d image("../data/learn_dx11/image_processing/chessboard_marble.png", 4);

	D3D11_TEXTURE2D_DESC source_desc = {};
	source_desc.Width = image.size().width;
	source_desc.Height = image.size().height;
	source_desc.MipLevels = 1;
	source_desc.ArraySize = 1;
	source_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	source_desc.SampleDesc.Count = 1;
	source_desc.SampleDesc.Quality = 0;
	source_desc.Usage = D3D11_USAGE_IMMUTABLE;
	source_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA source_data = {};
	source_data.pSysMem = image.data();
	source_data.SysMemPitch = image.size().width * cg::data::byte_count(image.pixel_format());
	HRESULT hr = _device->CreateTexture2D(&source_desc, &source_data, &_tex_source.ptr);
	assert(hr == S_OK);
	hr = _device->CreateShaderResourceView(_tex_source.ptr, nullptr, &_tex_srv_source.ptr);
	assert(hr == S_OK);

	D3D11_TEXTURE2D_DESC filter_desc = source_desc;
	filter_desc.Usage = D3D11_USAGE_DEFAULT;
	filter_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	hr = _device->CreateTexture2D(&filter_desc, nullptr, &_tex_intermediate.ptr);
	assert(hr == S_OK);
	hr = _device->CreateShaderResourceView(_tex_intermediate.ptr, nullptr, &_tex_srv_intermediate.ptr);
	assert(hr == S_OK);
	hr = _device->CreateUnorderedAccessView(_tex_intermediate.ptr, nullptr, &_tex_uav_intermediate.ptr);
	assert(hr == S_OK);

	hr = _device->CreateTexture2D(&filter_desc, nullptr, &_tex_final.ptr);
	assert(hr == S_OK);
	hr = _device->CreateShaderResourceView(_tex_final.ptr, nullptr, &_tex_srv_final.ptr);
	assert(hr == S_OK);
	hr = _device->CreateUnorderedAccessView(_tex_final.ptr, nullptr, &_tex_uav_final.ptr);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = _device->CreateSamplerState(&sampler_desc, &_sampler_state.ptr);
	assert(hr == S_OK);
}

void Gaussian_filter_example::perform_filtering()
{
	constexpr size_t compute_kernel_width = 128;

	uint2 offset_arr[2];
	Com_ptr<ID3D11Buffer> offset_cbuffer = make_cbuffer(_device, 2 * sizeof(offset_arr));

	D3D11_TEXTURE2D_DESC tex_desc;
	_tex_source->GetDesc(&tex_desc);
	const uint2 size(tex_desc.Width, tex_desc.Height);

	_device_ctx->CSSetShader(_gaussian_filter_compute.shader(), nullptr, 0);
	_device_ctx->CSSetConstantBuffers(0, 1, &offset_cbuffer.ptr);
	
	// filter horizontally
	offset_arr[0] = uint2(1, 0);
	_device_ctx->UpdateSubresource(offset_cbuffer.ptr, 0, nullptr, offset_arr, 0, 0);
	_device_ctx->CSSetUnorderedAccessViews(0, 1, &_tex_uav_intermediate.ptr, nullptr);
	_device_ctx->CSSetShaderResources(0, 1, &_tex_srv_source.ptr);
	_device_ctx->Dispatch(size.width / compute_kernel_width, size.height, 1);
	
	// filter vertically
	offset_arr[0] = uint2(0, 1);
	_device_ctx->UpdateSubresource(offset_cbuffer.ptr, 0, nullptr, offset_arr, 0, 0);
	_device_ctx->CSSetUnorderedAccessViews(0, 1, &_tex_uav_final.ptr, nullptr);
	_device_ctx->CSSetShaderResources(0, 1, &_tex_srv_intermediate.ptr);
	_device_ctx->Dispatch(size.width / compute_kernel_width, size.height, 1);
	
	ID3D11UnorderedAccessView* uavs[1] = { nullptr };
	_device_ctx->CSSetShader(nullptr, nullptr, 0);
	_device_ctx->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
}

void Gaussian_filter_example::on_viewport_resize(const cg::uint2& viewport_size) 
{
	update_pvm_matrix(viewport_size.aspect_ratio());
}

void Gaussian_filter_example::render()
{
	const float4 clear_color = cg::rgba(0xd1d7ffff);

	clear_color_buffer(clear_color);
	
	// source image
	_device_ctx->PSSetShaderResources(0, 1, &_tex_srv_source.ptr);
	update_matrix_cbuffer(_pvm_matrix_0);
	_device_ctx->Draw(4, 0);
	
	// final image
	_device_ctx->PSSetShaderResources(0, 1, &_tex_srv_final.ptr);
	update_matrix_cbuffer(_pvm_matrix_1);
	_device_ctx->Draw(4, 0);
}

void Gaussian_filter_example::setup_pipeline_state()
{
	assert(_rasterizer_state.ptr);

	// input assembler
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// vertex shader
	_device_ctx->VSSetShader(_render_image_shader.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_matrix_cbuffer.ptr);
	// pixel shader
	_device_ctx->PSSetShader(_render_image_shader.pixel_shader(), nullptr, 0);
	_device_ctx->PSSetSamplers(0, 1, &_sampler_state.ptr);

	// rasterizer
	_device_ctx->RSSetState(_rasterizer_state.ptr);
	// output merger
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Gaussian_filter_example::update_matrix_cbuffer(const cg::mat4& matrix)
{
	float arr[16];
	to_array_column_major_order(matrix, arr);
	_device_ctx->UpdateSubresource(_matrix_cbuffer.ptr, 0, nullptr, arr, 0, 0);
}

void Gaussian_filter_example::update_pvm_matrix(float aspect_ratio)
{
	assert(aspect_ratio > 0.0f);
	assert(_matrix_cbuffer.ptr);

	const mat4 projection_matrix = cg::orthographic_matrix_directx(aspect_ratio, 1.0f, 0.1f, 100.0f);
	_pvm_matrix_0 = projection_matrix * _model_matrix_0;
	_pvm_matrix_1 = projection_matrix * _model_matrix_1;
}

} // namespace image_processing
} // namespace learn_dx11
