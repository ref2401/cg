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
	init_shaders();
	init_textures();
}

void Gaussian_filter_example::init_shaders()
{
	auto compute_desc = cg::data::load_hlsl_compute_desc("../data/learn_dx11/image_processing/gaussian_filter.compute.hlsl");
	compute_desc.compute_shader_entry_point = "cs_main";
	_gaussian_filter_compute = Hlsl_compute(_device, compute_desc);
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

	D3D11_TEXTURE2D_DESC filter_desc = source_desc;
	filter_desc.Usage = D3D11_USAGE_DEFAULT;
	hr = _device->CreateTexture2D(&filter_desc, nullptr, &_tex_intermidiate.ptr);
	assert(hr == S_OK);
	hr = _device->CreateTexture2D(&filter_desc, nullptr, &_tex_final.ptr);
	assert(hr == S_OK);
}

void Gaussian_filter_example::render()
{
}

} // namespace image_processing
} // namespace learn_dx11
