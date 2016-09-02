#include "cg/math/math.h"

#include <string>
#include <utility>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
namespace VisualStudio {
namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::float_3>(const cg::float_3& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(Vector_unittest)
{
public:

	//TEST_METHOD(static_members)
	//{
	//	Assert::AreEqual(float_3(0, 0, 0), float_3::zero);
	//	Assert::AreEqual(float_3(1, 0, 0), float_3::unit_x);
	//	Assert::AreEqual(float_3(0, 1, 0), float_3::unit_y);
	//	Assert::AreEqual(float_3(0, 0, 1), float_3::unit_z);
	//	Assert::AreEqual(float_3(1, 1, 1), float_3::unit_xyz);
	//}

	TEST_METHOD(ctors)
	{
		float_3 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0));

		float_3 v1 = float_3(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24));

		float_3 v3 = float_3(1, 2, 3);
		Assert::IsTrue((v3.x == 1) && (v3.y == 2) && (v3.z == 3));

		// copy ctor
		float_3 vc = v3;
		Assert::IsTrue((vc.x == v3.x) && (vc.y == v3.y) && (vc.z == v3.z));

		// move ctor
		float_3 vm = std::move(v3);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z));
	}

	TEST_METHOD(assignment)
	{
		float_3 v = float_3(5, 6, 7);

		// copy assignment
		float_3 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z));

		// move assignment
		float_3 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z));
	}

	TEST_METHOD(operator_equal)
	{
		float_3 v(1, 2, 3);

		Assert::AreNotEqual(v, float_3(100, 2, 3));
		Assert::AreNotEqual(v, float_3(1, 100, 3));
		Assert::AreNotEqual(v, float_3(1, 2, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, float_3(1, 2, 3));
	}
};

} // unittest
