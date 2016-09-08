#include "cg/math/math.h"

#include <cmath>
#include "unittest/math/math_unittest_utils.h"

using cg::float3;
using cg::float4;
using cg::mat3;
using cg::mat4;
using cg::quat;


namespace unittest {

TEST_CLASS(Transform_unittest) {

	TEST_METHOD(from_axis_angle_rotation)
	{
		using cg::approx_equal;
		using cg::from_axis_angle_rotation;
		using cg::is_normalized;
		using cg::normalize;

		float3 axis = normalize(float3(-3.f, 5.4f, -2.f));
		quat q = from_axis_angle_rotation(axis, cg::pi_2);

		Assert::IsTrue(is_normalized(q));
		Assert::IsTrue(approx_equal(q.a, std::cos(cg::pi_4)));
	}

	TEST_METHOD(position_get_set)
	{
		using cg::position;
		using cg::set_position;

		mat4 m(
			0, 0, 0, 24,
			0, 0, 0, 42,
			0, 0, 0, 60,
			0, 0, 0, 0);

		Assert::AreEqual(float3(24, 42, 60), position(m));

		set_position(m, float3(1, 2, 3));
		Assert::AreEqual(float3(1, 2, 3), position(m));
	}

	TEST_METHOD(rotation_matrix_quat)
	{
		using cg::approx_equal;
		using cg::from_axis_angle_rotation;
		using cg::inverse;
		using cg::is_orthogonal;
		using cg::mul;
		using cg::normalize;
		using cg::rotation_matrix;


		Assert::AreEqual(mat3::zero, rotation_matrix<mat3>(quat::zero));
		Assert::AreEqual(mat3::identity, rotation_matrix<mat3>(quat::identity));
		Assert::AreEqual(mat4::identity, rotation_matrix<mat4>(quat::identity));
		Assert::AreEqual(mat4::identity, rotation_matrix<mat4>(quat::identity));

		float3 axis = normalize(float3(-5, 7, 1));
		float angle = cg::pi_2;
		quat q = from_axis_angle_rotation(axis, angle);

		mat3 r3 = rotation_matrix<mat3>(q);
		Assert::IsTrue(is_orthogonal(r3));
		Assert::AreEqual(r3, rotation_matrix<mat3>(axis, angle));

		mat4 r4 = rotation_matrix<mat4>(q);
		Assert::IsTrue(is_orthogonal(r4));
		Assert::AreEqual(r4, rotation_matrix<mat4>(axis, angle));

		// conjugation operation test
		float3 p(3, -7, 1);
		mat3 mR = rotation_matrix<mat3>(q);
		float3 res_mat_p = mul(mR, p); // rotation matrix * p

		quat pq = quat(p, 0);
		quat mult_prod = (q * pq) * inverse(q); // p' = qpq^(-1) = qpq* - conjugation operation
		float3 res_quat_conj = float3(mult_prod.x, mult_prod.y, mult_prod.z);
		Assert::AreEqual(res_mat_p, res_quat_conj, L"p' = qpq^(-1) = qpq* and RotationMatrix * vector");
	}

	TEST_METHOD(rotation_matrix_axis_angle)
	{
		using cg::approx_equal;
		using cg::inverse;
		using cg::is_orthogonal;
		using cg::normalize;
		using cg::rotation_matrix;
		using cg::trace;
		using cg::transpose;

		Assert::AreEqual(mat3::identity, rotation_matrix<mat3>(float3::unit_x, 0.f));
		Assert::AreEqual(mat4::identity, rotation_matrix<mat4>(float3::unit_x, 0.f));

		float3 axis = normalize(float3(-4, -5, 1));
		// For every 3x3 rotation matrix, that rotates by any radians around any axis, 
		// the trace is constant independent of axis.
		float angle = cg::pi_4;
		float expected_trace = 1.f + 2.f * std::cos(angle);
		mat3 r3 = rotation_matrix<mat3>(axis, angle);
		Assert::AreEqual(expected_trace, trace(r3));
		
		// TRetMat: mat4
		mat4 r = rotation_matrix<mat4>(axis, angle);
		Assert::IsTrue(is_orthogonal(r));
		Assert::IsTrue(is_orthogonal(transpose(r)));
		Assert::IsTrue(approx_equal(std::abs(det(r)), 1.f), L"|r| == +/-1");
		Assert::IsTrue(transpose(r) == inverse(r));
	}

	TEST_METHOD(rotation_matrix_ox_oy_oz)
	{
		using cg::rotation_matrix;
		using cg::rotation_matrix_ox;
		using cg::rotation_matrix_oy;
		using cg::rotation_matrix_oz;
		using cg::trace;

		Assert::AreEqual(mat3::identity, rotation_matrix_ox<mat3>(0.f));
		Assert::AreEqual(mat3::identity, rotation_matrix_oy<mat3>(0.f));
		Assert::AreEqual(mat3::identity, rotation_matrix_oz<mat3>(0.f));
		Assert::AreEqual(mat4::identity, rotation_matrix_ox<mat4>(0.f));
		Assert::AreEqual(mat4::identity, rotation_matrix_oy<mat4>(0.f));
		Assert::AreEqual(mat4::identity, rotation_matrix_oz<mat4>(0.f));

		// For every 3x3 rotation matrix, that rotates by any radians around any axis, 
		// the trace is constant independent of axis.
		float angle = cg::pi_4;
		float expected_trace = 1.f + 2.f * std::cos(angle);

		mat3 r_ox = rotation_matrix_ox<mat3>(angle);
		mat3 r_oy = rotation_matrix_oy<mat3>(angle);
		mat3 r_oz = rotation_matrix_oz<mat3>(angle);

		Assert::AreEqual(expected_trace, trace(r_ox));
		Assert::AreEqual(expected_trace, trace(r_oy));
		Assert::AreEqual(expected_trace, trace(r_oz));

		// TRetMat: mat3
		Assert::AreEqual(r_ox, rotation_matrix<mat3>(float3::unit_x, angle));
		Assert::AreEqual(r_oy, rotation_matrix<mat3>(float3::unit_y, angle));
		Assert::AreEqual(r_oz, rotation_matrix<mat3>(float3::unit_z, angle));

		// TRetMat: mat4
		Assert::AreEqual(rotation_matrix_ox<mat4>(angle), rotation_matrix<mat4>(float3::unit_x, angle));
		Assert::AreEqual(rotation_matrix_oy<mat4>(angle), rotation_matrix<mat4>(float3::unit_y, angle));
		Assert::AreEqual(rotation_matrix_oz<mat4>(angle), rotation_matrix<mat4>(float3::unit_z, angle));
	}

	TEST_METHOD(translation_matrix)
	{
		using cg::translation_matrix;

		float3 pos(5, 6, 7);
		mat4 expected_matrix = mat4::identity;
		expected_matrix.m03 = pos.x;
		expected_matrix.m13 = pos.y;
		expected_matrix.m23 = pos.z;

		Assert::AreEqual(expected_matrix, translation_matrix(pos));
	}

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