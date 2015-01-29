#pragma once

#include <cstdint>

namespace LTL
{
namespace detail
{

template<typename T>
class tag_ptr
{
public:
        tag_ptr() : _ptr(nullptr) {}
        tag_ptr(T* ptr, uint8_t value = 0) : _ptr(ptr) { data(value); }
        tag_ptr(const tag_ptr& o) : _ptr(o._ptr) {}
        virtual ~tag_ptr() {}

        tag_ptr& operator=(const tag_ptr& o) { _ptr = o.get(); }
        
        operator bool() const { return static_cast<bool>(static_cast<uint64_t>(_ptr) & ~static_cast<uint64_t>(3)); }
        T* get() const { return (T*)(static_cast<uint64_t>(_ptr) & ~static_cast<uint64_t>(3)); }

        void reset() { _ptr = nullptr; }
        void swap(tag_ptr& o)
        {
                T* tmp = _ptr;
                _ptr = o._ptr;
                o._ptr = tmp;
        }

        uint8_t data() const { return static_cast<uint8_t>(static_cast<uint64_t>(_ptr) & static_cast<uint64_t>(3)); }
        void data(uint8_t value) { _ptr = static_cast<T*>(static_cast<uint64_t>(get()) | static_cast<uint64_t>(value & static_cast<uint8_t>(3))); };

        T& operator*() const { return *_ptr; }
        T* operator->() const { return _ptr; }

private:
        T* _ptr;
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
