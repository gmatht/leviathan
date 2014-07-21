#pragma once

#include <type_traits>
 
namespace details {

template<typename T>
size_t type_id_impl()
{
	static size_t impl = 0;
	return reinterpret_cast<size_t>(&impl);
}
 
template<typename T, typename U>
struct copy_const
{
	using type = T;
};
 
template<typename T, typename U>
struct copy_const<T, U const>
{
	using type = typename std::add_const<T>::type;
};
 
template<typename T, typename U>
using copy_const_t = typename copy_const<T, U>::type;

}
 
/**!
 * Returns a numeric identifier that is unique for every type, ignoring
 * reference qualifiers.
 */
template<typename T>
size_t type_id()
{
	return details::type_id_impl<typename std::remove_reference<T>::type>();
}
 
template<typename T, typename U, typename ReturnT = details::copy_const_t<T, U>>
auto fast_cast(U *ptr)-> decltype(ptr->tid(), (ReturnT*)nullptr)
{
	if (type_id<T>() != ptr->tid())
		return nullptr;
 
	return static_cast<ReturnT *>(ptr);
}
