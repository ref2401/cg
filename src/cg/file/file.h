#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include "cg/base/base.h"


namespace cg {
namespace file {

// Handles all files as binary. Implements only read facilities.
class File final {
public:

	File();

	explicit File(const std::string& filename);

	explicit File(const char* filename);
	
	File(const File& f) = delete;

	File(File&& f) noexcept;

	~File() noexcept;


	File& operator=(const File& f) = delete;

	File& operator=(File&& f) noexcept;

	// Returns true if the end of the file has been reached.
	// Also returns true if this->handle() equals to nullptr.
	bool eof() const;

	// Returns the name of the file being processed by this File object.
	const std::string& filename() const noexcept;

	// Returns true if the file has been opened.
	bool is_open() const noexcept;


	// Closes the file and sets filename & handle to empty string and nullptr respectively.
	void close() noexcept;

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

// File_exception class is the only exception class in the cg::file module 
// that is used to indicate that any file related operation has failed.
class File_exception final : std::runtime_error {
public:

	explicit File_exception(const std::string& msg);

	explicit File_exception(const char* msg);

	~File_exception() noexcept;
};


inline bool File::eof() const
{
	enforce<File_exception>(_handle, "Invalid operation. File is not open.");
	return (std::feof(_handle) != 0);
}

inline const std::string& File::filename() const noexcept
{
	return _filename;
}

inline bool File::is_open() const noexcept
{
	return (_handle != nullptr);
}


} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_H_
