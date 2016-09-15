#ifndef UNITTEST_DATA_COMMON_DATA_H_
#define UNITTEST_DATA_COMMON_DATA_H_

#include "cg/data/mesh.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::data::Interleaved_vertex_format>(const cg::data::Interleaved_vertex_format& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::data::Vertex>(const cg::data::Vertex& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::data::Vertex_attribs>(const cg::data::Vertex_attribs& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

#endif // UNITTEST_DATA_COMMON_DATA_H_
