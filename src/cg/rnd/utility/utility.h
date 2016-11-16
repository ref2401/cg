#ifndef CG_RND_UTILITY_UTILITY_H_
#define CG_RND_UTILITY_UTILITY_H_

#include <vector>
#include "cg/math/math.h"


namespace cg {
namespace rnd {
namespace utility {

// Unique_com_ptr is a smart pointer that owns and manages a COM object through a pointer 
// and disposes of that object when the Unique_com_ptr goes out of scope.
template<typename T>
struct Unique_com_ptr final {
	
	// Commented because unittests use fake COM interface.
	//static_assert(std::is_base_of<IUnknown, T>::value, "T must be derived from IUnknown.");


	Unique_com_ptr() noexcept {}

	explicit Unique_com_ptr(T* ptr) noexcept :
		ptr(ptr)
	{}

	Unique_com_ptr(const Unique_com_ptr&) = delete;

	Unique_com_ptr(Unique_com_ptr&& com_ptr) noexcept :
		ptr(com_ptr.ptr)
	{
		com_ptr.ptr = nullptr;
	}

	~Unique_com_ptr() noexcept
	{
		dispose();
	}


	Unique_com_ptr& operator=(const Unique_com_ptr&) = delete;

	Unique_com_ptr& operator=(Unique_com_ptr&& com_ptr) noexcept
	{
		if (this == &com_ptr) return *this;

		dispose();
		ptr = com_ptr.ptr;
		com_ptr = nullptr;
		return *this;
	}

	Unique_com_ptr& operator=(T* ptr) noexcept
	{
		dispose();
		this->ptr = ptr;
		return *this;
	}

	Unique_com_ptr& operator=(nullptr_t) noexcept
	{
		dispose();
		return *this;
	}

	operator bool() const noexcept
	{
		return (ptr != nullptr);
	}

	T& operator*() const noexcept
	{
		return *ptr;
	}

	T* operator->() const noexcept
	{
		return ptr;
	}


	// Releases the managed COM object if such is present.
	void dispose() noexcept;

	// Releases the ownership of the managed COM object and returns a pointer to it.
	// Does not call ptr->Release(). ptr == nullptr after that. 
	T* release_ownership() noexcept
	{
		T* tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	// Pointer to the managed COM object.
	T* ptr = nullptr;
};

template<typename T>
void Unique_com_ptr<T>::dispose() noexcept
{
	if (ptr == nullptr) return;

	auto c = ptr->Release();
	ptr = nullptr;
}

template<typename T>
inline bool operator==(const Unique_com_ptr<T>& l, const Unique_com_ptr<T>& r) noexcept
{
	return l.ptr == r.ptr;
}

template<typename T>
inline bool operator==(const Unique_com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator==(nullptr_t, const Unique_com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator!=(const Unique_com_ptr<T>& l, const Unique_com_ptr<T>& r) noexcept
{
	return l.ptr != r.ptr;
}

template<typename T>
inline bool operator!=(const Unique_com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr != nullptr;
}

template<typename T>
inline bool operator!=(nullptr_t, const Unique_com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr != nullptr;
}


// Generates random vectors within a unit hemisphere.
// The hemisphere is located at (0, 0, 0) and oriendted along the positive OZ axis.
std::vector<cg::float3> generate_hemispherical_sample_kernel(size_t sample_count);

std::vector<cg::float3> generate_sphere_normalized_sample_kernel(size_t sample_count);

} // namespace utility
} // namespace rnd
} // namespace cg

#endif // CG_RND_UTILITY_UTILITY_H_
