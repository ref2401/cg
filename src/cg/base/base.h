#ifndef CG_BASE_BASE_H_
#define CG_BASE_BASE_H_

#include <cassert>
#include <exception>
#include <string>
#include <type_traits>
#include <utility>

// Convenient macro to format more readable cg::enforce message.
// Prepends string arguments with __FILE__ & __LINE__ macro which.
#define ENFORSE_MSG(...) __FILE__, '(', std::to_string(__LINE__), "): ", __VA_ARGS__

namespace cg {

namespace internal {

inline void accumulate_exception_messge_impl(std::string& dest, const std::exception& exc)
{
	dest.append("- ");
	dest.append(exc.what());
	dest.push_back('\n');

	try {
		std::rethrow_if_nested(exc);
	}
	catch (const std::exception& nested_exc) {
		accumulate_exception_messge_impl(dest, nested_exc);
	}
	catch (...) {
		assert(false); // hell no!
	}
}

inline std::string concat_impl(std::string& dest)
{
	return dest;
}

template<typename... Args>
std::string concat_impl(std::string& dest, const char& c, const Args&... args)
{
	dest.push_back(c);
	return concat_impl(dest, args...);
}

template<typename... Args>
std::string concat_impl(std::string& dest, const std::string& str, const Args&... args)
{
	dest.append(str);
	return concat_impl(dest, args...);
}

template<typename... Args>
std::string concat_impl(std::string& dest, const char* str, const Args&... args)
{
	dest.append(str);
	return concat_impl(dest, args...);
}

} // namespace internal


// Concatenates any number of std::string, c-style string or char arguments into one string.
template<typename... Args>
inline std::string concat(const Args&... args)
{
	std::string dest;
	return cg::internal::concat_impl(dest, args...);
}

// Enforces that the given value is true.
//	Requires:
//	-	Exception: must be derived from std::exception or its descendants. 
//		The exception of type Exception is thrown if the value evaluates to false.
//	Params:
//	-	v:  value to test.
//	-	msg: error message to put in the exception if it is thrown.
template<typename Exception = std::runtime_error, typename T, typename... Strings>
void enforce(const T& v, const Strings&... strs)
{
	static_assert(std::is_base_of<std::exception, Exception>::value, "Exception must be derived from std::exception.");

	if (v) return;

	std::string msg = concat(strs...);
	throw Exception(msg);
}

// Constructs exception message string considering all the nested exceptions.
// Each exception message is formatted as a new line and starts with " - " prefix. 
inline std::string get_exception_message(const std::exception& exc)
{
	std::string msg;
	cg::internal::accumulate_exception_messge_impl(msg, exc);
	return msg;
}

}

#endif // CG_BASE_BASE_H_
