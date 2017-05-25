#include "technique/pbr/pbr.h"

#include <type_traits>
#include "cg/data/model.h"

using namespace cg::data;


namespace {

com_ptr<ID3D11Texture2D> load_envmap(ID3D11Device* device, const char* filename)
{
	const image_2d hdr_image(filename, 4);
	com_ptr<ID3D11Texture2D> tex_envmap;

	D3D11_TEXTURE2D_DESC envmap_desc = {};
	envmap_desc.Width = hdr_image.size.x;
	envmap_desc.Height = hdr_image.size.y;
	envmap_desc.MipLevels = 1;
	envmap_desc.ArraySize = 1;
	envmap_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	envmap_desc.SampleDesc.Count = 1;
	envmap_desc.SampleDesc.Quality = 0;
	envmap_desc.Usage = D3D11_USAGE_IMMUTABLE;
	envmap_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA env_map_data = {};
	env_map_data.pSysMem = hdr_image.data;
	env_map_data.SysMemPitch = UINT(hdr_image.size.x * byte_count(hdr_image.pixel_format));
	
	HRESULT hr = device->CreateTexture2D(&envmap_desc, &env_map_data, &tex_envmap.ptr);
	assert(hr == S_OK);

	return tex_envmap;
}

} // namespace


namespace pbr {

// ----- cube_envmap_pass -----

cube_envmap_pass::cube_envmap_pass(ID3D11Device* device, ID3D11DeviceContext* device_ctx, ID3D11Debug* debug,
	const char* envmap_filename, size_t cube_side_size, size_t irradiance_side_size,
	size_t reflection_size_size)
	: device_(device),
	device_ctx_(device_ctx),
	debug_(debug)
{
	assert(device);
	assert(device_ctx);
	assert(debug);
	assert(envmap_filename);
	assert(cube_side_size > 0);
	assert(irradiance_side_size > 0);
	assert(reflection_size_size >= 32);

	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = tex_desc.Height = UINT(cube_side_size);
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 6;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tex_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	// cube envmap
	HRESULT hr = device->CreateTexture2D(&tex_desc, nullptr, &tex_cube_envmap_.ptr);
	assert(hr == S_OK);
	hr = device->CreateShaderResourceView(tex_cube_envmap_, nullptr, &tex_cube_envmap_srv_.ptr);
	assert(hr == S_OK);
	
	// irradiance map
	tex_desc.Width = tex_desc.Height = UINT(irradiance_side_size);
	hr = device->CreateTexture2D(&tex_desc, nullptr, &tex_irradiance_map_.ptr);
	assert(hr == S_OK);
	hr = device->CreateShaderResourceView(tex_irradiance_map_, nullptr, &tex_irradiance_map_srv_.ptr);
	assert(hr == S_OK);

	// reflection map
	tex_desc.Width = tex_desc.Height = UINT(reflection_size_size);
	tex_desc.MipLevels = UINT(cube_envmap_pass::reflection_mip_level_count);
	hr = device->CreateTexture2D(&tex_desc, nullptr, &tex_reflection_map_.ptr);
	assert(hr == S_OK);
	hr = device->CreateShaderResourceView(tex_reflection_map_, nullptr, &tex_reflection_map_srv_.ptr);
	assert(hr == S_OK);

	init_pipeline_state();

	// depth stencil state
	com_ptr<ID3D11DepthStencilState> depth_stencil_state;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	hr = device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state.ptr);
	device_ctx_->OMSetDepthStencilState(depth_stencil_state, 0);
	init_brdf_maps();
	init_cube_maps(envmap_filename, cube_side_size, irradiance_side_size, reflection_size_size);
}

void cube_envmap_pass::init_brdf_maps()
{
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = tex_desc.Height = 2048;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT hr = device_->CreateTexture2D(&tex_desc, nullptr, &tex_brdf_map_.ptr);
	assert(hr == S_OK);
	hr = device_->CreateShaderResourceView(tex_brdf_map_, nullptr, &tex_brdf_map_srv_.ptr);
	assert(hr == S_OK);
	com_ptr<ID3D11RenderTargetView> tex_brdf_map_rtv;
	hr = device_->CreateRenderTargetView(tex_brdf_map_, nullptr, &tex_brdf_map_rtv.ptr);
	assert(hr == S_OK);

	// shader
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/gen_brdf_map.hlsl");
	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.pixel_shader_entry_point = "ps_main";
	hlsl_shader shader = hlsl_shader(device_, shader_desc);

	// render a quat and populate the brdf map:
	//
	device_ctx_->IASetInputLayout(nullptr);
	device_ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_ctx_->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	device_ctx_->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// rasterizer state
	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_FRONT;
	rastr_desc.FrontCounterClockwise = true;
	hr = device_->CreateRasterizerState(&rastr_desc, &rasterizer_state_.ptr);
	assert(hr == S_OK);
	// viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = viewport.Height = float(tex_desc.Width);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	device_ctx_->RSSetViewports(1, &viewport);

	device_ctx_->VSSetShader(shader.vertex_shader, nullptr, 0);
	device_ctx_->VSSetConstantBuffers(0, 0, nullptr);
	device_ctx_->PSSetShader(shader.pixel_shader, nullptr, 0);
	device_ctx_->PSSetShaderResources(0, 0, nullptr);
	device_ctx_->OMSetRenderTargets(1, &tex_brdf_map_rtv.ptr, nullptr);
	hr = debug_->ValidateContext(device_ctx_);
	assert(hr == S_OK);

	device_ctx_->Draw(4, 0);
}

void cube_envmap_pass::init_cube_maps(const char* envmap_filename, size_t cube_side_size, size_t irradiance_side_size,
	size_t reflection_size_size)
{
	// load an epirectengular hdr image:
	//
	com_ptr<ID3D11Texture2D> tex_envmap = load_envmap(device_, envmap_filename);
	com_ptr<ID3D11ShaderResourceView> tex_envmap_srv;
	HRESULT hr = device_->CreateShaderResourceView(tex_envmap, nullptr, &tex_envmap_srv.ptr);
	assert(hr == S_OK);

	// init pipeline state:
	//
	// input assembler
	device_ctx_->IASetInputLayout(nullptr);
	device_ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_ctx_->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	device_ctx_->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// rasterizer
	device_ctx_->RSSetState(rasterizer_state_);

	constexpr size_t face_count = 6;
	const float4x4 view_matrices[face_count] = {
		view_matrix(float3::zero,	float3(1.0f, 0.0f, 0.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(-1.0f, 0.0f, 0.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(0.0f, -1.0f, 0.0f),	-float3::unit_z),
		view_matrix(float3::zero,	float3(0.0f, 1.0f, 0.0f),	float3::unit_z),
		view_matrix(float3::zero,	float3(0.0f, 0.0f, 1.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(0.0f, 0.0f, -1.0f),	-float3::unit_y)
	};

	// generate tex_cube_envmap_:
	//
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = viewport.Height = float(cube_side_size);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		device_ctx_->RSSetViewports(1, &viewport);

		// shader
		auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/gen_cube_envmap.hlsl");
		shader_desc.vertex_shader_entry_point = "vs_main";
		shader_desc.pixel_shader_entry_point = "ps_main";
		hlsl_shader shader = hlsl_shader(device_, shader_desc);
		device_ctx_->VSSetShader(shader.vertex_shader, nullptr, 0);
		device_ctx_->VSSetConstantBuffers(0, 1, &constant_buffer_.ptr);
		device_ctx_->PSSetShader(shader.pixel_shader, nullptr, 0);
		device_ctx_->PSSetShaderResources(0, 1, &tex_envmap_srv.ptr);
		device_ctx_->PSSetSamplers(0, 1, &sampler_state_.ptr);

		for (size_t i = 0; i < face_count; ++i) {
			// render target view
			com_ptr<ID3D11RenderTargetView> rtv;
			D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
			rtv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtv_desc.Texture2DArray.FirstArraySlice = UINT(i);
			rtv_desc.Texture2DArray.ArraySize = 1;
			HRESULT hr = device_->CreateRenderTargetView(tex_cube_envmap_, &rtv_desc, &rtv.ptr);
			assert(hr == S_OK);

			device_ctx_->OMSetRenderTargets(1, &rtv.ptr, nullptr);
			hr = debug_->ValidateContext(device_ctx_);
			assert(hr == S_OK);
			const float4x4 proj_view_matrix = view_matrices[i];
			device_ctx_->UpdateSubresource(constant_buffer_, 0, nullptr, &proj_view_matrix.m00, 0, 0);
			device_ctx_->Draw(cube_envmap_pass::cube_index_count, 0);
		}
	}

	device_ctx_->OMSetRenderTargets(0, nullptr, nullptr);
	// generate tex_irradiance_map_:
	//
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = viewport.Height = float(irradiance_side_size);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		device_ctx_->RSSetViewports(1, &viewport);

		// shader
		auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/gen_irradiance_map.hlsl");
		shader_desc.vertex_shader_entry_point = "vs_main";
		shader_desc.pixel_shader_entry_point = "ps_main";
		hlsl_shader shader = hlsl_shader(device_, shader_desc);
		device_ctx_->VSSetShader(shader.vertex_shader, nullptr, 0);
		device_ctx_->VSSetConstantBuffers(0, 1, &constant_buffer_.ptr);
		device_ctx_->PSSetShader(shader.pixel_shader, nullptr, 0);
		device_ctx_->PSSetShaderResources(0, 1, &tex_cube_envmap_srv_.ptr);
		device_ctx_->PSSetSamplers(0, 1, &sampler_state_.ptr);

		for (size_t i = 0; i < face_count; ++i) {
			// render target view
			com_ptr<ID3D11RenderTargetView> rtv;
			D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
			rtv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtv_desc.Texture2DArray.FirstArraySlice = UINT(i);
			rtv_desc.Texture2DArray.ArraySize = 1;
			HRESULT hr = device_->CreateRenderTargetView(tex_irradiance_map_, &rtv_desc, &rtv.ptr);
			assert(hr == S_OK);

			device_ctx_->OMSetRenderTargets(1, &rtv.ptr, nullptr);
			hr = debug_->ValidateContext(device_ctx_);
			assert(hr == S_OK);
			const float4x4 proj_view_matrix = view_matrices[i];
			device_ctx_->UpdateSubresource(constant_buffer_, 0, nullptr, &proj_view_matrix.m00, 0, 0);
			device_ctx_->Draw(cube_envmap_pass::cube_index_count, 0);
		}
	}

	// generate tex_reflection_map:
	//
	{
		// shader
		auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/gen_reflection_map.hlsl");
		shader_desc.vertex_shader_entry_point = "vs_main";
		shader_desc.pixel_shader_entry_point = "ps_main";
		hlsl_shader shader = hlsl_shader(device_, shader_desc);
		device_ctx_->VSSetShader(shader.vertex_shader, nullptr, 0);
		device_ctx_->VSSetConstantBuffers(0, 1, &constant_buffer_.ptr);
		device_ctx_->PSSetShader(shader.pixel_shader, nullptr, 0);
		device_ctx_->PSSetShaderResources(0, 1, &tex_cube_envmap_srv_.ptr);
		device_ctx_->PSSetSamplers(0, 1, &sampler_state_.ptr);

		for (size_t mip = 0; mip < cube_envmap_pass::reflection_mip_level_count; ++mip) {
			const float mip_side_size = reflection_size_size * std::pow(0.5f, mip);
			
			D3D11_VIEWPORT viewport;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = viewport.Height = mip_side_size;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			device_ctx_->RSSetViewports(1, &viewport);

			for (size_t i = 0; i < face_count; ++i) {
				// render target view
				com_ptr<ID3D11RenderTargetView> rtv;
				D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
				rtv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv_desc.Texture2DArray.MipSlice = UINT(mip);
				rtv_desc.Texture2DArray.FirstArraySlice = UINT(i);
				rtv_desc.Texture2DArray.ArraySize = 1;
				HRESULT hr = device_->CreateRenderTargetView(tex_reflection_map_, &rtv_desc, &rtv.ptr);
				assert(hr == S_OK);

				device_ctx_->OMSetRenderTargets(1, &rtv.ptr, nullptr);
				hr = debug_->ValidateContext(device_ctx_);
				assert(hr == S_OK);
				const float4x4 proj_view_matrix = view_matrices[i];
				device_ctx_->UpdateSubresource(constant_buffer_, 0, nullptr, &proj_view_matrix.m00, 0, 0);
				device_ctx_->Draw(cube_envmap_pass::cube_index_count, 0);
			}
		}
	}
}

void cube_envmap_pass::init_pipeline_state()
{
	constant_buffer_ = constant_buffer(device_, sizeof(float4x4));

	// shader_
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/cube_envmap.hlsl");
	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.pixel_shader_entry_point = "ps_main";
	shader_ = hlsl_shader(device_, shader_desc);

	// rasterizer state
	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_FRONT;
	rastr_desc.FrontCounterClockwise = true;
	HRESULT hr = device_->CreateRasterizerState(&rastr_desc, &rasterizer_state_.ptr);
	assert(hr == S_OK);

	// depth stencil state
	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = true;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device_->CreateDepthStencilState(&ds_desc, &depth_stencil_state_.ptr);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = device_->CreateSamplerState(&sampler_desc, &sampler_state_.ptr);
	assert(hr == S_OK);
}

void cube_envmap_pass::render_envmap(const float4x4& projection_view_matrix)
{
	device_ctx_->UpdateSubresource(constant_buffer_, 0, nullptr, &projection_view_matrix.m00, 0, 0);

	device_ctx_->RSSetState(rasterizer_state_);
	device_ctx_->OMSetDepthStencilState(depth_stencil_state_, 0);
	// input assembly
	device_ctx_->IASetInputLayout(nullptr);
	device_ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_ctx_->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	device_ctx_->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// shader_
	device_ctx_->VSSetShader(shader_.vertex_shader, nullptr, 0);
	device_ctx_->VSSetConstantBuffers(0, 1, &constant_buffer_.ptr);
	device_ctx_->PSSetShader(shader_.pixel_shader, nullptr, 0);
	device_ctx_->PSSetShaderResources(0, 1, &tex_cube_envmap_srv_.ptr);
	//device_ctx_->PSSetShaderResources(0, 1, &tex_irradiance_map_srv_.ptr);
	//device_ctx_->PSSetShaderResources(0, 1, &tex_reflection_map_srv_.ptr);
	device_ctx_->PSSetSamplers(0, 1, &sampler_state_.ptr);
	
	HRESULT hr = debug_->ValidateContext(device_ctx_);
	assert(hr == S_OK);

	device_ctx_->Draw(cube_envmap_pass::cube_index_count, 0);
}

// ----- pbr -----

pbr::pbr(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx)
	: example(app_ctx, rhi_ctx),
	rhi_ctx_(dynamic_cast<cg::rnd::dx11::dx11_rhi_context&>(rhi_ctx)),
	device_(this->rhi_ctx_.device()),
	debug_(this->rhi_ctx_.debug()),
	device_ctx_(this->rhi_ctx_.device_ctx()),
	curr_viewpoint_(float3(0.0f, 0, 17.0f), float3::zero, float3::unit_y),
	cube_envmap_pass_(device_, device_ctx_, debug_, "../../data/hdr/winter_forest/WinterForest_Ref.hdr", 512, 32, 128)
{
	update_projection_matrix();
	model_position_ = float3::zero;
	model_rotation_ = quat::identity;
	//model_rotation_ = from_axis_angle_rotation(float3::unit_y, pi_8) * from_axis_angle_rotation(float3::unit_x, pi_2);
	model_scale_ = float3(4.0f);

	cb_vertex_shader_ = constant_buffer(device_, sizeof(float) * pbr::cb_vertex_shader_component_count);
	//cb_pixel_shader_ = constant_buffer(device_, sizeof(float) * parallax_occlusion_mapping::cb_pixel_shader_component_count);

	init_shader();
	init_geometry();
	init_pipeline_state();
}

void pbr::init_geometry()
{
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/bunny.obj");
	auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/sphere_64x32_1.obj");
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/rect_2x2.obj");
	assert(model.mesh_count() == 1);
	index_count_ = UINT(model.meshes()[0].index_count);

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = UINT(model.vertex_data_byte_count());
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.vertex_data().data();
	HRESULT hr = device_->CreateBuffer(&vb_desc, &vb_data, &vertex_buffer_.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = UINT(model.index_data_byte_count());
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = model.index_data().data();
	hr = device_->CreateBuffer(&ib_desc, &ib_data, &index_buffer_.ptr);
	assert(hr == S_OK);

	// input layout
	using Vf = decltype(model)::Format;
	vertex_stride_ = Vf::vertex_byte_count;
	D3D11_INPUT_ELEMENT_DESC layout_desc[4] = {
		{ "VERT_POSITION_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::position_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_NORMAL_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::normal_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, Vf::tex_coord_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_TANGENT_SPACE_MS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, Vf::tangent_space_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device_->CreateInputLayout(layout_desc, std::extent<decltype(layout_desc)>::value,
		shader_.vertex_shader_bytecode->GetBufferPointer(),
		shader_.vertex_shader_bytecode->GetBufferSize(),
		&input_layout_.ptr);
	assert(hr == S_OK);
}

void pbr::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = true;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;
	HRESULT hr = device_->CreateDepthStencilState(&ds_desc, &depth_stencil_state_.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_BACK;
	rastr_desc.FrontCounterClockwise = true;
	hr = device_->CreateRasterizerState(&rastr_desc, &rasterizer_state_.ptr);
	assert(hr == S_OK);

	D3D11_TEXTURE2D_DESC debug_desc = {};
	debug_desc.Width = UINT(rhi_ctx_.viewport().Width);
	debug_desc.Height = UINT(rhi_ctx_.viewport().Height);
	debug_desc.MipLevels = 1;
	debug_desc.ArraySize = 1;
	debug_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	debug_desc.SampleDesc.Count = 1;
	debug_desc.SampleDesc.Quality = 0;
	debug_desc.Usage = D3D11_USAGE_DEFAULT;
	debug_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	hr = device_->CreateTexture2D(&debug_desc, nullptr, &tex_debug_.ptr);
	assert(hr == S_OK);

	hr = device_->CreateRenderTargetView(tex_debug_, nullptr, &tex_debug_rtv_.ptr);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = device_->CreateSamplerState(&sampler_desc, &sampler_state_.ptr);
	assert(hr == S_OK);
}

void pbr::init_shader()
{
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/pbr.hlsl");
	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.pixel_shader_entry_point = "ps_main";
	shader_ = hlsl_shader(device_, shader_desc);
}

void pbr::on_mouse_move()
{
	const uint2 p = _app_ctx.mouse.position();
	const float2 curr_pos(float(p.x), float(p.y));
	const float2 diff = curr_pos - prev_mouse_position_;
	prev_mouse_position_ = curr_pos;

	if (!_app_ctx.mouse.middle_down() || diff == float2::zero) return;

	const bool y_offset_satisfies = !approx_equal(diff.y, 0.0f, 0.01f);
	const bool x_offset_satisfies = !approx_equal(diff.x, 0.0f, 0.01f);

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies)
		view_roll_angles_.y += (diff.x > 0.0f) ? -pi_128 : pi_128;

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies)
		view_roll_angles_.x += (diff.y > 0.0f) ? pi_128 : -pi_128;
}

void pbr::on_window_resize()
{
	update_projection_matrix();
}

void pbr::render(float interpolation_factor)
{
	//cube_envmap_pass cube_envmap_pass_(device_, device_ctx_, debug_, "../../data/hdr/winter_forest/WinterForest_Ref.hdr", 512, 32, 128);

	auto viewpoint = lerp(curr_viewpoint_, prev_viewpoint_, interpolation_factor);
	viewpoint.up = normalize(viewpoint.up);
	update_cb_vertex_shader(viewpoint);

	const UINT offset = 0;
	device_ctx_->RSSetViewports(1, &rhi_ctx_.viewport());
	ID3D11RenderTargetView* rtv_list[2] = { rhi_ctx_.rtv_window(), tex_debug_rtv_.ptr };
	device_ctx_->OMSetRenderTargets(2, rtv_list, rhi_ctx_.dsv_depth_stencil());
	device_ctx_->ClearRenderTargetView(rhi_ctx_.rtv_window(), &float4::unit_xyzw.x);
	device_ctx_->ClearDepthStencilView(rhi_ctx_.dsv_depth_stencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//// render a model:
	////
	//device_ctx_->RSSetState(rasterizer_state_);
	//device_ctx_->OMSetDepthStencilState(depth_stencil_state_, 0);
	//// input assembly
	//device_ctx_->IASetInputLayout(input_layout_);
	//device_ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//device_ctx_->IASetVertexBuffers(0, 1, &vertex_buffer_.ptr, &vertex_stride_, &offset);
	//device_ctx_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
	//// vertex & pixel shaders
	//device_ctx_->VSSetShader(shader_.vertex_shader, nullptr, 0);
	//device_ctx_->VSSetConstantBuffers(0, 1, &cb_vertex_shader_.ptr);
	//device_ctx_->PSSetShader(shader_.pixel_shader, nullptr, 0);
	//ID3D11ShaderResourceView* srv = cube_envmap_pass_.tex_irradiance_map_srv();
	//device_ctx_->PSSetShaderResources(0, 1, &srv);
	//device_ctx_->PSSetSamplers(0, 1, &sampler_state_.ptr);
	//HRESULT hr = debug_->ValidateContext(device_ctx_);
	//assert(hr == S_OK);
	//device_ctx_->DrawIndexed(index_count_, 0, 0);

	// render cube envmap:
	//
	const cg::Viewpoint viewpoint_cube_envmap(float3::zero, forward(viewpoint), viewpoint.up);
	const float4x4 cube_pvm_matrix = projection_matrix_ * view_matrix(viewpoint_cube_envmap);
	cube_envmap_pass_.render_envmap(cube_pvm_matrix);

	prev_viewpoint_ = curr_viewpoint_;
}

void pbr::update(float dt_msec)
{
	if (view_roll_angles_ != float2::zero) {
		float dist = distance(curr_viewpoint_);
		float3 ox = cross(forward(curr_viewpoint_), curr_viewpoint_.up);
		ox.y = 0.0f; // ox is always parallel the world's OX.
		ox = normalize(ox);

		if (!approx_equal(view_roll_angles_.y, 0.0f)) {
			const quat q = from_axis_angle_rotation(float3::unit_y, view_roll_angles_.y);
			curr_viewpoint_.position = dist * normalize(rotate(q, curr_viewpoint_.position));

			ox = rotate(q, ox);
			ox.y = 0.0f;
			ox = normalize(ox);
		}

		if (!approx_equal(view_roll_angles_.x, 0.0f)) {
			const quat q = from_axis_angle_rotation(ox, view_roll_angles_.x);
			curr_viewpoint_.position = dist * normalize(rotate(q, curr_viewpoint_.position));
		}

		curr_viewpoint_.up = normalize(cross(ox, forward(curr_viewpoint_)));
	}

	view_roll_angles_ = float2::zero;
}

void pbr::update_cb_vertex_shader(const cg::Viewpoint& viewpoint)
{
	static const float3 light_dir_to_ws = normalize(float3(0, 0, 10));
	const float4x4 model_matrix = trs_matrix(model_position_, model_rotation_, model_scale_);
	const float4x4 normal_matrix = rotation_matrix<float4x4>(model_rotation_);
	const float4x4 pvm_matrix = projection_matrix_ * view_matrix(viewpoint) * model_matrix;

	float arr[pbr::cb_vertex_shader_component_count];
	to_array_column_major_order(pvm_matrix, arr);
	to_array_column_major_order(model_matrix, arr + 16);
	to_array_column_major_order(normal_matrix, arr + 32);
	arr[48] = light_dir_to_ws.x;
	arr[49] = light_dir_to_ws.y;
	arr[50] = light_dir_to_ws.z;
	arr[51] = 0;
	arr[52] = viewpoint.position.x;
	arr[53] = viewpoint.position.y;
	arr[54] = viewpoint.position.z;
	arr[55] = 0;
	device_ctx_->UpdateSubresource(cb_vertex_shader_, 0, nullptr, arr, 0, 0);
}

void pbr::update_projection_matrix()
{
	projection_matrix_ = perspective_matrix_directx(pi_3, rhi_ctx_.viewport_aspect_ratio(), 0.1f, 1000.0f);
}

} // namespace pbr
