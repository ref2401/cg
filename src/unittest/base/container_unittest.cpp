#include "cg/base/container.h"

#include <algorithm>
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

	TEST_METHOD(push_back_integers)
	{
		using cg::push_back;

		std::vector<uint64_t> integers;
		uint64_t expected_integers[7] = { 1, 2, 3, 4, 5, 6, 6 };

		push_back(integers, 1);
		Assert::AreEqual<size_t>(1, integers.size());

		push_back(integers, 2, 3, 4, 5);
		Assert::AreEqual<size_t>(5, integers.size());

		auto v = 6;
		push_back(integers, v);
		push_back(integers, std::move(v));
		Assert::AreEqual<size_t>(7, integers.size());

		Assert::IsTrue(std::equal(integers.cbegin(), integers.cend(), expected_integers));
	}

	TEST_METHOD(push_back_strings)
	{
		using cg::push_back;

		std::vector<std::string> strings;
		std::string expected_strings[7] = { "aaa", "bbb", "ccc", "ddd", "eee", "fff", "fff" };

		push_back(strings, "aaa");
		Assert::AreEqual<size_t>(1, strings.size());

		push_back(strings, "bbb", "ccc", "ddd", "eee");
		Assert::AreEqual<size_t>(5, strings.size());

		char* char_arr = "fff";
		std::string str = char_arr;
		push_back(strings, char_arr);
		push_back(strings, std::move(str));
		Assert::AreEqual<size_t>(7, strings.size());
		Assert::IsTrue(str.empty());
	}
};

} // namespace unittest
