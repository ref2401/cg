#include "cg/file/file.h"

#include <cassert>


namespace cg {
namespace file {

File::File(const std::string& filename) : File(filename.c_str()) {}

#pragma warning(push)
#pragma warning(disable:4996)
File::File(const char* filename)
	: _filename(filename), _file_handle(nullptr)
{
	assert(filename);

	_file_handle = std::fopen(filename, "rb");
	enforce<File_exception>(_file_handle, "Failed to open a file.");
}
#pragma warning(pop)

File::File(File&& f) noexcept : 
	_filename(std::move(f._filename)), 
	_file_handle(f._file_handle)
{
	f._file_handle = nullptr;
}

File::~File() noexcept
{
	if (!_file_handle) return;

	fclose(_file_handle);
	_file_handle = nullptr;
}

File& File::operator=(File&& f) noexcept
{
	if (_file_handle)
		fclose(_file_handle);

	_filename = std::move(f._filename);
	_file_handle = f._file_handle;
	f._file_handle = nullptr;

	return *this;
}


File_exception::File_exception(const std::string& msg) : std::runtime_error(msg) {}
File_exception::File_exception(const char* msg) : std::runtime_error(msg) {}
File_exception::~File_exception() {}

} // namespace file
} // namespace cg