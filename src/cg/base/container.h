#ifndef CG_BASE_CONTAINER_H_
#define CG_BASE_CONTAINER_H_


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

} // namespace cg

#endif // CG_BASE_CONTAINER_H_
