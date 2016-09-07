#include "cg/math/math.h"

#include "unittest/math/math_unittest_utils.h"

using cg::float3;
using cg::quat;


namespace unittest {

TEST_CLASS(Quat_unittest) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(quat(1, 0, 0, 0), quat::i);
		Assert::AreEqual(quat(0, 1, 0, 0), quat::j);
		Assert::AreEqual(quat(0, 0, 1, 0), quat::k);
		Assert::AreEqual(quat(0, 0, 0, 1), quat::identity);
		Assert::AreEqual(quat(0, 0, 0, 0), quat::zero);
	}

	TEST_METHOD(ctors)
	{
		quat q0(1, 2, 3, 4);
		Assert::IsTrue((q0.x == 1) && (q0.y == 2) && (q0.z == 3) && (q0.a == 4));

		quat q1(float3(1, 2, 3), 4);
		Assert::IsTrue((q1.x == 1) && (q1.y == 2) && (q1.z == 3) && (q1.a == 4));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		quat q(5, 6, 7, 8);
		quat r(1, 2, 3, 4);

		(q += r) += r;
		Assert::AreEqual(quat(7, 10, 13, 16), q);

		(q -= r) -= r;
		Assert::AreEqual(quat(5, 6, 7, 8), q);

		(q *= 2) *= 3;
		Assert::AreEqual(quat(30, 36, 42, 48), q);

		(q /= 3) /= 2;
		Assert::AreEqual(quat(5, 6, 7, 8), q);

		// NOTE(ref2401): Hamilton product compaund assignment operator 
		// is tested in the binary_operators test method.
	}


	TEST_METHOD(binary_operator)
	{
		quat q(1, 2, 3, 4);

		Assert::AreEqual(quat(1.11f, 2.22f, 3.33f, 4.44f), q + quat(0.11f, 0.22f, 0.33f, 0.44f));
		Assert::AreEqual(quat(1.11f, 2.22f, 3.33f, 4.44f), quat(0.11f, 0.22f, 0.33f, 0.44f) + q);

		Assert::AreEqual(quat(0, 1, 2, 3), q - quat(1, 1, 1, 1));
		Assert::AreEqual(quat(0, -1, -2, -3), quat(1, 1, 1, 1) - q);

		Assert::AreEqual(quat(10, 20, 30, 40), q * 10);
		Assert::AreEqual(quat(10, 20, 30, 40), 10 * q);

		Assert::AreEqual(quat(0.5f, 1, 1.5f, 2), q / 2);
		Assert::AreEqual(quat(2, 1, 2.f / 3, 0.5f), 2 / q);

		// Hamilton product
		quat neg_identity(0, 0, 0, -1);
		Assert::AreEqual(neg_identity, quat::i * quat::i);
		Assert::AreEqual(neg_identity, quat::j * quat::j);
		Assert::AreEqual(neg_identity, quat::k * quat::k);
		Assert::AreEqual(neg_identity, (quat::i * quat::j) * quat::k);

		Assert::AreEqual(quat::k, quat::i * quat::j, L"ij = k");
		Assert::AreEqual(quat::i, quat::j * quat::k, L"jk = i");
		Assert::AreEqual(quat::j, quat::k * quat::i, L"ki = j");
		Assert::AreEqual(-quat::k, quat::j * quat::i, L"ji = -k");
		Assert::AreEqual(-quat::i, quat::k * quat::j, L"kj = -i");
		Assert::AreEqual(-quat::j, quat::i * quat::k, L"ik = -j");

		quat p(2, 3, 4, 1);
		quat r(10, 11, 12, 9);

		Assert::AreEqual(p * (q * r), (p * q) * r);
		Assert::AreEqual(p*(5*q + 7*r), 5*(p*q) + 7*(p*r), L"linearity: p(5q + 7r) = 5pq + 7pr");
		Assert::AreEqual((5*p + 7*q)*r, 5*(p*r) + 7*(q*r), L"linearity: (5p + 7q)r = 5pr + 7qr");

		Assert::AreEqual(q*5, q*quat(0, 0, 0, 5));

		// Hamilton product compaund assignment operator
		quat p1 = p;
		Assert::AreEqual(p * q * r, (p1 *= q) *= r);
	}

	TEST_METHOD(conjugate)
	{
		using cg::approx_equal;
		using cg::conjugate;
		using cg::len;

		quat q(2, 3, 4, 1);
		quat r(6, 7, 8, 5);

		Assert::IsTrue(approx_equal(len(q), len(conjugate(q))), L"|q*| = |q|");
		Assert::AreEqual(q, conjugate(conjugate(q)), L"(q*)* = q");
		Assert::AreEqual(conjugate(q + r), conjugate(q) + conjugate(r), L"(q + r)* = q* + r*");
		Assert::AreEqual(conjugate(q * r), conjugate(r) * conjugate(q), L"(qr)* = r*q*");
	}

	TEST_METHOD(equal_operator)
	{
		quat q(1, 2, 3, 4);

		Assert::AreNotEqual(q, quat(100, 2, 3, 4));
		Assert::AreNotEqual(q, quat(1, 100, 3, 4));
		Assert::AreNotEqual(q, quat(1, 2, 100, 4));
		Assert::AreNotEqual(q, quat(1, 2, 3, 100));

		Assert::AreEqual(q, q);
		Assert::AreEqual(q, quat(1, 2, 3, 4));
	}

	TEST_METHOD(inverse)
	{
		using cg::inverse;

		Assert::AreEqual(quat::identity, inverse(quat::identity));

		quat q = quat(2, 3, 4, 5);

		Assert::AreEqual(quat::identity, q * inverse(q));
		Assert::AreEqual(quat::identity, inverse(q) * q);
	}

	TEST_METHOD(is_normalized)
	{
		using cg::is_normalized;
		using cg::len;

		Assert::IsTrue(is_normalized(quat::i));
		Assert::IsTrue(is_normalized(quat::j));
		Assert::IsTrue(is_normalized(quat::k));
		Assert::IsTrue(is_normalized(quat::identity));

		quat q(1, 2, 3, 4);
		Assert::IsTrue(len(q) > 1 && (!is_normalized(q)));
	}

	TEST_METHOD(len_and_len_squared)
	{
		quat q(2, 3, 4, 5);
		quat r(4, 5, 6, 7);

		Assert::AreEqual(2.f*2 + 3*3 + 4*4 + 5*5, cg::len_squared(q));
		Assert::AreEqual(std::sqrt(2.f*2 + 3*3 + 4*4 + 5*5), cg::len(q));

		Assert::AreEqual(2*cg::len(q), cg::len(2*q), L"|aQ| = |a| * |Q|");
		
		Assert::IsTrue(
			cg::approx_equal(cg::len(q + r), 
			cg::len(q) + cg::len(r), 0.1f), 
			L"|Q + P| <= |Q| + |P|");
		
		Assert::IsTrue(
			cg::approx_equal(cg::len(q * r), 
			cg::len(q) * cg::len(r)), 
			L"|QP| = |Q| * |P|");
	}

	TEST_METHOD(normalize)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::normalize;

		Assert::AreEqual(quat::zero, normalize(quat::zero));
		Assert::AreEqual(quat::identity, normalize(quat::identity));

		quat q = quat(6, 7, -8, 0);
		Assert::IsTrue(approx_equal( 1.f, len(normalize(q)) ));

		
	}

	TEST_METHOD(slerp)
	{
		using cg::is_normalized;
		using cg::normalize;
		using cg::slerp;

		quat q = normalize(quat(1, 2, 3, 4));
		quat r = normalize(quat(5, 6, 7, 8));
		Assert::AreEqual(q, slerp(q, r, 0.f));
		Assert::AreEqual(r, slerp(q, r, 1.f));

		quat qs = slerp(q, r, 0.5f);
		Assert::IsTrue(is_normalized(qs));
	}

	TEST_METHOD(unary_operators)
	{
		quat q(1, 2, 3, 4);

		Assert::AreEqual(quat(-1, -2, -3, -4), -q);
		Assert::AreEqual(quat(1, 2, 3, 4), -(-q));
	}
};

} // namespace unittest
