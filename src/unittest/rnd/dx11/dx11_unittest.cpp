#include "cg/rnd/dx11/dx11.h"

#include <iostream>
#include <utility>
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using cg::rnd::dx11::Com_ptr;


namespace {

struct Com_fake_class {

	Com_fake_class() noexcept :
	counter(1)
	{}

	long long Release() noexcept
	{
		return --counter;
	}

	long long counter = 0;
};

std::ostream& operator<<(std::ostream& o, const Com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

std::wostream& operator<<(std::wostream& o, const Com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

} // namespace


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Com_fake_class>(const Com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_utility_Com_ptr) {
public:

	TEST_METHOD(assignmen_operators)
	{
		{ // ucp0 = nullptr;
			Com_fake_class com_obj;
			Com_ptr<Com_fake_class> ucp(&com_obj);
			ucp = nullptr;
			Assert::IsNull(ucp.ptr);
			Assert::AreEqual<long long>(0, com_obj.counter);
		}

		{ // move assignment
			Com_fake_class com_obj0;
			Com_ptr<Com_fake_class> ucp0(&com_obj0);

			Com_fake_class com_obj1;
			Com_ptr<Com_fake_class> ucp1(&com_obj1);
			
			ucp0 = std::move(ucp1);
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp0 prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// ucp1 counter has not changed
			Assert::IsTrue(&com_obj1 == ucp0.ptr);				// ucp0 points to the com_obj1 resource
			Assert::IsNull(ucp1.ptr);							// ucp1 points to nothing
		}

		{ // move assignment, self assignment
			Com_fake_class com_obj;
			Com_ptr<Com_fake_class> ucp(&com_obj);

			ucp = std::move(ucp);
			Assert::AreEqual<long long>(1, com_obj.counter);	// counter has not changed
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj == ucp.ptr);
		}

		{ // ucp = raw_pointer
			Com_fake_class com_obj0;
			Com_ptr<Com_fake_class> ucp(&com_obj0);

			Com_fake_class com_obj1;

			ucp = &com_obj1;
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// counter has not changed
			Assert::IsTrue(&com_obj1 == ucp.ptr);				// ucp points to the com_obj1 resource
		}
	}

	TEST_METHOD(ctors_dtor)
	{
		Com_ptr<Com_fake_class> ucp0;
		Assert::IsNull(ucp0.ptr);

		Com_ptr<Com_fake_class> ucp00 = nullptr;
		Assert::IsNull(ucp00.ptr);

		// ctor(T*) & dtor().
		Com_fake_class com_obj0;
		Assert::AreEqual<long long>(1, com_obj0.counter);
		{
			Com_ptr<Com_fake_class> ucp(&com_obj0);
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj0 == ucp.ptr);
		}
		Assert::AreEqual<long long>(0, com_obj0.counter);

		// move ctor
		Com_fake_class com_obj1;
		Com_ptr<Com_fake_class> ucp1(&com_obj1);
		Com_ptr<Com_fake_class> ucp2(std::move(ucp1));
		Assert::AreEqual<long long>(1, com_obj1.counter);
		Assert::IsNull(ucp1.ptr);
		Assert::IsNotNull(ucp2.ptr);
		Assert::IsTrue(&com_obj1 == ucp2.ptr);
	}

	TEST_METHOD(implicit_cast_to_bool)
	{
		Com_ptr<Com_fake_class> ucp0;
		Assert::IsFalse(ucp0);

		Com_fake_class com_obj;
		Com_ptr<Com_fake_class> ucp1(&com_obj);
		Assert::IsTrue(ucp1);
	}

	TEST_METHOD(dispose)
	{
		Com_ptr<Com_fake_class> ucp0;
		ucp0.dispose();
		Assert::IsNull(ucp0.ptr);

		Com_fake_class com_obj;
		Com_ptr<Com_fake_class> ucp1(&com_obj);
		Assert::IsNotNull(ucp1.ptr);
		ucp1.dispose();
		Assert::IsNull(ucp1.ptr);
	}

	TEST_METHOD(operator_arrow)
	{
		Com_fake_class com_obj;
		Com_ptr<Com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, ucp->counter);
	}

	TEST_METHOD(operator_asterisk)
	{
		Com_fake_class com_obj;
		Com_ptr<Com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, (*ucp).counter);
	}

	TEST_METHOD(release_ownership)
	{
		Com_ptr<Com_fake_class> ucp0;
		auto* ptr0 = ucp0.release_ownership();
		Assert::IsNull(ucp0.ptr);
		Assert::IsNull(ptr0);

		Com_fake_class com_obj;
		Com_ptr<Com_fake_class> ucp1(&com_obj);
		auto* ptr1= ucp1.release_ownership();
		Assert::IsNull(ucp1.ptr);
		Assert::IsTrue(&com_obj == ptr1);
	}
};

} // namespace unittest