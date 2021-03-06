#include "technique/parallax_occlusion_mapping/parallax_occlusion_mapping.h"

#include "cg/data/image.h"
#include "cg/data/model.h"

using namespace cg;
using namespace cg::data;
using namespace cg::rnd::dx11;


namespace {

using parallax_occlusion_mapping::Material;

void put_shader_recource_views(ID3D11ShaderResourceView** arr, const Material& material) noexcept
{
	assert(material.tex_srv_diffuse_rgb);
	assert(material.tex_srv_height_map);
	assert(material.tex_srv_normal_map);

	arr[0] = material.tex_srv_diffuse_rgb.ptr;
	arr[1] = material.tex_srv_height_map.ptr;
	arr[2] = material.tex_srv_normal_map.ptr;
}

} // namespace


namespace parallax_occlusion_mapping {

// ----- Material -----

Material::Material(ID3D11Device* device, float tex_coord_step_scale, float min_sample_count,
	float max_sample_count, float self_shadowing_factor, const char* diffuse_rgb_filename, 
	const char* height_map_filename, const char* normal_map_height)
	: tex_coord_step_scale(tex_coord_step_scale), min_sample_count(min_sample_count),
		max_sample_count(max_sample_count), self_shadowing_factor(self_shadowing_factor)
{
	assert(device);
	assert(diffuse_rgb_filename);
	assert(height_map_filename);
	assert(normal_map_height);

	// diffuse rgb
	{
		image_2d image(diffuse_rgb_filename, 4, true);

		D3D11_TEXTURE2D_DESC diffuse_desc = {};
		diffuse_desc.Width = image.size.x;
		diffuse_desc.Height = image.size.y;
		diffuse_desc.MipLevels = 1;
		diffuse_desc.ArraySize = 1;
		diffuse_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		diffuse_desc.SampleDesc.Count = 1;
		diffuse_desc.SampleDesc.Quality = 0;
		diffuse_desc.Usage = D3D11_USAGE_IMMUTABLE;
		diffuse_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		D3D11_SUBRESOURCE_DATA diffuse_data = {};
		diffuse_data.pSysMem = image.data;
		diffuse_data.SysMemPitch = UINT(image.size.x * byte_count(image.pixel_format));
		HRESULT hr = device->CreateTexture2D(&diffuse_desc, &diffuse_data, &tex_diffuse_rgb.ptr);
		assert(hr == S_OK);

		hr = device->CreateShaderResourceView(tex_diffuse_rgb, nullptr, &tex_srv_diffuse_rgb.ptr);
		assert(hr == S_OK);
	}

	// height map
	{
		image_2d image(height_map_filename, 1, true);

		D3D11_TEXTURE2D_DESC displ_desc = {};
		displ_desc.Width = image.size.x;
		displ_desc.Height = image.size.y;
		displ_desc.MipLevels = 1;
		displ_desc.ArraySize = 1;
		displ_desc.Format = DXGI_FORMAT_R8_UNORM;
		displ_desc.SampleDesc.Count = 1;
		displ_desc.SampleDesc.Quality = 0;
		displ_desc.Usage = D3D11_USAGE_IMMUTABLE;
		displ_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		D3D11_SUBRESOURCE_DATA displ_data = {};
		displ_data.pSysMem = image.data;
		displ_data.SysMemPitch = UINT(image.size.x * byte_count(image.pixel_format));
		HRESULT hr = device->CreateTexture2D(&displ_desc, &displ_data, &tex_height_map.ptr);
		assert(hr == S_OK);

		hr = device->CreateShaderResourceView(tex_height_map, nullptr, &tex_srv_height_map.ptr);
		assert(hr == S_OK);
	}

	// normal map
	{
		image_2d image(normal_map_height, 4, true);

		D3D11_TEXTURE2D_DESC normal_desc = {};
		normal_desc.Width = image.size.x;
		normal_desc.Height = image.size.y;
		normal_desc.MipLevels = 1;
		normal_desc.ArraySize = 1;
		normal_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		normal_desc.SampleDesc.Count = 1;
		normal_desc.SampleDesc.Quality = 0;
		normal_desc.Usage = D3D11_USAGE_IMMUTABLE;
		normal_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		D3D11_SUBRESOURCE_DATA normal_data = {};
		normal_data.pSysMem = image.data;
		normal_data.SysMemPitch = UINT(image.size.x * byte_count(image.pixel_format));
		HRESULT hr = device->CreateTexture2D(&normal_desc, &normal_data, &tex_normal_map.ptr);
		assert(hr == S_OK);

		hr = device->CreateShaderResourceView(tex_normal_map, nullptr, &tex_srv_normal_map.ptr);
		assert(hr == S_OK);
	}
}

// ----- parallax_occlusion_mapping -----

parallax_occlusion_mapping::parallax_occlusion_mapping(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx)
	: example(app_ctx, rhi_ctx),
	_rhi_ctx(dynamic_cast<cg::rnd::dx11::dx11_rhi_context&>(rhi_ctx)),
	_device(_rhi_ctx.device()),
	_debug(_rhi_ctx.debug()),
	_device_ctx(_rhi_ctx.device_ctx()),
	_curr_viewpoint(float3(-1.0f, 0, 6.0f), float3::zero, float3::unit_y),
	_prev_viewpoint(_curr_viewpoint),
	_dlight_position_ws(100.0f, 100.0f, 100.0f),
	_dlight_velocity_ws(0.0f, 0.0f, 0.0f)
{
	update_projection_matrix();
	_model_position = float3::zero;
	_model_scale = float3(3.0f);

	int_cbuffers();
	init_shaders();
	init_geometry();
	init_pipeline_state();
	init_materials();

	setup_pipeline_state();
}

void parallax_occlusion_mapping::int_cbuffers()
{
	_cb_vertex_shader = constant_buffer(_device, sizeof(float) * parallax_occlusion_mapping::cb_vertex_shader_component_count);
	_cb_pixel_shader = constant_buffer(_device, sizeof(float) * parallax_occlusion_mapping::cb_pixel_shader_component_count);
}

void parallax_occlusion_mapping::init_geometry()
{
	auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/rect_1x1.obj");
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/bunny.obj");
	assert(model.mesh_count() == 1);
	_index_count = UINT(model.meshes()[0].index_count);

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = UINT(model.vertex_data_byte_count());
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.vertex_data().data();
	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = UINT(model.index_data_byte_count());
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = model.index_data().data();
	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	using Vf = decltype(model)::Format;
	_vertex_stride = Vf::vertex_byte_count;
	D3D11_INPUT_ELEMENT_DESC layout_desc[4] = {
		{ "VERT_POSITION_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::position_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_NORMAL_MS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Vf::normal_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "VERT_TEX_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, Vf::tex_coord_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VERT_TANGENT_SPACE_MS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, Vf::tangent_space_byte_offset, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = _device->CreateInputLayout(layout_desc, 4,
		_pom_shader.vertex_shader_bytecode->GetBufferPointer(),
		_pom_shader.vertex_shader_bytecode->GetBufferSize(),
		&_pom_input_layout.ptr);
	assert(hr == S_OK);
}

void parallax_occlusion_mapping::init_materials()
{
	_rock_wall_material = Material(_device, 0.1f, 4.0f, 32.0f, 0.9f,
		"../../data/parallax_occlusion_mapping/rocks-diffuse.jpg",
		"../../data/parallax_occlusion_mapping/rocks-displacement.jpg",
		"../../data/parallax_occlusion_mapping/rocks-normal.jpg");

	_four_shapes_material = Material(_device, 0.1f, 4.0f, 32.0f, 0.9f,
		"../../data/parallax_occlusion_mapping/four_shapes_diffuse_rgb.jpg",
		"../../data/parallax_occlusion_mapping/four_shapes_height_map.png",
		"../../data/parallax_occlusion_mapping/four_shapes_normal_map.png");


	_curr_material = &_rock_wall_material;

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HRESULT hr = _device->CreateSamplerState(&sampler_desc, &_sampler_state.ptr);
	assert(hr == S_OK);
}

void parallax_occlusion_mapping::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC ds_desc = {};
	ds_desc.DepthEnable = true;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;
	HRESULT hr = _device->CreateDepthStencilState(&ds_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rastr_desc = {};
	rastr_desc.FillMode = D3D11_FILL_SOLID;
	rastr_desc.CullMode = D3D11_CULL_BACK;
	rastr_desc.FrontCounterClockwise = true;
	hr = _device->CreateRasterizerState(&rastr_desc, &_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void parallax_occlusion_mapping::init_shaders()
{
	hlsl_shader_desc pom_shader_desc = load_hlsl_shader_set_desc(
		"../../data/parallax_occlusion_mapping/parallax_occlusion_mapping.hlsl");
	pom_shader_desc.vertex_shader_entry_point = "vs_main";
	pom_shader_desc.pixel_shader_entry_point = "ps_main";
	_pom_shader = hlsl_shader(_device, pom_shader_desc);
}

void parallax_occlusion_mapping::on_mouse_move()
{
	const uint2 p = _app_ctx.mouse.position();
	const float2 curr_pos(float(p.x), float(p.y));
	const float2 diff = curr_pos - _prev_mouse_position;
	_prev_mouse_position = curr_pos;

	if (!_app_ctx.mouse.middle_down() || diff == float2::zero) return;

	const bool y_offset_satisfies = !approx_equal(diff.y, 0.0f, 0.01f);
	const bool x_offset_satisfies = !approx_equal(diff.x, 0.0f, 0.01f);

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies)
		_view_roll_angles.y += (diff.x > 0.f) ? -pi_128 : pi_128;

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies)
		_view_roll_angles.x += (diff.y > 0.f) ? pi_128 : -pi_128;
}

void parallax_occlusion_mapping::on_window_resize()
{
	update_projection_matrix();
}

void parallax_occlusion_mapping::render(float interpolation_factor)
{
	auto viewpoint = lerp(_curr_viewpoint, _prev_viewpoint, interpolation_factor);
	viewpoint.up = normalize(viewpoint.up);
	update_cb_vertex_shader(viewpoint);

	_device_ctx->ClearRenderTargetView(_rhi_ctx.rtv_window(), &float4::unit_xyzw.x);
	_device_ctx->ClearDepthStencilView(_rhi_ctx.dsv_depth_stencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	_device_ctx->DrawIndexed(_index_count, 0, 0);

	_prev_viewpoint = _curr_viewpoint;
}

void parallax_occlusion_mapping::update_cb_vertex_shader(const Viewpoint& viewpoint)
{
	const float3 dlight_dir_ws = normalize(-_dlight_position_ws);
	const float4x4 model_matrix = ts_matrix(_model_position, _model_scale);
	const float4x4 normal_matrix = float4x4::identity;
	const float4x4 projection_view_matrix = _projection_matrix * view_matrix(viewpoint);
	
	float arr[parallax_occlusion_mapping::cb_vertex_shader_component_count];
	to_array_column_major_order(projection_view_matrix, arr);
	to_array_column_major_order(model_matrix, arr + 16);
	to_array_column_major_order(normal_matrix, arr + 32);
	arr[48] = viewpoint.position.x;
	arr[49] = viewpoint.position.y;
	arr[50] = viewpoint.position.z;
	arr[51] = _curr_material->tex_coord_step_scale;
	arr[52] = _dlight_position_ws.x;
	arr[53] = _dlight_position_ws.y;
	arr[54] = _dlight_position_ws.z;
	arr[55] = 0.0f;
	arr[56] = dlight_dir_ws.x;
	arr[57] = dlight_dir_ws.y;
	arr[58] = dlight_dir_ws.z;
	arr[59] = 0.0f;
	_device_ctx->UpdateSubresource(_cb_vertex_shader, 0, nullptr, arr, 0, 0);
}

void parallax_occlusion_mapping::update_cb_pixel_shader()
{
	const float arr[parallax_occlusion_mapping::cb_pixel_shader_component_count] = {
		_curr_material->min_sample_count,
		_curr_material->max_sample_count,
		_curr_material->self_shadowing_factor,
		_curr_material->tex_coord_step_scale
	};
	_device_ctx->UpdateSubresource(_cb_pixel_shader, 0, nullptr, arr, 0, 0);
}

void parallax_occlusion_mapping::setup_pipeline_state()
{
	const UINT offset = 0;
	_device_ctx->IASetInputLayout(_pom_input_layout);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &_vertex_stride, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	_device_ctx->VSSetShader(_pom_shader.vertex_shader, nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_cb_vertex_shader.ptr);
	_device_ctx->PSSetShader(_pom_shader.pixel_shader, nullptr, 0);
	_device_ctx->PSSetConstantBuffers(0, 1, &_cb_pixel_shader.ptr);
	_device_ctx->PSSetSamplers(0, 1, &_sampler_state.ptr);
	ID3D11ShaderResourceView* views[3];
	put_shader_recource_views(views, *_curr_material);
	_device_ctx->PSSetShaderResources(0, 3, views);

	_device_ctx->RSSetState(_rasterizer_state);
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void parallax_occlusion_mapping::update(float dt_msec) 
{
	_dlight_position_ws += _dlight_velocity_ws;
	if (abs(_dlight_position_ws.x) > 100.0f) {
		_dlight_velocity_ws *= -1.0f;
	}

	update_cb_pixel_shader();


	if (_view_roll_angles != float2::zero) {
		float dist = distance(_curr_viewpoint);
		float3 ox = cross(forward(_curr_viewpoint), _curr_viewpoint.up);
		ox.y = 0.0f; // ox is always parallel the world's OX.
		ox = normalize(ox);

		if (!approx_equal(_view_roll_angles.y, 0.0f)) {
			quat q = from_axis_angle_rotation(float3::unit_y, _view_roll_angles.y);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));

			ox = rotate(q, ox);
			ox.y = 0.0f;
			ox = normalize(ox);
		}

		if (!approx_equal(_view_roll_angles.x, 0.0f)) {
			quat q = from_axis_angle_rotation(ox, _view_roll_angles.x);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));
		}

		_curr_viewpoint.up = normalize(cross(ox, forward(_curr_viewpoint)));
	}

	_view_roll_angles = float2::zero;
}

void parallax_occlusion_mapping::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_directx(pi_3, _rhi_ctx.viewport_aspect_ratio(), 0.1f, 1000.0f);
}

} // parallax_occlusion_mapping
