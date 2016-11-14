#ifndef CG_FILE_FILE_BASE_H_
#define CG_FILE_FILE_BASE_H_

#include <cassert>
#include <cstdio>
#include <iterator>
#include <stdexcept>


namespace cg {
namespace file {

enum class File_seek_origin : unsigned char {
	current_position = 0,
	file_start = 1,
};

// Handles all files as binary. Implements only read facilities.
class File final {
public:

	File() noexcept = default;

	explicit File(const std::string& filename);

	explicit File(const char* filename);
	
	File(const File& f) = delete;

	File(File&& f) noexcept;

	~File() noexcept;


	File& operator=(File&& f) noexcept;


	// Closes the file and sets filename & handle to empty string and nullptr respectively.
	void close() noexcept;

	// Returns true if the end of the file has been reached.
	// Also returns true if this->handle() equals to nullptr.
	bool eof() const
	{
		assert(_handle);
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

	// Sets the new position of the internal file.
	// Params:
	// -	offset: Number of bytes to shift the position relative to origin.
	// -	origin: Position in file to which offset is added.
	void seek(long offset, File_seek_origin origin) const;

private:
	std::string _filename;
	mutable FILE* _handle = nullptr;
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

// Returns true if the given file (or directory) exists.
bool exists(const std::string& filename);

// Returns true if the given file (or directory) exists.
bool exists(const char* filename);

// Returns the content of the specified text file.
std::string load_text(const std::string& filename);

// Returns the content of the specified text file
std::string load_text(const char* filename);

} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_BASE_H_
