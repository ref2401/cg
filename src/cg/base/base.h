#ifndef CG_BASE_BASE_H_
#define CG_BASE_BASE_H_

#include <cassert>
#include <exception>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>


// Convenient macro to format more readable cg::enforce message.
// Prepends string arguments with __FILE__ & __LINE__ macro which.
// Dont forget to write semicolon at the end.
// Example:
// std::string msg = EXCEPTION_MSG("blah-blah")		<- compile time error
// std::string msg = EXCEPTION_MSG("blah-blah");	<- ok.
#define EXCEPTION_MSG(...) cg::concat(__FILE__, '(', __LINE__, "): ", __VA_ARGS__)

// Enforces that the given expression is true.
// If the expression evaluates to false then std::runtime_error is thrown.
#define ENFORCE(expression, ...)								\
	if (!(expression)) {										\
		throw std::runtime_error(EXCEPTION_MSG(__VA_ARGS__));	\
	}																										

namespace cg {
namespace internal {

inline void concat_impl(std::ostringstream& string_stream)
{}

template<typename T, typename... Args>
void concat_impl(std::ostringstream& string_stream, const T& obj, const Args&... args)
{
	string_stream << obj;
	concat_impl(string_stream, args...);
}

} // namespace internal


// Concatenates any number of std::string, c-style string or char arguments into one string.
template<typename... Args>
inline std::string concat(const Args&... args)
{
	std::ostringstream string_stream;
	cg::internal::concat_impl(string_stream, args...);
	return string_stream.str();
}

// Constructs exception message string considering all the nested exceptions.
// Each exception message is formatted as a new line and starts with " - " prefix. 
std::string exception_message(const std::exception& exc);

// Calculates how many bytes are in the specified number of kilobytes.
// Uses base 2 definition: 1 Kb = 1024 bytes = 2^10 bytes.
constexpr size_t kilobytes(size_t amount) noexcept
{
	return amount * 1024;
}

// Calculates how many bytes are in the specified number of megabytes.
// Uses base 2 definition: 1 Mb = 1048576 bytes = (1024 * 1024) bytes = 2^20 bytes.
constexpr size_t megabytes(size_t amount) noexcept
{
	return amount * 1048576;
}

}

#endif // CG_BASE_BASE_H_
