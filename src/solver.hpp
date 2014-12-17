#pragma once

#include "boost/dynamic_bitset.hpp"
#include "formula.hpp"
#include <vector>
#include <tuple>
#include <limits>
#include <stack>
#include <queue>

namespace LTL
{
namespace detail
{

const uint64_t MAX_FRAME = std::numeric_limits<uint64_t>::max();
const uint64_t MAX_FORMULA = std::numeric_limits<uint64_t>::max();

using Bitset = boost::dynamic_bitset<uint64_t>;

template <class T, class S>
inline const S& Container(const std::stack<T, S>& s)
{
        struct Stack : private std::stack<T, S>
        {
                static const S& Container(const std::stack<T, S>& _s)
                {
                        return _s.*&Stack::c;
                }
        };
        return Stack::Container(s);
}

std::tuple<bool, std::vector<FormulaSet>, uint64_t> is_satisfiable(const FormulaPtr formula, bool model);

class Model;

class Solver
{
public:
        enum class Result : uint8_t
        {
                UNDEFINED,
                SATISFIABLE,
                UNSATISFIABLE
        };

        enum class State : uint8_t
        {
                INITIALIZED,
                RUNNING,
                DONE
        };

        Solver() = delete;
        ~Solver();

        Solver(const Solver&) = delete;
        Solver(const Solver&&) = delete;

        Solver& operator=(const Solver&) = delete;
        Solver& operator=(const Solver&&) = delete;

        Solver(FormulaPtr formula, uint64_t maximum_depth = std::numeric_limits<uint64_t>::max(), uint8_t backtrace_probability = 100, uint8_t backtrace_percentage = 100);

        inline State get_state() const
        {
                return _state;
        }

        inline Result is_satisfiable() const
        {
                return _result;
        }

        inline uint64_t maximum_depth() const
        {
                return _maximum_depth;
        }

        inline uint8_t backtrace_probability() const
        {
                return _backtrace_probability;
        }

        inline uint8_t backtrace_percentage() const
        {
                return _backtrace_percentage;
        }

        Result solve();
        Model get_model();

private:
        FormulaPtr _formula;

        uint64_t _maximum_depth;
        uint8_t _backtrace_probability;
        uint8_t _backtrace_percentage;

        State _state;
        Result _result;

        void _initialize();                      // Initialize the Solver state
        void _get_next_model();      // Start from the current Solver state and calculates a new model (if any)
};

}
}
