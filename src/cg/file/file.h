#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include <cstdio>
#include <stdexcept>
#include "cg/base/base.h"


namespace cg {
namespace file {

class File final {
public:

	explicit File(const std::string& filename);

	explicit File(const char* filename);
	
	File(const File& f) = delete;

	File(File&& f) noexcept;

	~File() noexcept;


	File& operator=(const File& f) = delete;

	File& operator=(File&& f) noexcept;


	const std::string& filename() const noexcept;

	FILE* handle() noexcept;

private:
	std::string _filename;
	FILE* _file_handle;
};

inline const std::string& File::filename() const noexcept
{
	return _filename;
}

inline FILE* File::handle() noexcept
{
	return _file_handle;
}


// File_exception class is the only exception class in the cg::file module 
// that is used to indicate that any file related operation has failed.
class File_exception final : std::runtime_error {
public:

	explicit File_exception(const std::string& msg);

	explicit File_exception(const char* msg);

	~File_exception() noexcept;
};

} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_H_
