#pragma once

#include <cstdint>
#include <iostream>

#if __GNUC__ >= 5 || __clang__
#define CONSTEXPR  constexpr
#else
#define CONSTEXPR
#endif

namespace LTL
{
namespace detail
{

template<typename Derived>
class Identifiable
{
public:
        CONSTEXPR Identifiable() : _id(0) {}
        CONSTEXPR explicit Identifiable(uint64_t id) : _id(id) {}
        CONSTEXPR Identifiable(const Derived& d) : _id(d._id) {}

        inline CONSTEXPR Derived& operator=(const Derived& d)
        {
                _id = d._id;
		return *this;
        }

        inline CONSTEXPR operator bool() const
        {
                return static_cast<bool>(_id);
        }

        inline CONSTEXPR operator unsigned long() const
        {
                return static_cast<unsigned long>(_id);
        }

        inline CONSTEXPR operator unsigned long long() const
        {
                return static_cast<unsigned long long>(_id);
        }

        inline CONSTEXPR friend bool operator==(const Derived& d1, const Derived& d2)
        {
                return d1._id == d2._id;
        }

        inline CONSTEXPR friend bool operator!=(const Derived& d1, const Derived& d2)
        {
                return d1._id != d2._id;
        }

        inline CONSTEXPR Derived& operator++()
        {
                ++_id;
                return *this;
        }

        inline CONSTEXPR Derived operator++(int)
        {
                Derived temp(*this);
                ++_id;
                return temp;
        }

        inline CONSTEXPR Derived& operator--()
        {
                --_id;
                return *this;
        }

        inline CONSTEXPR Derived operator--(int)
        {
                Derived temp(*this);
                --_id;
                return temp;
        }

        inline CONSTEXPR friend bool operator<(const Derived& d1, const Derived& d2)
        {
                return d1._id < d2._id;
        }

        inline CONSTEXPR friend bool operator>(const Derived& d1, const Derived& d2)
        {
                return d1._id > d2._id;
        }

        inline CONSTEXPR friend bool operator<=(const Derived& d1, const Derived& d2)
        {
                return d1._id <= d2._id;
        }

        inline CONSTEXPR friend bool operator>=(const Derived& d1, const Derived& d2)
        {
                return d1._id >= d2._id;
        }

        template<typename T>
        inline CONSTEXPR friend Derived operator+(const Derived& d, T c)
        {
                return Derived(d._id + c);
        }

        template<typename T>
        inline CONSTEXPR friend Derived operator-(const Derived& d, T c)
        {
                return Derived(d._id - c);
        }

        inline static CONSTEXPR Derived max()
        {
                return Derived(std::numeric_limits<uint64_t>::max());
        }

        inline static CONSTEXPR Derived min()
        {
                return Derived(std::numeric_limits<uint64_t>::max());
        }

private:
        uint64_t _id;
};

class FrameID : public Identifiable<FrameID>
{
public:
        CONSTEXPR FrameID() {}
        CONSTEXPR explicit FrameID(uint64_t id) : Identifiable(id) {}
        CONSTEXPR FrameID(const FrameID& id) : Identifiable(id) {}
};

class FormulaID : public Identifiable<FormulaID>
{
public:
        CONSTEXPR FormulaID() {}
        CONSTEXPR explicit FormulaID(uint64_t id) : Identifiable(id) {}
        CONSTEXPR FormulaID(const FormulaID& id) : Identifiable(id) {}
};

}
}

#undef CONSTEXPR

template<typename T>
std::ostream& operator<<(std::ostream& os, LTL::detail::Identifiable<T> id)
{
        return os << (uint64_t)id;
}

namespace std
{
        template<>
        struct hash<LTL::detail::FormulaID>
        {
                size_t operator()(LTL::detail::FormulaID id) const
                {
                        return hash<uint64_t>()(static_cast<uint64_t>(id));
                }
        };
}
