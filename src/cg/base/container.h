#ifndef CG_BASE_CONTAINER_H_
#define CG_BASE_CONTAINER_H_

#include <utility>
#include <vector>


namespace cg {

// Returns the number of bytes occupied by arr.
template<typename T, size_t count>
constexpr size_t byte_count(const T (&arr)[count]) noexcept
{
	return sizeof(T) * count;
}

// Returns the number of bytes occupied by elements of the c.
// Take in account that c by itself may occupy more space.
template<typename Container>
inline size_t byte_count(const Container& c) noexcept
{
	return sizeof(Container::value_type) * c.size();
}

template<typename T>
inline void push_back(std::vector<T>& vector) 
{}

template<typename T, typename U, typename... Args>
void push_back(std::vector<T>& vector, U&& object, Args&&... args)
{
	vector.push_back(std::forward<U>(object));
	push_back(vector, std::forward<Args>(args)...);
}

//template<typename... Args>
//void push_back(std::vector<std::string>& vector, const char* str, Args&&... args)
//{
//	vector.emplace_back(str);
//	push_back(vector, std::forward<Args>(args)...);
//}
//
//template<typename... Args>
//void push_back(std::vector<std::string>& vector, const char* str, Args&&... args)
//{
//	vector.emplace_back(str);
//	push_back(vector, std::forward<Args>(args)...);
//}

} // namespace cg

#endif // CG_BASE_CONTAINER_H_
