#include "cg/file/file.h"


namespace cg {
namespace file {

// ----- File -----

File::File() noexcept : _handle(nullptr) {}

File::File(const std::string& filename) : File(filename.c_str()) {}

File::File(const char* filename)
	: _handle(nullptr)
{
	open(filename);
}

File::File(File&& f) noexcept : 
	_filename(std::move(f._filename)), 
	_handle(f._handle)
{
	f._handle = nullptr;
}

File::~File() noexcept
{
	close();
}

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
	enforce<File_exception>(_handle, "Failed to open a file.");
	_filename = filename;
}
#pragma warning(pop)

bool File::read_byte(void* buff) const
{
	assert(buff);
	enforce<File_exception>(_handle, "Invalid operation. File is not open.");

	int res = std::fgetc(_handle);
	if (res == EOF) return false;

	*(reinterpret_cast<unsigned char*>(buff)) = static_cast<unsigned char>(res);
	return true;
}

size_t File::read_bytes(void* buff, size_t byte_count) const
{
	assert(buff);
	assert(byte_count);
	enforce<File_exception>(_handle, "Invalid operation. File is not open.");

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

} // namespace file
} // namespace cg