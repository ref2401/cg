#include "cg/math/math.h"

#include "unittest/math/math_unittest_utils.h"

using cg::float3;
using cg::float4;
using cg::mat3;
using cg::mat4;
using cg::quat;


namespace unittest {

TEST_CLASS(Transform_unittest) {

	TEST_METHOD(internal_is_3d_float_vector)
	{
		using cg::internal::is_3d_float_vector;

		Assert::IsTrue(is_3d_float_vector<float3>::value);
		Assert::IsTrue(is_3d_float_vector<float4>::value);

		Assert::IsFalse(is_3d_float_vector<char>::value);
		Assert::IsFalse(is_3d_float_vector<cg::float2>::value);
		Assert::IsFalse(is_3d_float_vector<cg::uint2>::value);
		Assert::IsFalse(is_3d_float_vector<mat3>::value);
		Assert::IsFalse(is_3d_float_vector<mat4>::value);
	}

	TEST_METHOD(internal_is_matrix)
	{
		using cg::internal::is_matrix;

		Assert::IsTrue(is_matrix<mat3>::value);
		Assert::IsTrue(is_matrix<mat4>::value);

		Assert::IsFalse(is_matrix<char>::value);
		Assert::IsFalse(is_matrix<cg::float2>::value);
		Assert::IsFalse(is_matrix<float3>::value);
		Assert::IsFalse(is_matrix<float4>::value);
		Assert::IsFalse(is_matrix<cg::uint2>::value);
	}

};

} // namespace unittest