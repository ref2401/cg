#include "technique/pbr/pbr.h"

#include <type_traits>
#include "cg/data/model.h"

using namespace cg::data;


namespace pbr {

pbr::pbr(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx)
	: example(app_ctx, rhi_ctx),
	rhi_ctx(dynamic_cast<cg::rnd::dx11::dx11_rhi_context&>(rhi_ctx)),
	device(this->rhi_ctx.device()),
	debug(this->rhi_ctx.debug()),
	device_ctx(this->rhi_ctx.device_ctx()),
	curr_viewpoint(float3(0.0f, 0, 17.0f), float3::zero, float3::unit_y)
{
	update_projection_matrix();
	model_position = float3::zero;
	model_rotation = quat::identity;
	//model_rotation = from_axis_angle_rotation(float3::unit_y, pi_8) * from_axis_angle_rotation(float3::unit_x, pi_2);
	model_scale = float3(4.0f);

	cb_vertex_shader = constant_buffer(device, sizeof(float) * pbr::cb_vertex_shader_component_count);
	//cb_pixel_shader = constant_buffer(device, sizeof(float) * parallax_occlusion_mapping::cb_pixel_shader_component_count);

	init_shader();
	init_geometry();
	init_pipeline_state();

	init_cube_envmap_data();
	init_irradiance_map();
}

void pbr::init_cube_envmap_data()
{
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/cube_envmap.hlsl");
	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.pixel_shader_entry_point = "ps_main";
	cube_envmap_shader = hlsl_shader(device, shader_desc);

	cb_buffer_cube_envmap = constant_buffer(device, sizeof(float4x4));

	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = true;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	HRESULT hr = device->CreateDepthStencilState(&ds_desc, &cube_envmap_depth_stencil_state_.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_FRONT;
	rastr_desc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&rastr_desc, &cube_envmap_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void pbr::init_geometry()
{
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/bunny.obj");
	auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/sphere_64x32_1.obj");
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/rect_2x2.obj");
	assert(model.mesh_count() == 1);
	index_count = UINT(model.meshes()[0].index_count);

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = UINT(model.vertex_data_byte_count());
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.vertex_data().data();
	HRESULT hr = device->CreateBuffer(&vb_desc, &vb_data, &vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = UINT(model.index_data_byte_count());
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = model.index_data().data();
	hr = device->CreateBuffer(&ib_desc, &ib_data, &index_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	using Vf = decltype(model)::Format;
	vertex_stride = Vf::vertex_byte_count;
	D3D11_INPUT_ELEMENT_DESC layout_desc[4] = {
		{ "VERT_POSITION_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::position_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_NORMAL_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::normal_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, Vf::tex_coord_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_TANGENT_SPACE_MS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, Vf::tangent_space_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device->CreateInputLayout(layout_desc, std::extent<decltype(layout_desc)>::value,
		shader.vertex_shader_bytecode->GetBufferPointer(),
		shader.vertex_shader_bytecode->GetBufferSize(),
		&input_layout.ptr);
	assert(hr == S_OK);
}

void pbr::init_irradiance_map()
{
	// load an hdr image to the gpu:
	//
	const image_2d hdr_image("../../data/hdr_ice_lake/Ice_Lake_Ref.hdr", 4);
	com_ptr<ID3D11Texture2D> tex_env_map;
	com_ptr<ID3D11ShaderResourceView> tex_envmap_srv;

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
	HRESULT hr = device->CreateTexture2D(&envmap_desc, &env_map_data, &tex_env_map.ptr);
	assert(hr == S_OK);
	hr = device->CreateShaderResourceView(tex_env_map, nullptr, &tex_envmap_srv.ptr);
	assert(hr == S_OK);

	// project hdr image to a cube map:
	//
	com_ptr<ID3D11DepthStencilState> ds_state;
	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = false;
	ds_desc.StencilEnable = false;
	hr = device->CreateDepthStencilState(&ds_desc, &ds_state.ptr);
	assert(hr == S_OK);

	constexpr size_t face_count = 6;
	D3D11_TEXTURE2D_DESC cube_envmap_desc = {};
	cube_envmap_desc.Width = cube_envmap_desc.Height = std::min(hdr_image.size.x, hdr_image.size.y);
	cube_envmap_desc.MipLevels = 1;
	cube_envmap_desc.ArraySize = UINT(face_count);
	cube_envmap_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	cube_envmap_desc.SampleDesc.Count = 1;
	cube_envmap_desc.SampleDesc.Quality = 0;
	cube_envmap_desc.Usage = D3D11_USAGE_DEFAULT;
	cube_envmap_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	cube_envmap_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateTexture2D(&cube_envmap_desc, nullptr, &tex_cube_envmap.ptr);
	assert(hr == S_OK);
	hr = device->CreateShaderResourceView(tex_cube_envmap, nullptr, &tex_cube_envmap_srv.ptr);
	assert(hr == S_OK);

	// gen envmap shader
	auto gen_envmap_shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/gen_envmap.hlsl");
	gen_envmap_shader_desc.vertex_shader_entry_point = "vs_main";
	gen_envmap_shader_desc.pixel_shader_entry_point = "ps_main";
	hlsl_shader gen_envmap_shader = hlsl_shader(device, gen_envmap_shader_desc);

	com_ptr<ID3D11Buffer> cb_vs_gen_envmap = constant_buffer(device, sizeof(float4x4));
	const float4x4 view_matrices[face_count] = {
		view_matrix(float3::zero,	float3(1.0f, 0.0f, 0.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(-1.0f, 0.0f, 0.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(0.0f, -1.0f, 0.0f),	-float3::unit_z),
		view_matrix(float3::zero,	float3(0.0f, 1.0f, 0.0f),	float3::unit_z),
		view_matrix(float3::zero,	float3(0.0f, 0.0f, 1.0f),	-float3::unit_y),
		view_matrix(float3::zero,	float3(0.0f, 0.0f, -1.0f),	-float3::unit_y)
	};

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = float(cube_envmap_desc.Width);
	vp.Height = float(cube_envmap_desc.Height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	const UINT offset = 0;
	device_ctx->IASetInputLayout(nullptr);
	device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_ctx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	device_ctx->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	device_ctx->VSSetShader(gen_envmap_shader.vertex_shader, nullptr, 0);
	device_ctx->VSSetConstantBuffers(0, 1, &cb_vs_gen_envmap.ptr);
	device_ctx->PSSetShader(gen_envmap_shader.pixel_shader, nullptr, 0);
	device_ctx->PSSetShaderResources(0, 1, &tex_envmap_srv.ptr);
	device_ctx->PSSetSamplers(0, 1, &sampler_state.ptr);
	device_ctx->RSSetState(cube_envmap_rasterizer_state);
	device_ctx->RSSetViewports(1, &vp);
	device_ctx->OMSetDepthStencilState(depth_stencil_state, 0);

	for (size_t i = 0; i < face_count; ++i) {
		com_ptr<ID3D11RenderTargetView> tex_cube_envmap_rtv;
		D3D11_RENDER_TARGET_VIEW_DESC env_cube_mad_rtv_desc = {};
		env_cube_mad_rtv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		env_cube_mad_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		env_cube_mad_rtv_desc.Texture2DArray.FirstArraySlice = UINT(i);
		env_cube_mad_rtv_desc.Texture2DArray.ArraySize = 1;
		hr = device->CreateRenderTargetView(tex_cube_envmap, &env_cube_mad_rtv_desc, &tex_cube_envmap_rtv.ptr);
		assert(hr == S_OK);
		device_ctx->OMSetRenderTargets(1, &tex_cube_envmap_rtv.ptr, nullptr);
		hr = debug->ValidateContext(device_ctx);
		assert(hr == S_OK);

		const float4x4 proj_view_matrix = view_matrices[i];
		device_ctx->UpdateSubresource(cb_vs_gen_envmap, 0, nullptr, &proj_view_matrix.m00, 0, 0);
		device_ctx->Draw(cube_index_count_, 0);
	}
}

void pbr::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = true;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;
	HRESULT hr = device->CreateDepthStencilState(&ds_desc, &depth_stencil_state.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_BACK;
	rastr_desc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&rastr_desc, &rasterizer_state.ptr);
	assert(hr == S_OK);

	D3D11_TEXTURE2D_DESC debug_desc = {};
	debug_desc.Width = UINT(rhi_ctx.viewport().Width);
	debug_desc.Height = UINT(rhi_ctx.viewport().Height);
	debug_desc.MipLevels = 1;
	debug_desc.ArraySize = 1;
	debug_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	debug_desc.SampleDesc.Count = 1;
	debug_desc.SampleDesc.Quality = 0;
	debug_desc.Usage = D3D11_USAGE_DEFAULT;
	debug_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	hr = device->CreateTexture2D(&debug_desc, nullptr, &tex_debug.ptr);
	assert(hr == S_OK);

	hr = device->CreateRenderTargetView(tex_debug, nullptr, &tex_debug_rtv.ptr);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_state.ptr);
	assert(hr == S_OK);
}

void pbr::init_shader()
{
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/pbr/pbr.hlsl");
	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.pixel_shader_entry_point = "ps_main";
	shader = hlsl_shader(device, shader_desc);
}

void pbr::on_mouse_move()
{
	const uint2 p = _app_ctx.mouse.position();
	const float2 curr_pos(float(p.x), float(p.y));
	const float2 diff = curr_pos - prev_mouse_position;
	prev_mouse_position = curr_pos;

	if (!_app_ctx.mouse.middle_down() || diff == float2::zero) return;

	const bool y_offset_satisfies = !approx_equal(diff.y, 0.0f, 0.01f);
	const bool x_offset_satisfies = !approx_equal(diff.x, 0.0f, 0.01f);

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies)
		view_roll_angles.y += (diff.x > 0.0f) ? -pi_128 : pi_128;

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies)
		view_roll_angles.x += (diff.y > 0.0f) ? pi_128 : -pi_128;
}

void pbr::on_window_resize()
{
	update_projection_matrix();
}

void pbr::render(float interpolation_factor)
{
	auto viewpoint = lerp(curr_viewpoint, prev_viewpoint, interpolation_factor);
	viewpoint.up = normalize(viewpoint.up);
	update_cb_vertex_shader(viewpoint);

	//init_irradiance_map();

	const UINT offset = 0;
	device_ctx->RSSetViewports(1, &rhi_ctx.viewport());
	ID3D11RenderTargetView* rtv_list[2] = { rhi_ctx.rtv_window(), tex_debug_rtv.ptr };
	device_ctx->OMSetRenderTargets(2, rtv_list, rhi_ctx.dsv_depth_stencil());
	device_ctx->ClearRenderTargetView(rhi_ctx.rtv_window(), &float4::unit_xyzw.x);
	device_ctx->ClearDepthStencilView(rhi_ctx.dsv_depth_stencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// render a model:
	//
	device_ctx->RSSetState(rasterizer_state);
	device_ctx->OMSetDepthStencilState(depth_stencil_state, 0);
	// input assembly
	device_ctx->IASetInputLayout(input_layout);
	device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_ctx->IASetVertexBuffers(0, 1, &vertex_buffer.ptr, &vertex_stride, &offset);
	device_ctx->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	// vertex & pixel shaders
	device_ctx->VSSetShader(shader.vertex_shader, nullptr, 0);
	device_ctx->VSSetConstantBuffers(0, 1, &cb_vertex_shader.ptr);
	device_ctx->PSSetShader(shader.pixel_shader, nullptr, 0);
	
	HRESULT hr = debug->ValidateContext(device_ctx);
	assert(hr == S_OK);
	device_ctx->DrawIndexed(index_count, 0, 0);

	// render cube envmap:
	//
	device_ctx->RSSetState(cube_envmap_rasterizer_state);
	device_ctx->OMSetDepthStencilState(cube_envmap_depth_stencil_state_, 0);
	// input assembly
	device_ctx->IASetInputLayout(nullptr);
	device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_ctx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	device_ctx->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	// vertex & pixel shaders
	device_ctx->VSSetShader(cube_envmap_shader.vertex_shader, nullptr, 0);
	device_ctx->VSSetConstantBuffers(0, 1, &cb_buffer_cube_envmap.ptr);
	device_ctx->PSSetShader(cube_envmap_shader.pixel_shader, nullptr, 0);
	device_ctx->PSSetShaderResources(0, 1, &tex_cube_envmap_srv.ptr);
	device_ctx->PSSetSamplers(0, 1, &sampler_state.ptr);
	hr = debug->ValidateContext(device_ctx);
	assert(hr == S_OK);
	device_ctx->Draw(cube_index_count_, 0);

	prev_viewpoint = curr_viewpoint;
}

void pbr::update(float dt_msec)
{
	if (view_roll_angles != float2::zero) {
		float dist = distance(curr_viewpoint);
		float3 ox = cross(forward(curr_viewpoint), curr_viewpoint.up);
		ox.y = 0.0f; // ox is always parallel the world's OX.
		ox = normalize(ox);

		if (!approx_equal(view_roll_angles.y, 0.0f)) {
			const quat q = from_axis_angle_rotation(float3::unit_y, view_roll_angles.y);
			curr_viewpoint.position = dist * normalize(rotate(q, curr_viewpoint.position));

			ox = rotate(q, ox);
			ox.y = 0.0f;
			ox = normalize(ox);
		}

		if (!approx_equal(view_roll_angles.x, 0.0f)) {
			const quat q = from_axis_angle_rotation(ox, view_roll_angles.x);
			curr_viewpoint.position = dist * normalize(rotate(q, curr_viewpoint.position));
		}

		curr_viewpoint.up = normalize(cross(ox, forward(curr_viewpoint)));
	}

	view_roll_angles = float2::zero;
}

void pbr::update_cb_vertex_shader(const cg::Viewpoint& viewpoint)
{
	static const float3 light_dir_to_ws = normalize(float3(0, 0, 10));
	const float4x4 model_matrix = trs_matrix(model_position, model_rotation, model_scale);
	const float4x4 normal_matrix = rotation_matrix<float4x4>(model_rotation);
	const float4x4 pvm_matrix = projection_matrix * view_matrix(viewpoint) * model_matrix;

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
	device_ctx->UpdateSubresource(cb_vertex_shader, 0, nullptr, arr, 0, 0);

	const cg::Viewpoint viewpoint_cube_envmap(float3::zero, forward(viewpoint), viewpoint.up);
	const float4x4 cube_pvm_matrix = projection_matrix * view_matrix(viewpoint_cube_envmap);
	device_ctx->UpdateSubresource(cb_buffer_cube_envmap, 0, nullptr, &cube_pvm_matrix.m00, 0, 0);
}

void pbr::update_projection_matrix()
{
	projection_matrix = perspective_matrix_directx(pi_3, rhi_ctx.viewport_aspect_ratio(), 0.1f, 1000.0f);
}

} // namespace pbr
