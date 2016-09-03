#ifndef UNITTEST_MATH_MATH_UNITTEST_UTILS_H_
#define UNITTEST_MATH_MATH_UNITTEST_UTILS_H_

#include "cg/math/math.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::float_3>(const cg::float_3& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::float_4>(const cg::float_4& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

#endif // UNITTEST_MATH_MATH_UNITTEST_UTILS_H_
