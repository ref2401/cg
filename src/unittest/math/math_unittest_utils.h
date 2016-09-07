#ifndef UNITTEST_MATH_MATH_UNITTEST_UTILS_H_
#define UNITTEST_MATH_MATH_UNITTEST_UTILS_H_

#include "cg/math/math.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::float2>(const cg::float2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::float3>(const cg::float3& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::float4>(const cg::float4& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::mat3>(const cg::mat3& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::mat4>(const cg::mat4& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::quat>(const cg::quat& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::uint2>(const cg::uint2& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

#endif // UNITTEST_MATH_MATH_UNITTEST_UTILS_H_
