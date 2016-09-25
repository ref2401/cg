#include "cg/math/math.h"

#include <cmath>
#include "unittest/math/common_math.h"

using cg::float3;
using cg::float4;
using cg::mat3;
using cg::mat4;
using cg::quat;


namespace unittest {

TEST_CLASS(cg_math_Transform) {

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

	TEST_METHOD(from_rotation_matrix)
	{
		using cg::from_axis_angle_rotation;
		using cg::from_rotation_matrix;
		using cg::normalize;
		using cg::rotation_matrix;

		Assert::AreEqual(quat::zero, from_rotation_matrix(mat3::zero));
		Assert::AreEqual(quat::zero, from_rotation_matrix(mat4::zero));
		Assert::AreEqual(quat::identity, from_rotation_matrix(mat3::identity));
		Assert::AreEqual(quat::identity, from_rotation_matrix(mat4::identity));

		float3 axis = normalize(float3(1, 2, 5));
		float angle = cg::pi_4;
		quat expected_quat = from_axis_angle_rotation(axis, angle);

		mat4 mR = rotation_matrix<mat4>(axis, angle);
		quat actual_quat = from_rotation_matrix(mR);
		Assert::AreEqual(expected_quat, actual_quat);
	}

	TEST_METHOD(orthographic_matrix)
	{
		using cg::orthographic_matrix;

		float width = 800.f;
		float height = 600.f;
		float near = -5.f;
		float far = 10.f;

		// manual computation of (left, right, bottom, top, near, far) tuple
		float hw = width / 2.f;
		float hh = height / 2.f;

		mat4 om1 = orthographic_matrix(-hw, hw, -hh, hh, near, far);
		mat4 om2 = orthographic_matrix(width, height, near, far);
		Assert::AreEqual(om1, om2);
	}

	TEST_METHOD(perspective_matrix)
	{
		using cg::perspective_matrix;

		float near = 1.f;
		float far = 100.f;
		float fov = cg::pi_2;
		float ratio = 800.f / 600.f;

		// manual computation of (left, right, bottom, top, near, far) tuple
		float height = near * std::tan(fov / 2.f);
		float width = height * ratio;

		mat4 pm1 = perspective_matrix(-width, width, -height, height, near, far);
		mat4 pm2 = perspective_matrix(fov, ratio, near, far);
		Assert::AreEqual(pm1, pm2);
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

	TEST_METHOD(rotation_matrix_look_at)
	{
		using cg::rotation_matrix;

		// rotation around the OX axis
		mat3 oxM1 = rotation_matrix<mat3>(float3::zero, float3::unit_y, -float3::unit_z);
		Assert::AreEqual(oxM1, rotation_matrix<mat3>(float3::unit_x, -cg::pi_2)); // -pi_2 - counter clockwise

		// rotation arount the OY axis
		mat3 oyM1 = rotation_matrix<mat3>(-float3::unit_z, float3::zero, float3::unit_y);
		Assert::AreEqual(oyM1, mat3::identity);

		mat3 oyM2 = rotation_matrix<mat3>(float3::zero, float3(1, 0, 1), float3::unit_y);
		Assert::AreEqual(oyM2, rotation_matrix<mat3>(float3::unit_y, cg::pi_4));

		mat4 oyM3 = rotation_matrix<mat4>(float3(0, 0, 7), float3(0, 0, -1), float3::unit_y);
		Assert::AreEqual(oyM3, rotation_matrix<mat4>(float3::unit_y, cg::pi));
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

	TEST_METHOD(scale_matrix)
	{
		using cg::scale_matrix;

		Assert::AreEqual(mat3(4, 0, 0, 0, 5, 0, 0, 0, 6), scale_matrix<mat3>(float3(4, 5, 6)));
		Assert::AreEqual(mat4(7, 0, 0, 0, 0, 8, 0, 0, 0, 0, 9, 0, 0, 0, 0, 1), scale_matrix<mat4>(float3(7, 8, 9)));
	}

	TEST_METHOD(tr_matrix_pos_quat)
	{
		using cg::from_axis_angle_rotation;
		using cg::normalize;
		using cg::rotation_matrix;
		using cg::tr_matrix;
		using cg::translation_matrix;

		quat q = from_axis_angle_rotation(normalize(float3(-5, 3, -10)), cg::pi_4);
		float3 pos(7, 8, 9);

		Assert::AreEqual(translation_matrix(pos) * rotation_matrix<mat4>(q), tr_matrix(pos, q));
	}

	TEST_METHOD(tr_matrix_pos_look_at)
	{
		using cg::rotation_matrix;
		using cg::tr_matrix;
		using cg::translation_matrix;

		float3 p(4, -1, 8);
		float3 t(1, 1, -8);
		Assert::AreEqual(translation_matrix(p) * rotation_matrix<mat4>(p, t), tr_matrix(p, t));
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

	TEST_METHOD(trs_matrix)
	{
		using cg::from_axis_angle_rotation;
		using cg::normalize;
		using cg::rotation_matrix;
		using cg::scale_matrix;
		using cg::trs_matrix;
		using cg::translation_matrix;

		float3 p(7, 8, 9);
		quat q = from_axis_angle_rotation(normalize(float3(-5, 3, -10)), cg::pi_4);
		float3 s(2, 3, 4);

		mat4 mT = translation_matrix(p);
		mat4 mR = rotation_matrix<mat4>(q);
		mat4 mS = scale_matrix<mat4>(s);

		Assert::AreEqual(mT * mR * mS, trs_matrix(p, q, s));
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
