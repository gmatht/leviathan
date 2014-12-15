#pragma once

#include <cstddef>
#include <limits>
#include <cassert>
#include <cstdint>

namespace dynamic_bitset
{
namespace detail
{

inline size_t count(uint64_t value)
{
        return __builtin_popcountll(value);
}

inline size_t count(uint32_t value)
{
        return __builtin_popcount(value);
}

inline size_t leading_zero(uint64_t value)
{
        return __builtin_clzll(value);
}

inline size_t leading_zero(uint32_t value)
{
        return __builtin_clz(value);
}

inline size_t trailing_zero(uint64_t value)
{
        return __builtin_ctzll(value);
}

inline size_t trailing_zero(uint32_t value)
{
        return __builtin_ctz(value);
}

template<typename T, typename = void>
class wrapper;

template<typename T>
class wrapper<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
        static_assert(std::is_unsigned<T>::value, "Type T is not an unsigned integral type!");

public:
        using size_type = std::size_t;

        wrapper() = default;
        wrapper(wrapper const&) = default;
        wrapper& operator=(wrapper const&) = default;

        explicit wrapper(T v) : _v(v) {}

        static constexpr size_type bits = sizeof(T) * 8;

        bool get(size_type i) const
        {
                assert(i < bits);
                return _v & (static_cast<T>(1) << i);
        }

        void set()
        {
                _v = std::numeric_limits<T>::max();
        }

        void set(size_type i, bool b)
        {
                assert(i < bits);
                _v = (_v & ~(static_cast<T>(1) << i)) | (static_cast<T>(b) << i);
        }

        void reset()
        {
                _v = static_cast<T>(0);
        }

        void flip(size_type i)
        {
                assert(i < bits);
                _v ^= (static_cast<T>(1) << i);
        }

        void flip()
        {
                _v = ~_v;
        }

        size_type count() const
        {
                return detail::count(_v);

                /* Alternative implementation
                T val = _v;
                val = val - ((val >> 1) & (T)~(T)0/3);
                val = (val & (T)~(T)0/15*3) + ((val >> 2) & (T)~(T)0/15*3);
                val = (val + (val >> 4)) & (T)~(T)0/255*15;
                return (T)(val * ((T)~(T)0/255)) >> (sizeof(T) - 1) * 8;
                */
        }

        size_type leading_zero() const
        {
                return detail::leading_zero(_v);
        }

        size_type trailing_zero() const
        {
                return detail::trailing_zero(_v);
        }

        bool all() const
        {
                return _v == std::numeric_limits<T>::max();
        }

        bool none() const
        {
                return _v == static_cast<T>(0);
        }

        bool any() const
        {
                return !none();
        }

        #define OP(Op) \
        wrapper &operator Op##=(wrapper w) { \
                _v Op##= w._v; \
                return *this; \
        }

        OP(|)
        OP(&)
        OP(^)
        #undef OP

        wrapper& operator<<=(size_t shift)
        {
                _v <<= shift;
                return *this;
        }

        wrapper& operator>>=(size_t shift)
        {
                _v >>= shift;
                return *this;
        }

        wrapper operator~() const
        {
                return wrapper(~_v);
        }

        friend bool operator==(wrapper w1, wrapper w2)
        {
                return w1._v == w2._v;
        }

        friend bool operator!=(wrapper w1, wrapper w2)
        {
                return !(w1 == w2);
        }

private:
        T _v{};
};

#define OP(Op)                                                                                      \
template<typename T>                                                                      \
wrapper<T> operator Op (wrapper<T> w1, wrapper<T> w2) \
{                                                                                                                  \
        w1 Op##= w2;                                                                               \
        return w1;                                                                                       \
}

OP(|)
OP(&)
OP(^)
#undef OP

template<typename T>
wrapper<T> operator<<(wrapper<T> w1, size_t s)
{
        w1 <<= s;
        return w1;
}

template<typename T>
wrapper<T> operator>>(wrapper<T> w1, size_t s)
{
        w1 >>= s;
        return w1;
}

}

}
