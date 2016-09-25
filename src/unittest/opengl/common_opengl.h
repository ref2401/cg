#ifndef UNITTEST_OPENGL_COMMON_OPENGL_H_
#define UNITTEST_OPENGL_COMMON_OPENGL_H_

#include "cg/opengl/opengl.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::opengl::DE_cmd>(const cg::opengl::DE_cmd& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::opengl::DE_base_vertex_params>(const cg::opengl::DE_base_vertex_params& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::opengl::DE_indirect_params>(const cg::opengl::DE_indirect_params& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

#endif UNITTEST_OPENGL_COMMON_OPENGL_H_
