#include "learn_dx11/tess/terrain_tessellation_example.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>
#include "cg/base/container.h"

using namespace cg;


namespace {

class Vertex_index_helper final {
public:

	Vertex_index_helper(size_t row_count, size_t column_count) noexcept
		: _row_count(int64_t(row_count)), _column_count(int64_t(column_count))
	{
		assert(row_count > 0);
		assert(column_count > 0);
	}


	uint32_t bottom_left_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row, int64_t(0), _row_count);
		int64_t c = cg::clamp(column, int64_t(0), _column_count);

		return uint32_t(r * _row_count + c);
	}

	uint32_t bottom_right_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row, int64_t(0), _row_count);
		int64_t c = cg::clamp(column + 1, int64_t(0), _column_count);

		return uint32_t(r * _row_count + c);
	}

	uint32_t top_left_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row + 1, int64_t(0), _row_count);
		int64_t c = cg::clamp(column, int64_t(0), _column_count);

		return uint32_t(r * _row_count + c);
	}

	uint32_t top_right_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row + 1, int64_t(0), _row_count);
		int64_t c = cg::clamp(column + 1, int64_t(0), _column_count);

		return uint32_t(r * _row_count + c);
	}

private:

	int64_t _row_count = 0;
	int64_t _column_count = 0;
};

struct Terrain_model final {
	// each vertex has position(float3) and tex_coord(float2) -> 5 components per vertex.
	static constexpr size_t vertex_component_count = 5;


	// each vertex has position(float3) and tex_coord(float2)
	std::vector<float> vertex_attribs;
	
	// 12 indices for each patch. 
	// 4 stands for the cell itselft (bottom left, bottom right, top left, top right)
	// 2 - left cell (bottom left, top left)
	// 2 - bottom cell (bottom left, bottom right)
	// 2 - right cell (bottom right, top right)
	// 2 - top cell (top left, top right)
	std::vector<uint32_t> indices;

	size_t patch_count = 0;
};

Terrain_model make_terrain_model(size_t z_cell_count, size_t x_cell_count)
{
	assert(z_cell_count > 0);
	assert(x_cell_count > 0);


	constexpr size_t patch_index_count = 12;
	const size_t expected_vertex_attrib_count = (z_cell_count + 1) * (x_cell_count + 1) * Terrain_model::vertex_component_count;
	const size_t expected_index_count = z_cell_count * x_cell_count * patch_index_count;
	
	Terrain_model model;
	model.vertex_attribs.reserve(expected_vertex_attrib_count);
	model.indices.reserve(expected_index_count);
	model.patch_count = z_cell_count * x_cell_count;

	// ----- vertices -----
	// positions are in range [(-0.5f, 0.0f, -0.5f), (0.5f, 0.0f, 0.5f)]
	// tex_coords are in range [(0.0, 0.0), (1.0, 1.0)]
	// bottom-left position of the grid is (-0.5f, 0.0f, 0.5f) tex_coords [0.0f, 0.0f]
	// top-right position of the grid is (0.5f, 0.0f, -0.5f) tex_coords [1.0f, 1.0f]
	const size_t z_coord_count = z_cell_count + 1;
	const size_t x_coord_count = x_cell_count + 1;

	for (size_t z = z_coord_count; z > 0; --z) {
		const float z_coord = float(z - 1) / z_cell_count;

		for (size_t x = 0; x < x_coord_count; ++x) {
			const float x_coord = float(x) / x_cell_count;
			const float vert[Terrain_model::vertex_component_count] = {
				x_coord - 0.5f, 0.0f, z_coord - 0.5f,	// position
				x_coord, 1.0f - z_coord					// tex_coord
			};

			model.vertex_attribs.insert(model.vertex_attribs.cend(), vert, vert + Terrain_model::vertex_component_count);
		}
	}

	// ----- patch indices -----
	Vertex_index_helper index_helper(z_cell_count, x_cell_count);

	for (int64_t r = 0; r < z_cell_count; ++r) {
		for (int64_t c = 0; c < x_cell_count; ++c) {

			//std::cout << std::endl << "<" << r << ", " << c << ">" << std::endl << std::endl;

			uint32_t patch_indices[patch_index_count];
			patch_indices[0] = index_helper.bottom_left_index(r, c);
			patch_indices[1] = index_helper.bottom_right_index(r, c);
			patch_indices[2] = index_helper.top_left_index(r, c);
			patch_indices[3] = index_helper.top_right_index(r, c);
			// left cell: 
			patch_indices[4] = index_helper.bottom_left_index(r, c - 1);
			patch_indices[5] = index_helper.top_left_index(r, c - 1);
			// bottom cell:
			patch_indices[6] = index_helper.bottom_left_index(r - 1, c);
			patch_indices[7] = index_helper.bottom_right_index(r - 1, c);
			// right cell:
			patch_indices[8] = index_helper.bottom_right_index(r, c + 1);
			patch_indices[9] = index_helper.top_right_index(r, c + 1);
			// top cell:
			patch_indices[10] = index_helper.top_left_index(r + 1, c);
			patch_indices[11] = index_helper.top_right_index(r + 1, c);

			model.indices.insert(model.indices.cend(), patch_indices, patch_indices + patch_index_count);

			//std::cout << patch_indices[0] << " " << patch_indices[1] << " " 
			//	<< patch_indices[2] << " " << patch_indices[3] << " " << std::endl;

			//std::cout << "left:\t" << patch_indices[4] << " " << patch_indices[5] << std::endl;
			//std::cout << "bottom:\t" << patch_indices[6] << " " << patch_indices[7] << std::endl;
			//std::cout << "right:\t" << patch_indices[8] << " " << patch_indices[9] << std::endl;
			//std::cout << "top: \t" << patch_indices[10] << " " << patch_indices[11] << std::endl;
		}

		std::cout << std::endl;
	}

	assert(model.vertex_attribs.size() == expected_vertex_attrib_count);
	assert(model.indices.size() == expected_index_count);
	return model;
}
} // namespace


namespace learn_dx11 {
namespace tess {


// ----- Terrain_tessellation_example -----

Terrain_tessellation_example::Terrain_tessellation_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	_model_matrix = scale_matrix<mat4>(float3(3.0f));
	_view_matrix = view_matrix(float3(0, 2, 2), float3::zero);
	
	init_cbuffer();
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	init_pipeline_state();

	update_projection_matrix(rnd_ctx.viewport_size().aspect_ratio());
	setup_projection_view_matrix();
	setup_pipeline_state();
}

void Terrain_tessellation_example::init_cbuffer()
{
	_model_cbuffer = make_cbuffer(_device, sizeof(mat4));

	float arr[16];
	to_array_column_major_order(_model_matrix, arr);
	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, arr, 0, 0);

	_projection_view_cbuffer = make_cbuffer(_device, sizeof(mat4));
}

void Terrain_tessellation_example::init_geometry()
{
	Terrain_model terrain_model = make_terrain_model(3, 2);
	_index_count = UINT(terrain_model.indices.size());

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = cg::byte_count(terrain_model.vertex_attribs);
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = terrain_model.vertex_attribs.data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = cg::byte_count(terrain_model.indices);
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = terrain_model.indices.data();

	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	D3D11_INPUT_ELEMENT_DESC layout_desc[2] = {
		{ "V_POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_TEX_COORD" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _device->CreateInputLayout(layout_desc, 2,
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	// default rastr state
	D3D11_RASTERIZER_DESC default_rastr_desc = {};
	default_rastr_desc.FillMode = D3D11_FILL_SOLID;
	default_rastr_desc.CullMode = D3D11_CULL_BACK;
	default_rastr_desc.FrontCounterClockwise = true;
	hr = _device->CreateRasterizerState(&default_rastr_desc, &_default_rasterizer_state.ptr);
	assert(hr == S_OK);

	// wireframe rastr state
	D3D11_RASTERIZER_DESC wireframe_rastr_desc = default_rastr_desc;
	wireframe_rastr_desc.FillMode = D3D11_FILL_WIREFRAME;
	hr = _device->CreateRasterizerState(&wireframe_rastr_desc, &_wireframe_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::init_shaders()
{
	auto shader_desc = cg::data::load_hlsl_shader_set_desc(
		"../data/learn_dx11/tess/terrain_tessellation.hlsl");

	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.hull_shader_entry_point = "hs_main";
	shader_desc.domain_shader_entry_point = "ds_main";
	shader_desc.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, shader_desc);
}

void Terrain_tessellation_example::on_viewport_resize(const uint2& viewport_size)
{
	update_projection_matrix(viewport_size.aspect_ratio());
	setup_projection_view_matrix();
}

void Terrain_tessellation_example::render()
{
	static const float4 clear_color = cg::rgba(0xd1d7ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->DrawIndexed(_index_count, 0, 0);
}

void Terrain_tessellation_example::setup_pipeline_state()
{
	// input assembler
	constexpr size_t offset = 0;
	constexpr size_t vertex_byte_count = sizeof(float) * Terrain_model::vertex_component_count;
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &vertex_byte_count, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);

	// vertex shader
	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_model_cbuffer.ptr);
	// hull shader
	_device_ctx->HSSetShader(_shader_set.hull_shader(), nullptr, 0);
	// domain shader
	_device_ctx->DSSetShader(_shader_set.domain_shader(), nullptr, 0);
	_device_ctx->DSSetConstantBuffers(0, 1, &_projection_view_cbuffer.ptr);
	// pixel shader
	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	_device_ctx->RSSetState(_wireframe_rasterizer_state.ptr);
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 0);


	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::setup_projection_view_matrix()
{
	const mat4 mat = _projection_matrix * _view_matrix;

	float arr[16];
	to_array_column_major_order(mat, arr);
	_device_ctx->UpdateSubresource(_projection_view_cbuffer.ptr, 0, nullptr, arr, 0, 0);
}

void Terrain_tessellation_example::update_projection_matrix(float aspect_ratio)
{
	assert(aspect_ratio > 0.0f);

	_projection_matrix = cg::perspective_matrix_directx(cg::pi_3, aspect_ratio, 0.1f, 100.0f);
}

} // namespace tess
} // namespace learn_dx11
