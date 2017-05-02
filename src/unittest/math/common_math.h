#ifndef UNITTEST_MATH_COMMON_MATH_H_
#define UNITTEST_MATH_COMMON_MATH_H_

#include "cg/base/math.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<math::float2>(const math::float2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::float3>(const math::float3& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::float4>(const math::float4& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::float3x3>(const math::float3x3& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::float4x4>(const math::float4x4& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::quat>(const math::quat& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::ubyte4>(const math::ubyte4& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::uint2>(const math::uint2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<math::uint4>(const math::uint4& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


#endif // UNITTEST_MATH_COMMON_MATH_H_
