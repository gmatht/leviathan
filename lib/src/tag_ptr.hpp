#pragma once

#include <cstdint>

namespace LTL {
namespace detail {

template <size_t V>
struct log2 {
  enum { value = log2<V / 2>::value + 1 };
};

template <>
struct log2<1> {
  enum { value = 0 };
};

template <typename T>
class tag_ptr {
public:
  tag_ptr() : _ptr(nullptr) {}
  tag_ptr(T *ptr, uint8_t value = 0) : _ptr(ptr) { data(value); }
  tag_ptr(const tag_ptr &o) : _ptr(o._ptr) {}
  virtual ~tag_ptr() {}
  tag_ptr &operator=(const tag_ptr &o) { _ptr = o._ptr; }
  operator bool() const
  {
    return static_cast<bool>(_ptr_bits & ~static_cast<uintptr_t>(data_mask));
  }
  T *get() const { return reinterpret_cast<T *>(_ptr_bits & ~data_mask); }
  void reset() { _ptr = nullptr; }
  void swap(tag_ptr &o)
  {
    T *tmp = _ptr;
    _ptr = o._ptr;
    o._ptr = tmp;
  }

  uint8_t data() const
  {
    return static_cast<uint8_t>(_ptr_bits & static_cast<uintptr_t>(data_mask));
  }
  void data(uint8_t value)
  {
    _ptr_bits = reinterpret_cast<uintptr_t>(get()) |
                static_cast<uintptr_t>(value & data_mask);
  };

  T &operator*() const { return *_ptr; }
  T *operator->() const { return _ptr; }
  static constexpr uint8_t data_bits = log2<alignof(T)>::value;
  static constexpr uint8_t data_mask = alignof(T) - (uint8_t)1;

private:
  union {
    T *_ptr;
    uintptr_t _ptr_bits;
  };
};

template <typename T, typename... Args>
tag_ptr<T> make_tag(Args &&... args)
{
  return tag_ptr<T>(std::forward<Args>(args)...);
}

template <typename T>
std::ostream &operator<<(std::ostream &os, tag_ptr<T> ptr)
{
  return os << ptr.get();
}

template <typename T>
void swap(tag_ptr<T> &p1, tag_ptr<T> &p2)
{
  p1.swap(p2);
}

template <typename T, typename U>
bool operator==(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() == rhs.get();
}

template <typename T, typename U>
bool operator!=(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() != rhs.get();
}

template <typename T, typename U>
bool operator<(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() < rhs.get();
}

template <typename T, typename U>
bool operator>(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() > rhs.get();
}

template <typename T, typename U>
bool operator<=(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() <= rhs.get();
}

template <typename T, typename U>
bool operator>=(const tag_ptr<T> &lhs, const tag_ptr<U> &rhs)
{
  return lhs.get() >= rhs.get();
}
}
}
