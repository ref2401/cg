#include "cg/base/container.h"

#include <array>
#include <type_traits>
#include <vector>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_base_container_Funcs) {
public:

	TEST_METHOD(byte_count)
	{
		using cg::byte_count;

		float arr[10];
		Assert::AreEqual(sizeof(float) * std::extent<decltype(arr)>::value, byte_count(arr));

		std::array<float, 4> std_arr;
		Assert::AreEqual(sizeof(float) * std_arr.size(), byte_count(std_arr));

		std::vector<float> vector { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
		Assert::AreEqual(sizeof(float) * vector.size(), byte_count(vector));
	}
};

} // namespace unittest
