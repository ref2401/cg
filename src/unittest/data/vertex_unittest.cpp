#include "cg/data/vertex.h"

#include "unittest/math/common_math.h"
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_vertex_Funcs) {
public:

	TEST_METHOD(compute_tangent_bitangent)
	{
		using cg::normalize;
		using cg::data::compute_tangent_bitangent;

		float3 lb_pos0(-1, -1, 0);	float2 lb_tc0 = float2::zero;		// left-bottom
		float3 lb_pos1(1, -1, 0);	float2 lb_tc1 = float2::unit_x;		// right-bottom
		float3 lb_pos2(1, 1, 0);	float2 lb_tc2 = float2::unit_xy;	// right-top

		auto tb0 = compute_tangent_bitangent(lb_pos0, lb_tc0, lb_pos1, lb_tc1, lb_pos2, lb_tc2);
		auto expected_tb0 = std::make_pair(float3::unit_x, float3::unit_y);
		Assert::AreEqual(float3::unit_x, normalize(tb0.first));		// tangent
		Assert::AreEqual(float3::unit_y, normalize(tb0.second));	// bitangent
	}

	TEST_METHOD(compute_tangent_handedness)
	{
		using cg::normalize;
		using cg::data::compute_tangent_handedness;

		float3 tangent = normalize(float3(1, 0, 1)); // z component is set to 1 to check Gram-Schmidt orthogonalization
		float3 bitangnet = float3::unit_y;
		float3 normal_positive = float3::unit_z;
		float3 normal_negative = -float3::unit_z;

		// right-handed basis
		auto th0 = compute_tangent_handedness(tangent, bitangnet, normal_positive);
		Assert::AreEqual(float4(float3::unit_x, 1.0f), th0);

		// left-handed basis
		auto th1 = compute_tangent_handedness(tangent, bitangnet, normal_negative);
		Assert::AreEqual(float4(float3::unit_x, -1.0f), th1);
	}

};

} // namespace unittest
