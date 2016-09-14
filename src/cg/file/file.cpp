#include "cg/file/file.h"


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

bool File::read_byte(void* buff)
{
	assert(buff);
	enforce<File_exception>(_file_handle, "Invalid operation. Can't read from nullptr file handle.");

	int res = std::fgetc(_file_handle);
	if (res == EOF) return false;

	*(reinterpret_cast<unsigned char*>(buff)) = static_cast<unsigned char>(res);
	return true;
}

size_t File::read_bytes(void* buff, size_t byte_count)
{
	assert(buff);
	assert(byte_count);
	enforce<File_exception>(_file_handle, "Invalid operation. Can't read from nullptr file handle.");

	return fread(buff, sizeof(unsigned char), byte_count, _file_handle);
}

File_exception::File_exception(const std::string& msg) : std::runtime_error(msg) {}
File_exception::File_exception(const char* msg) : std::runtime_error(msg) {}
File_exception::~File_exception() {}

} // namespace file
} // namespace cg