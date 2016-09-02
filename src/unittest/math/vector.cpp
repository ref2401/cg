#include "CppUnitTest.h"
#include "cg/math/math.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			Assert::AreEqual(5, cg::mul(4, 2));
		}

	};
}