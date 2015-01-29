#pragma once

#include <cstdint>
#include "utils/meta.h"

namespace LTL
{
namespace detail
{

template<typename T>
class tag_ptr
{
        static_assert(alignof(T) >= 4, "The type T must be aligned at least to 4 byte");

public:
        tag_ptr() : _ptr(nullptr) {}
        tag_ptr(T* ptr, uint8_t value = 0) : _ptr(ptr) { data(value); }
        tag_ptr(const tag_ptr& o) : _ptr(o._ptr) {}
        virtual ~tag_ptr() {}

        tag_ptr& operator=(const tag_ptr& o) { _ptr = o.get(); }
        
        operator bool() const { return static_cast<bool>(static_cast<uintptr_t>(_ptr) & ~static_cast<uintptr_t>(3)); }
        T* get() const { return reinterpret_cast<T*>(_ptr_bits & ~static_cast<uintptr_t>(3)); }

        void reset() { _ptr = nullptr; }
        void swap(tag_ptr& o)
        {
                T* tmp = _ptr;
                _ptr = o._ptr;
                o._ptr = tmp;
        }

        uint8_t data() const { return static_cast<uint8_t>(_ptr_bits & static_cast<uintptr_t>(3)); }
        void data(uint8_t value) { _ptr_bits = reinterpret_cast<uintptr_t>(get()) | static_cast<uintptr_t>(value & static_cast<uint8_t>(3)); };

        T& operator*() const { return *_ptr; }
        T* operator->() const { return _ptr; }

private:
        union
        {
                T* _ptr;
                uintptr_t _ptr_bits;
        };
};

template<typename T, typename... Args>
tag_ptr<T> make_tag(Args&&... args)
{
        return tag_ptr<T>(std::forward<Args>(args)...);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, tag_ptr<T> ptr)
{
        return os << ptr.get();
}

template<typename T>
void swap(tag_ptr<T>& p1, tag_ptr<T>& p2)
{
        p1.swap(p2);
}

template<typename T, typename U>
bool operator==(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() == rhs.get();
}

template<typename T, typename U>
bool operator!=(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() != rhs.get();
}

template<typename T, typename U>
bool operator<(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() < rhs.get();
}

template<typename T, typename U>
bool operator>(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() > rhs.get();
}

template<typename T, typename U>
bool operator<=(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() <= rhs.get();
}

template<typename T, typename U>
bool operator>=(const tag_ptr<T>& lhs, const tag_ptr<U>& rhs)
{
        return lhs.get() >= rhs.get();
}

}
}
