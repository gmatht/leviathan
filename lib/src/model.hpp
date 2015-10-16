#pragma once

#include <memory>
#include <vector>
#include <set>

namespace LTL
{
namespace detail
{

struct Literal
{
        Literal(const std::string& formula, bool positive = true) : _positive(positive), _atomic(formula) {}

        bool positive() const
        {
                return _positive;
        }

        bool negative() const
        {
                return !_positive;
        }

        std::string atomic_formula() const
        {
                return _atomic;
        }

        friend bool operator==(const Literal& l1, const Literal& l2)
        {
                return l1.positive() == l2.positive() && l1.atomic_formula() == l2.atomic_formula();
        }

        friend bool operator!=(const Literal& l1, const Literal& l2)
        {
                return !(l1 == l2);
        }

        friend bool operator<(const Literal& l1, const Literal& l2)
        {
                if (l1._atomic == l2._atomic)
                {
                        if (l1.positive() && l2.negative())
                                return true;
                        else
                                return false;
                }

                return std::lexicographical_compare(l1._atomic.begin(), l1._atomic.end(), l2._atomic.begin(), l2._atomic.end());
        }

private:
        bool _positive;
        std::string _atomic;
};

using State = std::set<Literal>;

struct Model
{
        std::vector<State> states;
        uint64_t loop_state{};
};

using ModelPtr = std::shared_ptr<Model>;

}
}
