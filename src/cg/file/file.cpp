#include "cg/file/file.h"

#include <cstring>
#include <type_traits>
#include <windows.h>


namespace cg {
namespace file {

// ----- File -----

File& File::operator=(File&& f) noexcept
{
	if (this == &f) return *this;
	
	close();
	_filename = std::move(f._filename);
	_handle = f._handle;
	f._handle = nullptr;

	return *this;
}

void File::close() noexcept
{
	if (!_handle) return;

	_filename.clear();
	fclose(_handle);
	_handle = nullptr;
}

void File::open(const std::string& filename)
{
	open(filename.c_str());
}

#pragma warning(push)
#pragma warning(disable:4996)
void File::open(const char* filename)
{
	close();

	assert(filename);

	_handle = std::fopen(filename, "rb");
	enforce(_handle, EXCEPTION_MSG("Failed to open file:", filename));
	_filename = filename;
}
#pragma warning(pop)

bool File::read_byte(void* buff) const
{
	assert(buff);
	enforce(_handle, EXCEPTION_MSG("Invalid operation. File is not open."));

	int res = std::fgetc(_handle);
	if (res == EOF) return false;

	*(reinterpret_cast<unsigned char*>(buff)) = static_cast<unsigned char>(res);
	return true;
}

size_t File::read_bytes(void* buff, size_t byte_count) const
{
	assert(buff);
	assert(byte_count);
	enforce(_handle, EXCEPTION_MSG("Invalid operation. File is not open."));

	return fread(buff, sizeof(unsigned char), byte_count, _handle);
}

// ----- By_line_iteator -----

const By_line_iterator By_line_iterator::end {};

By_line_iterator::By_line_iterator(const std::string& filename, bool keep_line_feed) 
	: _file(filename), _keep_line_feed(keep_line_feed)
{
	read_next_line();
}

By_line_iterator::By_line_iterator(const char* filename, bool keep_line_feed) 
	: _file(filename), _keep_line_feed(keep_line_feed)
{
	read_next_line();
}

By_line_iterator::By_line_iterator(By_line_iterator&& it) noexcept 
	: _file(std::move(it._file)), 
	_keep_line_feed(it._keep_line_feed), 
	_line_buffer(std::move(it._line_buffer))
{}

By_line_iterator::~By_line_iterator() noexcept
{
	_file.close();
}

By_line_iterator& By_line_iterator::operator=(By_line_iterator&& it) noexcept
{
	_file = std::move(it._file);
	_keep_line_feed = it._keep_line_feed;
	_line_buffer = std::move(it._line_buffer);
	return *this;
}

void By_line_iterator::read_next_line()
{
	_line_buffer.clear();

	if (_file.eof()) {
		_file.close();
		return;
	}

	while (true) {
		char ch;

		bool res = _file.read_byte(&ch);
		if (!res) break;

		if (ch == '\r') continue; // skip the abomination.
		if (ch == '\n') break;

		_line_buffer.push_back(ch);
	}

	if (_keep_line_feed)
		_line_buffer.push_back('\n');
}

// ----- funcs ------

bool exists(const std::string& filename)
{
	assert(filename.size() < MAX_PATH); // :'(
	return (GetFileAttributes(filename.c_str()) != INVALID_FILE_ATTRIBUTES);
}

bool exists(const char* filename)
{
	assert(std::strlen(filename)< MAX_PATH); // :'(
	return (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES);
}

cg::data::Shader_program_source_code load_glsl_program_source(const std::string& filename)
{
	return load_glsl_program_source(filename.c_str());
}

cg::data::Shader_program_source_code load_glsl_program_source(const char* filename)
{
	using cg::data::Shader_program_source_code;

	if (!filename) return Shader_program_source_code {};

	std::string fn;
	Shader_program_source_code src;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");
	if (exists(fn))
		src.vertex_source = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".pixel.glsl");
	if (exists(fn))
		src.pixel_source = load_text(fn);

	return src;
}

std::string load_text(const std::string& filename)
{
	return load_text(filename.c_str());
}

std::string load_text(const char* filename)
{
	File f(filename);

	std::string text;
	
	char buffer[1024];
	while (!f.eof()) {
		size_t actual_bytes = f.read_bytes(buffer, std::extent<decltype(buffer)>::value);
		if (actual_bytes)
			text.append(buffer, actual_bytes);
	}

	return text;
}

} // namespace file
} // namespace cg