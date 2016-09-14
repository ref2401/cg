#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include "cg/base/base.h"


namespace cg {
namespace file {

// Implements only binary file read facilities.
class File final {
public:

	explicit File(const std::string& filename);

	explicit File(const char* filename);
	
	File(const File& f) = delete;

	File(File&& f) noexcept;

	~File() noexcept;


	File& operator=(const File& f) = delete;

	File& operator=(File&& f) noexcept;

	// Returns true if the end of the file has been reached.
	// Also returns true if this->handle() equals to nullptr.
	bool eof() const noexcept;

	// Returns the name of the file being processed by this File object.
	const std::string& filename() const noexcept;

	// Returns raw file handle that can be used directly with cstdio funcs.
	FILE* handle() noexcept;

	// Reads one byte and stores it in buff.
	// Returns: true is the read operation succeeded.
	bool read_byte(void* buff);
	
	// Reads a chunk of bytes and stores it in buff.
	// Returns: actual count of bytes read.
	size_t File::read_bytes(void* buff, size_t byte_count);

private:
	std::string _filename;
	FILE* _file_handle;
};

// File_exception class is the only exception class in the cg::file module 
// that is used to indicate that any file related operation has failed.
class File_exception final : std::runtime_error {
public:

	explicit File_exception(const std::string& msg);

	explicit File_exception(const char* msg);

	~File_exception() noexcept;
};


inline bool File::eof() const noexcept
{
	return (_file_handle) ? (std::feof(_file_handle) != 0) : true;
}

inline const std::string& File::filename() const noexcept
{
	return _filename;
}

inline FILE* File::handle() noexcept
{
	return _file_handle;
}


} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_H_
