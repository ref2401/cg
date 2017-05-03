#include "cg/rnd/dx11/dx11.h"

#include <iostream>
#include <utility>
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using cg::rnd::dx11::com_ptr;


namespace {

struct com_fake_class {

	com_fake_class() noexcept : counter(1) {}

	long long Release() noexcept
	{
		return --counter;
	}

	long long counter = 0;
};

std::ostream& operator<<(std::ostream& o, const com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

std::wostream& operator<<(std::wostream& o, const com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

} // namespace


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<com_fake_class>(const com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_dx11_com_ptr) {
public:

	TEST_METHOD(assignmen_operators)
	{
		{ // ucp0 = nullptr;
			com_fake_class com_obj;
			com_ptr<com_fake_class> ucp(&com_obj);
			ucp = nullptr;
			Assert::IsNull(ucp.ptr);
			Assert::AreEqual<long long>(0, com_obj.counter);
		}

		{ // move assignment
			com_fake_class com_obj0;
			com_ptr<com_fake_class> ucp0(&com_obj0);

			com_fake_class com_obj1;
			com_ptr<com_fake_class> ucp1(&com_obj1);
			
			ucp0 = std::move(ucp1);
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp0 prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// ucp1 counter has not changed
			Assert::IsTrue(&com_obj1 == ucp0.ptr);				// ucp0 points to the com_obj1 resource
			Assert::IsNull(ucp1.ptr);							// ucp1 points to nothing
		}

		{ // move assignment, self assignment
			com_fake_class com_obj;
			com_ptr<com_fake_class> ucp(&com_obj);

			ucp = std::move(ucp);
			Assert::AreEqual<long long>(1, com_obj.counter);	// counter has not changed
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj == ucp.ptr);
		}

		{ // ucp = raw_pointer
			com_fake_class com_obj0;
			com_ptr<com_fake_class> ucp(&com_obj0);

			com_fake_class com_obj1;

			ucp = &com_obj1;
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// counter has not changed
			Assert::IsTrue(&com_obj1 == ucp.ptr);				// ucp points to the com_obj1 resource
		}
	}

	TEST_METHOD(ctors_dtor)
	{
		com_ptr<com_fake_class> ucp0;
		Assert::IsNull(ucp0.ptr);

		com_ptr<com_fake_class> ucp00 = nullptr;
		Assert::IsNull(ucp00.ptr);

		// ctor(T*) & dtor().
		com_fake_class com_obj0;
		Assert::AreEqual<long long>(1, com_obj0.counter);
		{
			com_ptr<com_fake_class> ucp(&com_obj0);
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj0 == ucp.ptr);
		}
		Assert::AreEqual<long long>(0, com_obj0.counter);

		// move ctor
		com_fake_class com_obj1;
		com_ptr<com_fake_class> ucp1(&com_obj1);
		com_ptr<com_fake_class> ucp2(std::move(ucp1));
		Assert::AreEqual<long long>(1, com_obj1.counter);
		Assert::IsNull(ucp1.ptr);
		Assert::IsNotNull(ucp2.ptr);
		Assert::IsTrue(&com_obj1 == ucp2.ptr);
	}

	TEST_METHOD(implicit_cast_to_bool)
	{
		//com_ptr<com_fake_class> ucp0;
		//Assert::IsFalse(ucp0);

		//com_fake_class com_obj;
		//com_ptr<com_fake_class> ucp1(&com_obj);
		//Assert::IsTrue(ucp1);
	}

	TEST_METHOD(implicit_cast_to_ptr)
	{
		auto is_nullptr = [](com_fake_class* ptr) { return (ptr == nullptr); };
		
		com_ptr<com_fake_class> cp0;
		Assert::IsTrue(is_nullptr(cp0));
		com_fake_class* p0 = cp0;
		Assert::IsNull(p0);

		com_fake_class com_obj;
		com_ptr<com_fake_class> cp1(&com_obj);
		Assert::IsFalse(is_nullptr(cp1));
		com_fake_class* p1 = cp1;
		Assert::IsNotNull(p1);
	}

	TEST_METHOD(dispose)
	{
		com_ptr<com_fake_class> ucp0;
		ucp0.dispose();
		Assert::IsNull(ucp0.ptr);

		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp1(&com_obj);
		Assert::IsNotNull(ucp1.ptr);
		ucp1.dispose();
		Assert::IsNull(ucp1.ptr);
	}

	TEST_METHOD(operator_arrow)
	{
		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, ucp->counter);
	}

	TEST_METHOD(operator_asterisk)
	{
		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, (*ucp).counter);
	}

	TEST_METHOD(release_ownership)
	{
		com_ptr<com_fake_class> ucp0;
		auto* ptr0 = ucp0.release_ownership();
		Assert::IsNull(ucp0.ptr);
		Assert::IsNull(ptr0);

		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp1(&com_obj);
		auto* ptr1= ucp1.release_ownership();
		Assert::IsNull(ucp1.ptr);
		Assert::IsTrue(&com_obj == ptr1);
	}
};

} // namespace unittest