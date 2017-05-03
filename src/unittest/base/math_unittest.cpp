#include "cg/base/math.h"

#include <cmath>
#include <utility>
#include "unittest/base/common_math.h"

using cg::Viewpoint;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Viewpoint>(const Viewpoint& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_base_math_viewpoint) {
public:

	TEST_METHOD(assigment_operators)
	{
		Viewpoint vp(float3(1, 2, 3), float3(4, 5, 6), float3(7, 8, 9));

		Viewpoint vp_c;
		vp_c = vp;
		Assert::AreEqual(vp.position, vp_c.position);
		Assert::AreEqual(vp.target, vp_c.target);
		Assert::AreEqual(vp.up, vp_c.up);

		Viewpoint vp_m;
		vp_m = std::move(vp_c);
		Assert::AreEqual(vp.position, vp_m.position);
		Assert::AreEqual(vp.target, vp_m.target);
		Assert::AreEqual(vp.up, vp_m.up);
	}

	TEST_METHOD(ctors)
	{
		Viewpoint vp0;
		Assert::AreEqual(float3::zero, vp0.position);
		Assert::AreEqual(float3::zero, vp0.target);
		Assert::AreEqual(float3::zero, vp0.up);

		float3 pos(1, 2 ,3);
		float3 tgt(4, 5, 6);
		float3 up(7, 8, 9);

		Viewpoint vp1(pos, tgt, up);
		Assert::AreEqual(pos, vp1.position);
		Assert::AreEqual(tgt, vp1.target);
		Assert::AreEqual(up, vp1.up);

		// copy ctor.
		Viewpoint vp_c = vp1;
		Assert::AreEqual(vp1.position, vp_c.position);
		Assert::AreEqual(vp1.target, vp_c.target);
		Assert::AreEqual(vp1.up, vp_c.up);

		// move ctor.
		Viewpoint vp_m = std::move(vp_c);
		Assert::AreEqual(vp1.position, vp_m.position);
		Assert::AreEqual(vp1.target, vp_m.target);
		Assert::AreEqual(vp1.up, vp_m.up);
	}

	TEST_METHOD(equal_operators)
	{
		Viewpoint vp(float3(1), float3(2), float3(3));

		Assert::AreNotEqual(vp, Viewpoint(float3(100), float3(2), float3(3)));
		Assert::AreNotEqual(vp, Viewpoint(float3(1), float3(200), float3(3)));
		Assert::AreNotEqual(vp, Viewpoint(float3(1), float3(2), float3(300)));

		Assert::AreEqual(vp, Viewpoint(float3(1), float3(2), float3(3)));
	}
};

TEST_CLASS(cg_base_math_funcs) {

	TEST_METHOD(distance_Viewpoint)
	{
		float3 position(1, 2, 3);
		float3 target(4, 5, 6);
		Viewpoint vp(position, target);

		Assert::AreEqual(len(target - position), distance(vp));
	}

	TEST_METHOD(forward_Viewpoint)
	{
		float3 position(1, 2, 3);
		float3 target(4, 5, 6);
		Viewpoint vp(position, target);

		Assert::AreEqual(normalize(target - position), forward(vp));
	}

	TEST_METHOD(lerp_Viewpoints)
	{
		Viewpoint vp0(float3(1), float3(2), float3(3));
		Viewpoint vp1(float3(4), float3(5), float3(6));

		float factor = 0.73f;
		auto vp = lerp(vp0, vp1, factor);
		Assert::AreEqual(lerp(vp0.position, vp1.position, factor), vp.position);
		Assert::AreEqual(lerp(vp0.target, vp1.target, factor), vp.target);
		Assert::AreEqual(lerp(vp0.up, vp1.up, factor), vp.up);
	}

	TEST_METHOD(view_matrix_Viewpoint)
	{
		const float3 position(1, 2, 3);
		const float3 target(4, 5, 6);
		const float3 up = normalize(float3(7, 8, 9));
		const Viewpoint vp(position, target, normalize(up));

		Assert::AreEqual(view_matrix(position, target, normalize(up)), view_matrix(vp));
	}

	TEST_METHOD(view_matrix_Viewpoints)
	{
		float factor = 0.34f;
		Viewpoint vp0(float3(1), float3(2), float3::unit_x);
		Viewpoint vp1(float3(3), float3(4), float3::unit_y);
		auto vp = lerp(vp0, vp1, factor);
		vp.up = normalize(vp.up);
		auto expected_matrix = view_matrix(vp);

		Assert::AreEqual(expected_matrix, view_matrix(vp0, vp1, factor));
	}
};

} // namespace unittest
