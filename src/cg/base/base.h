#ifndef CG_BASE_BASE_H_
#define CG_BASE_BASE_H_

#include <stdexcept>
#include <type_traits>
#include <utility>


namespace cg {

// Enforces that the given value is true.
//	Requires:
//	-	Exception: must be derived from std::exception or its descendants. 
//		The exception of type Exception is thrown if the value evaluates to false.
//	Params:
//	-	v:  value to test.
//	-	msg: error message to put in the exception if it is thrown.
template<typename Exception = std::runtime_error, typename T>
void enforce(const T& v, const std::string& msg);

// Enforces that the given value is true.
//	Requires:
//	-	Exception: must be derived from std::exception or its descendants. 
//		The exception of type Exception is thrown if the value evaluates to false.
//	Params:
//	-	v:  value to test.
//	-	msg: error message to put in the exception if it is thrown.
template<typename Exception = std::runtime_error, typename T>
void enforce(const T& v, const char* msg);


template<typename Exception, typename T>
void enforce(const T& v, const std::string& msg)
{
	static_assert(std::is_base_of<std::exception, Exception>::value, "Exception must be derived from std::exception.");

	if (!v)
		throw Exception(msg);
}

template<typename Exception, typename T>
inline void enforce(const T& v, const char* msg)
{
	static_assert(std::is_base_of<std::exception, Exception>::value, "Exception must be derived from std::exception.");
	
	if (!v)
		throw Exception(msg);
}

}

#endif // CG_BASE_BASE_H_
