#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <initializer_list>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "wrapper.hpp"

#include <iostream>

namespace dynamic_bitset
{
namespace detail
{

#ifdef __EXCEPTIONS
#ifdef NDEBUG
#define throw_if(pred, exc, msg)
#else
#define throw_if(pred, exc, msg) \
        if (pred) \
                throw exc ( msg );
#endif
#else
#define throw_if(pred, exc, msg) \
        assert(!(pred) && #exc msg);
#endif

class dyn_bitset
{
        using T = uint32_t;

public:
        using size_type = std::size_t;

        template<typename Bitset>
        class reference_base
        {
        public:
                // TODO: Copy constructor

                operator bool() const noexcept
                {
                        return _ref._get(_pos);
                }

                bool operator~() const noexcept
                {
                        return !*this;
                }

        protected:
                friend Bitset;

                reference_base(Bitset& ref, size_type pos) : _ref(ref), _pos(pos) {}

                Bitset& _ref;
                size_type _pos;
        };

        class reference : public reference_base<dyn_bitset>
        {
                using reference_base<dyn_bitset>::reference_base;
                friend class dyn_bitset;

        public:
                reference& operator=(bool value) noexcept
                {
                        _ref._set(_pos, value);
                        return *this;
                }

                reference& flip() noexcept
                {
                        _ref._flip(_pos);
                        return *this;
                }
        };

        class const_reference : public reference_base<const dyn_bitset>
        {
                using reference_base<const dyn_bitset>::reference_base;
                friend class dyn_bitset;

                // TODO: Constructors
        };

        template<typename>
        friend class reference_base;
        
        explicit dyn_bitset(size_type size)
        {
                assert(size > 0);
                _size = size;
                _data = std::unique_ptr<wrapper<T>[]>(new wrapper<T>[get_elem_from_size(_size)]);
        }
        
        dyn_bitset(const dyn_bitset& other)
        {
                _size = other._size;
                _data = std::unique_ptr<wrapper<T>[]>(new wrapper<T>[get_elem_from_size(_size)]);
                std::copy(other.data_begin(), other.data_end(), data_begin());
        }

        dyn_bitset(dyn_bitset&& other)
        {
                swap(*this, other);
        }

        dyn_bitset& operator=(const dyn_bitset& other)
        {
                dyn_bitset b(other);
                swap(*this, b);
                return *this;
        }

        dyn_bitset& operator=(dyn_bitset&& other)
        {
                swap(*this, other);
                return *this;
        }

        dyn_bitset(std::initializer_list<bool> list)
        {
                assert(list.size() > 0);
                assert(std::all_of(list.begin(), list.end(), [] (T v) { return v == 0 || v == 1; }));

                _size = list.size();
                _data = std::unique_ptr<wrapper<T>[]>(new wrapper<T>[get_elem_from_size(_size)]);
                
                size_t i = 0;
                for (auto b : list)
                {
                        set(i, b);
                        ++i;
                }
        }

        friend void swap(dyn_bitset& first, dyn_bitset& second)
        {
                std::swap(first._size, second._size); 
                std::swap(first._data, second._data);
        }

        reference operator[](size_type pos)
        {
                assert(pos < _size);
                return reference(*this, pos);
        }

        const_reference operator[](size_type pos) const
        {
                assert(pos < _size);
                return const_reference(*this, pos);
        }

        const_reference test(size_type pos) const
        {
                throw_if(pos >= _size, std::out_of_range, "Requested position is out of range!")
                return const_reference(*this, pos);
        }

        size_type size() const
        {
                return _size;
        }

        size_type count() const
        {
                return std::accumulate(data_begin(), data_end(), static_cast<size_type>(0), [] (size_type s, wrapper<T> v) { return s + v.count(); });
        }

        size_type leading_zero() const
        {
                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                {
                        size_type lz = _data[i].leading_zero();
                        if (element_bits != lz)
                                return lz + (element_bits * i);
                }

                return _size;
        }

        size_type trailing_zero() const
        {
                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                {
                        size_type lz = _data[get_elem_from_size(_size) - 1 - i].trailing_zero();
                        if (element_bits != lz)
                                return lz + (element_bits * i);
                }

                return _size;
        }

        bool all() const
        {
                if (!std::all_of(data_begin(), data_end() - 1, std::mem_fn(&wrapper<T>::all)))
                    return false;

                wrapper<T>& w = *(data_end() - 1);
                for (size_t i = 0; i < _size % element_bits; ++i)
                {
                        if (!w.get(i))
                                return false;
                }

                return true;
        }

        bool any() const
        {
                if (std::any_of(data_begin(), data_end() - 1, std::mem_fn(&wrapper<T>::any)))
                        return true;

                wrapper<T>& w = *(data_end() - 1);
                for (size_t i = 0; i < _size % element_bits; ++i)
                {
                        if (w.get(i))
                                return true;
                }

                return false;
        }

        bool none() const
        {
                if (!std::all_of(data_begin(), data_end() -1, std::mem_fn(&wrapper<T>::none)))
                        return false;

                wrapper<T>& w = *(data_end() - 1);
                for (size_t i = 0; i < _size % element_bits; ++i)
                {
                        if (w.get(i))
                                return false;
                }

                return true;
        }

        void set(size_type i, bool value = true)
        {
                throw_if(i >= _size, std::out_of_range, "Requested position is out of range!")
                _set(i, value);
        }

        void set()
        {
                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i].set();
        }

        void reset(size_type i)
        {
                throw_if(i >= _size, std::out_of_range, "Requested position is out of range!")
                _set(i, false);
        }

        void reset()
        {
                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i].reset();
        }

        void flip()
        {
                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i].flip();
        }

        void flip(size_type i)
        {
                throw_if(i >= _size, std::out_of_range, "Requested position is out of range!")
                _flip(i);   
        }

        std::string to_string(char zero = '0', char one = '1') const
        {
                std::string res(_size, zero);
                for (size_t i = 0; i < _size; ++i)
                {
                        if (_get(_size - i - 1))
                                res[i] = one;
                }

                return res;
        }

        dyn_bitset& operator&=(const dyn_bitset& other)
        {
                throw_if(_size != other._size, std::length_error, "Cannot & bitsets of different sizes!")

                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i] &= other._data[i];

                return *this;
        }

        dyn_bitset& operator|=(const dyn_bitset& other)
        {
                throw_if(_size != other._size, std::length_error, "Cannot | bitsets of different sizes!")

                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i] |= other._data[i];

                return *this;
        }

        dyn_bitset& operator^=(const dyn_bitset& other)
        {
                throw_if(_size != other._size, std::length_error, "Cannot ^ bitsets of different sizes!")

                for (size_t i = 0; i < get_elem_from_size(_size); ++i)
                        _data[i] ^= other._data[i];

                return *this;
        }

        dyn_bitset operator~() const
        {
                dyn_bitset res(*this);
                res.flip();
                return res;
        }

        friend bool operator==(const dyn_bitset& b1, const dyn_bitset& b2)
        {
                throw_if(b1._size != b2._size, std::length_error, "Cannot == bitsets of different sizes!")

                for (size_t i = 0; i < get_elem_from_size(b1._size) - 1; ++i)
                {
                        if (b1._data[i] != b2._data[i])
                                return false;
                }

                wrapper<T>& w1 = *(b1.data_end() - 1);
                wrapper<T>& w2 = *(b2.data_end() - 1);
                for (size_t i = 0; i < b1._size % element_bits; ++i)
                {
                        if (w1.get(i) != w2.get(i))
                                return false;
                }

                return true;
        }

        friend bool operator!=(const dyn_bitset& b1, const dyn_bitset& b2)
        {
                throw_if(b1._size != b2._size, std::length_error, "Cannot != bitsets of different sizes!")

                return !(b1 == b2);
        }

        dyn_bitset& operator>>=(size_t shift)
        {
                bool c = false;
                for (int32_t i = get_elem_from_size(_size) - 1; i >= 0; --i)
                {
                        bool t = _data[i].get(0);
                        _data[i] >>= shift;
                        _data[i].set(element_bits - 1, c);
                        c = t;
                }

                return *this;
        }

        dyn_bitset& operator<<=(size_t shift)
        {
                bool c = false;
                for (int32_t i  = 0; i < get_elem_from_size(_size); ++i)
                {
                        bool t = _data[i].get(element_bits - 1);
                        _data[i] <<= shift;
                        _data[i].set(0, c);
                        c = t;
                }

                return *this;
        }

private:
        static constexpr size_type element_bits = wrapper<T>::bits;

        static size_type get_elem_from_size(size_type size)
        {
                return std::ceil(static_cast<float>(size) / element_bits);
        }

        std::unique_ptr<wrapper<T>[]> _data;
        size_type _size;

        bool _get(size_type i) const
        {
                return _data[i / element_bits].get(i % element_bits);
        }

        void _set(size_type i, bool value)
        {
                _data[i / element_bits].set(i % element_bits, value);
        }

        void _flip(size_type i)
        {
                return _data[i / element_bits].flip(i % element_bits);
        }

        wrapper<T>* data_begin() const
        {
                return _data.get();
        }

        wrapper<T>* data_end() const
        {
                return _data.get() + get_elem_from_size(_size);
        }
};

inline dyn_bitset operator&(const dyn_bitset& b1, const dyn_bitset& b2)
{
        dyn_bitset res(b1);
        return res &= b2;
}

inline dyn_bitset operator|(const dyn_bitset& b1, const dyn_bitset& b2)
{
        dyn_bitset res(b1);
        return res |= b2;
}

inline dyn_bitset operator^(const dyn_bitset& b1, const dyn_bitset& b2)
{
        dyn_bitset res(b1);
        return res ^= b2;
}

inline dyn_bitset operator>>(const dyn_bitset& b, size_t shift)
{
        dyn_bitset o(b);
        return o >>= shift;
}

inline dyn_bitset operator<<(const dyn_bitset& b, size_t shift)
{
        dyn_bitset o(b);
        return o <<= shift;
}

inline std::ostream& operator<<(std::ostream& os, const dyn_bitset& b)
{
        return os << b.to_string();
}

#undef throw_if

}

using detail::dyn_bitset;

}
