#include "technique/pbr/pbr.h"

#include <type_traits>
#include "cg/data/model.h"

using namespace cg::data;


namespace pbr {

pbr::pbr(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx)
	: example(app_ctx, rhi_ctx),
	rhi_ctx_(dynamic_cast<cg::rnd::dx11::dx11_rhi_context&>(rhi_ctx)),
	device_(rhi_ctx_.device()),
	debug_(rhi_ctx_.debug()),
	device_ctx_(rhi_ctx_.device_ctx()),
	curr_viewpoint_(float3(0.0f, 0, 12.0f), float3::zero, float3::unit_y)
{
	update_projection_matrix();
	model_position_ = float3::zero;
	model_scale_ = float3(3.0f);

	cb_vertex_shader_ = constant_buffer(device_, sizeof(float) * pbr::cb_vertex_shader_component_count);
	//cb_pixel_shader_ = constant_buffer(device_, sizeof(float) * parallax_occlusion_mapping::cb_pixel_shader_component_count);

	init_shader();
	init_geometry();
	init_pipeline_state();

	setup_pipeline_state();
}

void pbr::init_geometry()
{
	auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/bunny.obj");
	//auto model = load_model<vertex_attribs::p_n_tc_ts>("../../data/models/sphere_64x32.obj");
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
	auto viewpoint = lerp(curr_viewpoint_, prev_viewpoint_, interpolation_factor);
	viewpoint.up = normalize(viewpoint.up);
	update_cb_vertex_shader(viewpoint);

	device_ctx_->ClearRenderTargetView(rhi_ctx_.rtv_window(), &float4::unit_xyzw.x);
	device_ctx_->ClearDepthStencilView(rhi_ctx_.dsv_depth_stencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	device_ctx_->DrawIndexed(index_count_, 0, 0);

	prev_viewpoint_ = curr_viewpoint_;
}

void pbr::setup_pipeline_state()
{
	const UINT offset = 0;
	device_ctx_->IASetInputLayout(input_layout_);
	device_ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_ctx_->IASetVertexBuffers(0, 1, &vertex_buffer_.ptr, &vertex_stride_, &offset);
	device_ctx_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);

	device_ctx_->VSSetShader(shader_.vertex_shader, nullptr, 0);
	device_ctx_->VSSetConstantBuffers(0, 1, &cb_vertex_shader_.ptr);
	device_ctx_->PSSetShader(shader_.pixel_shader, nullptr, 0);
	//device_ctx_->PSSetConstantBuffers(0, 1, &_cb_pixel_shader.ptr);
	//device_ctx_->PSSetSamplers(0, 1, &_sampler_state.ptr);
	/*ID3D11ShaderResourceView* views[3];
	put_shader_recource_views(views, *_curr_material);
	_device_ctx->PSSetShaderResources(0, 3, views);*/

	device_ctx_->RSSetState(rasterizer_state_);
	device_ctx_->OMSetDepthStencilState(depth_stencil_state_, 0);

	HRESULT hr = debug_->ValidateContext(device_ctx_);
	assert(hr == S_OK);
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
	static const float3 light_dir_to_ws = normalize(float3(10, 10, 10));
	const float4x4 model_matrix = ts_matrix(model_position_, model_scale_);
	const float4x4 normal_matrix = float4x4::identity;
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
