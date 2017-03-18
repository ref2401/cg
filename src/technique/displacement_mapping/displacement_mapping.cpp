#include "technique/displacement_mapping/displacement_mapping.h"

#include <iostream>
#include "cg/data/image.h"
#include "cg/data/model.h"

using namespace cg;
using namespace cg::data;
using namespace cg::rnd::dx11;


namespace displacement_mapping {

// ----- Displacement_mapping -----

Displacement_mapping::Displacement_mapping(const cg::sys::App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx)
	: Example(app_ctx, rhi_ctx),
	_rhi_ctx(dynamic_cast<cg::rnd::dx11::DX11_rhi_context&>(rhi_ctx)),
	_device(_rhi_ctx.device()),
	_debug(_rhi_ctx.debug()),
	_device_ctx(_rhi_ctx.device_ctx()),
	_curr_viewpoint(float3(0, 0, 7), float3::zero, float3::unit_y),
	_prev_viewpoint(_curr_viewpoint)
{
	update_projection_matrix();
	_model_matrix = ts_matrix(float3(0.0f, -0.5f, 0.0f), float3(2.0f));

	init_cbuffers();
	init_shaders();
	init_geometry();
	init_pipeline_state();
	init_textures();
	setup_pipeline_state();
}

void Displacement_mapping::init_cbuffers()
{
	_cb_matrix = constant_buffer(_device, sizeof(mat4));
}

void Displacement_mapping::init_geometry()
{
	auto model = load_model<Vertex_attribs::p_n_tc_ts>("../data/sphere-20x20.obj");
	assert(model.mesh_count() == 1);
	_index_count = UINT(model.meshes()[0].index_count);

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = model.vertex_data_byte_count();
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model.vertex_data().data();
	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = model.index_data_byte_count();
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
		_pom_shader.vertex_shader_bytecode()->GetBufferPointer(),
		_pom_shader.vertex_shader_bytecode()->GetBufferSize(),
		&_pom_input_layout.ptr);
	assert(hr == S_OK);
}

void Displacement_mapping::init_pipeline_state()
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

void Displacement_mapping::init_shaders()
{
	Hlsl_shader_desc pom_shader_desc = load_hlsl_shader_set_desc(
		"../data/displacement_mapping/parallax_displacement_mapping.hlsl");
	pom_shader_desc.vertex_shader_entry_point = "vs_main";
	pom_shader_desc.pixel_shader_entry_point = "ps_main";
	_pom_shader = Hlsl_shader(_device, pom_shader_desc);
}

void Displacement_mapping::init_textures()
{
	Image_2d diffuse_rgb("../data/displacement_mapping/rocks-diffuse.jpg", 4);

	D3D11_TEXTURE2D_DESC diffuse_desc = {};
	diffuse_desc.Width = diffuse_rgb.size().width;
	diffuse_desc.Height = diffuse_rgb.size().height;
	diffuse_desc.MipLevels = 1;
	diffuse_desc.ArraySize = 1;
	diffuse_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	diffuse_desc.SampleDesc.Count = 1;
	diffuse_desc.SampleDesc.Quality = 0;
	diffuse_desc.Usage = D3D11_USAGE_IMMUTABLE;
	diffuse_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA diffuse_data = {};
	diffuse_data.pSysMem = diffuse_rgb.data();
	diffuse_data.SysMemPitch = diffuse_rgb.size().width * byte_count(diffuse_rgb.pixel_format());
	HRESULT hr = _device->CreateTexture2D(&diffuse_desc, &diffuse_data, &_tex_diffuse_rgb.ptr);
	assert(hr == S_OK);

	hr = _device->CreateShaderResourceView(_tex_diffuse_rgb.ptr, nullptr, &_tex_srv_diffuse_rgb.ptr);
	assert(hr == S_OK);
}

void Displacement_mapping::on_mouse_move()
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

void Displacement_mapping::on_window_resize()
{
	update_projection_matrix();
}

void Displacement_mapping::render(float interpolation_factor)
{
	const mat4 view_matrix = cg::view_matrix(_curr_viewpoint, _prev_viewpoint, interpolation_factor);
	const mat4 pvm_matrix = _projection_matrix * view_matrix * _model_matrix;
	float arr[16];
	to_array_column_major_order(pvm_matrix, arr);
	_device_ctx->UpdateSubresource(_cb_matrix.ptr, 0, nullptr, arr, 0, 0);

	_device_ctx->ClearRenderTargetView(_rhi_ctx.rtv_window(), float4::unit_xyzw.data);
	_device_ctx->ClearDepthStencilView(_rhi_ctx.dsv_depth_stencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	_device_ctx->DrawIndexed(_index_count, 0, 0);

	_prev_viewpoint = _curr_viewpoint;
}

void Displacement_mapping::setup_pipeline_state()
{
	const UINT offset = 0;
	_device_ctx->IASetInputLayout(_pom_input_layout.ptr);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &_vertex_stride, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);

	_device_ctx->VSSetShader(_pom_shader.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_cb_matrix.ptr);
	_device_ctx->PSSetShader(_pom_shader.pixel_shader(), nullptr, 0);
	_device_ctx->PSSetShaderResources(0, 1, &_tex_srv_diffuse_rgb.ptr);

	_device_ctx->RSSetState(_rasterizer_state.ptr);
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Displacement_mapping::update(float dt_msec) 
{
	if (_view_roll_angles != float2::zero) {
		float dist = _curr_viewpoint.distance();
		float3 ox = cross(_curr_viewpoint.forward(), _curr_viewpoint.up);
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

		_curr_viewpoint.up = normalize(cross(ox, _curr_viewpoint.forward()));
	}

	_view_roll_angles = float2::zero;
}

void Displacement_mapping::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_directx(pi_3, _rhi_ctx.viewport_aspect_ratio(), 0.1f, 1000.0f);
}

} // displacement_mapping
