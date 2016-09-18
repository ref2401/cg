#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include <cassert>
#include <cstdio>
#include <iterator>
#include <stdexcept>
#include "cg/base/base.h"
#include "cg/data/mesh.h"


namespace cg {
namespace file {

// File_exception class is the only exception class in the cg::file module 
// that is used to indicate that any file related operation has failed.
class File_exception final : std::runtime_error {
public:

	explicit File_exception(const std::string& msg) : std::runtime_error(msg) {}

	explicit File_exception(const char* msg) : std::runtime_error(msg) {}
};

// Handles all files as binary. Implements only read facilities.
class File final {
public:

	File() noexcept 
		: _handle(nullptr) 
	{}

	explicit File(const std::string& filename) 
		: File(filename.c_str()) 
	{}

	explicit File(const char* filename) 
		: _handle(nullptr)
	{
		open(filename);
	}
	
	File(const File& f) = delete;

	File(File&& f) noexcept
		: _filename(std::move(f._filename)), _handle(f._handle)
	{
		f._handle = nullptr;
	}

	~File() noexcept
	{
		close();
	}


	File& operator=(const File& f) = delete;

	File& operator=(File&& f) noexcept;


	// Closes the file and sets filename & handle to empty string and nullptr respectively.
	void close() noexcept;

	// Returns true if the end of the file has been reached.
	// Also returns true if this->handle() equals to nullptr.
	bool eof() const
	{
		enforce<File_exception>(_handle, "Invalid operation. File is not open.");
		return (std::feof(_handle) != 0);
	}

	// Returns the name of the file being processed by this File object.
	const std::string& filename() const noexcept
	{
		return _filename;
	}

	// Returns true if the file has been opened.
	bool is_open() const noexcept
	{
		return (_handle != nullptr);
	}

	void open(const std::string& filename);

	void open(const char* filename);

	// Reads one byte and stores it in buff.
	// Returns: true is the read operation succeeded.
	bool read_byte(void* buff) const;
	
	// Reads a chunk of bytes and stores it in buff.
	// Returns: actual count of bytes read.
	size_t File::read_bytes(void* buff, size_t byte_count) const;

private:
	std::string _filename;
	mutable FILE* _handle;
};

// By_line_iterator is an input iterator that provides ability to read from file one line at a time.
// After construction By_line_iterator already contains the first line from the file.
// Default constructor can be used to create the end iterator. 
// Also By_line_iterator::end static member can be used as the end iterator.
// Each call to operator++ invalidates internal line buffer, so the caller
// must copy its content when retention is needed.
class By_line_iterator final {
	class Proxy {
	public:
		Proxy(const std::string& line_buffer) : _line_buffer(line_buffer) {}
		Proxy(const Proxy& p) : _line_buffer(p._line_buffer) {}
		std::string& operator*() noexcept { return _line_buffer; }
	private:
		std::string _line_buffer;
	};

public:
	using category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using reference = std::string&;
	using pointer = std::string*;
	using valu_type = std::string;

	static const By_line_iterator end;

	// Constructs an interator that does not own any file.
	// Use such iterator as the end iterator.
	By_line_iterator() noexcept = default;

	By_line_iterator(const std::string& filename, bool keep_line_feed = false);

	By_line_iterator(const char* filename, bool keep_line_feed = false);

	By_line_iterator(const By_line_iterator& it) = delete;

	By_line_iterator(By_line_iterator&& it) noexcept;

	~By_line_iterator() noexcept;


	By_line_iterator& operator=(const By_line_iterator& it) = delete;

	By_line_iterator& operator=(By_line_iterator&& it) noexcept;

	reference operator*() noexcept
	{
		return _line_buffer;
	}

	pointer operator->() noexcept
	{
		return &_line_buffer;
	}

	By_line_iterator& operator++()
	{
		read_next_line();
		return *this;
	}

	Proxy operator++(int)
	{
		Proxy p(_line_buffer);
		read_next_line();
		return p;
	}
	

	// Returns the name of the file.
	const std::string& filename() const noexcept
	{
		return _file.filename();
	}

	// Returns true if the file has been opened. 
	// false means that this iterator is the end iterator.
	bool is_open() const noexcept
	{
		return _file.is_open();
	}


	friend bool operator==(const By_line_iterator& l, const By_line_iterator& r) noexcept
	{
		return !(l._file.is_open() && r._file.is_open());
	}

	friend bool operator!=(const By_line_iterator& l, const By_line_iterator& r) noexcept
	{
		return (l._file.is_open() || r._file.is_open());
	}

private:
	void read_next_line();

	File _file;
	bool _keep_line_feed;
	std::string _line_buffer;
};

cg::data::Interleaved_mesh_data load_mesh_wavefront(const std::string& filename, cg::data::Vertex_attribs attribs);

cg::data::Interleaved_mesh_data load_mesh_wavefront(const char * filename, cg::data::Vertex_attribs attribs);


} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_H_
