#pragma once

#include "boost/dynamic_bitset.hpp"
#include "boost/pool/pool_alloc.hpp"
#include "formula.hpp"
#include <vector>
#include <tuple>
#include <limits>
#include <stack>
#include <queue>
#include <unordered_map>
#include <random>

namespace LTL
{
namespace detail
{
/*
const uint64_t MAX_FRAME = std::numeric_limits<uint64_t>::max();
const uint64_t MAX_FORMULA = std::numeric_limits<uint64_t>::max();
*/
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

//std::tuple<bool, std::vector<FormulaSet>, uint64_t> is_satisfiable(const FormulaPtr formula, bool model);

/* New stuff */

template<typename Derived>
class Identifiable
{
public:
        constexpr Identifiable() : _id(0) {}
        constexpr explicit Identifiable(uint64_t id) : _id(id) {}
        constexpr Identifiable(const Derived& d) : _id(d._id) {}

        inline constexpr Derived& operator=(const Derived& d)
        {
                _id = d._id;
        }

        inline constexpr operator bool() const
        {
                return _id;
        }

        inline constexpr operator uint64_t() const
        {
                return _id;
        }

        inline constexpr friend bool operator==(const Derived& d1, const Derived& d2)
        {
                return d1._id == d2._id;
        }

        inline constexpr friend bool operator!=(const Derived& d1, const Derived& d2)
        {
                return d1._id != d2._id;
        }

        inline constexpr Derived& operator++()
        {
                ++_id;
                return *this;
        }

        inline constexpr Derived operator++(int)
        {
                Derived temp(*this);
                ++_id;
                return temp;
        }

        inline constexpr friend bool operator<(const Derived& d1, const Derived& d2)
        {
                return d1._id < d2._id;
        }

        inline constexpr friend bool operator>(const Derived& d1, const Derived& d2)
        {
                return d1._id > d2._id;
        }

        inline constexpr friend bool operator<=(const Derived& d1, const Derived& d2)
        {
                return d1._id <= d2._id;
        }

        inline constexpr friend bool operator>=(const Derived& d1, const Derived& d2)
        {
                return d1._id < d2._id;
        }

        template<typename T>
        inline constexpr friend Derived operator+(const Derived& d, T c)
        {
                return Derived(d._id + c);
        }

        inline static constexpr Derived max()
        {
                return Derived(std::numeric_limits<uint64_t>::max());
        }

        inline static constexpr Derived min()
        {
                return Derived(std::numeric_limits<uint64_t>::max());
        }

private:
        uint64_t _id;
};

class FrameID : public Identifiable<FrameID>
{
public:
        constexpr FrameID() {}
        constexpr explicit FrameID(uint64_t id) : Identifiable(id) {}
        constexpr FrameID(const FrameID& id) : Identifiable(id) {}
};

class FormulaID : public Identifiable<FormulaID>
{
public:
        constexpr FormulaID() {}
        constexpr explicit FormulaID(uint64_t id) : Identifiable(id) {}
        constexpr FormulaID(const FormulaID& id) : Identifiable(id) {}
};

}
}

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

namespace LTL
{
namespace detail
{

using Bitset = boost::dynamic_bitset<uint64_t>;

struct Frame
{
        Bitset formulas;
        Bitset toProcess;
        std::unordered_map<FormulaID, FrameID> eventualities;
        const FrameID id;
        FormulaID choosenFormula;
        bool choice;
        const Frame* chain;

        // Builds a frame with a single formula in it (represented by the index in the table) -> Start of the process
        Frame(const FrameID _id, const FormulaID _formula, uint64_t number_of_formulas)
                : formulas(number_of_formulas)
                , toProcess(number_of_formulas)
                , eventualities()
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(nullptr)
        {
                formulas.set(_formula);
                toProcess.set();
        }

        // Builds a frame with the same formulas of the given frame in it -> Choice point
        Frame(const FrameID _id, const Frame& _frame)
                : formulas(_frame.formulas)
                , toProcess(_frame.toProcess)
                , eventualities(_frame.eventualities)
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(_frame.chain)
        {
        }

        // Builds a frame with the given sets of eventualities (needs to be manually filled with the formulas) -> Step rule
        Frame(const FrameID _id, uint64_t number_of_formulas, const std::unordered_map<FormulaID, FrameID>& _eventualities, const Frame* chainPtr)
                : formulas(number_of_formulas)
                , toProcess(number_of_formulas)
                , eventualities(_eventualities)
                , id(_id)
                , choosenFormula(FormulaID::max())
                , choice(false)
                , chain(chainPtr)
        {
                toProcess.set();
        }
};

struct Model {};
using Stack = std::stack<Frame, std::deque<Frame, boost::fast_pool_allocator<Frame>>>;

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
                UNINITIALIZED,
                INITIALIZED,
                RUNNING,
                PAUSED,
                DONE
        };

        Solver() = delete;
        ~Solver() {}

        Solver(const Solver&) = delete;
        Solver(const Solver&&) = delete;

        Solver& operator=(const Solver&) = delete;
        Solver& operator=(const Solver&&) = delete;

        Solver(FormulaPtr formula, FrameID maximum_depth = FrameID::max(), uint32_t backtrace_probability = 100, uint32_t backtrace_percentage = 100);

        inline State state() const
        {
                return _state;
        }

        inline Result satisfiability() const
        {
                return _result;
        }

        inline FrameID maximum_depth() const
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

        Result solution();
        Model model();

private:
        FormulaPtr _formula;

        FrameID _maximum_depth;
        uint32_t _backtrace_probability;
        uint32_t _backtrace_percentage;

        State _state;
        Result _result;

        struct
        {
                Bitset negation;
                Bitset tomorrow;
                Bitset always;
                Bitset eventually;
                Bitset conjunction;
                Bitset disjunction;
                Bitset until;
                Bitset not_until;
                Bitset temporary;
        } _bitset;

        std::vector<FormulaID> _lhs;
        std::vector<FormulaID> _rhs;
        std::unordered_map<FormulaID, std::string> _atom_set;

        size_t _number_of_formulas;
        FormulaID _start_index;

        std::vector<FormulaPtr> _subformulas;
        Stack _stack;

        std::mt19937 _mt;
        std::uniform_int_distribution<uint32_t> _rand;

        void _initialize();                     // Initialize the Solver state
        void _add_formula_for_position(const FormulaPtr formula, FormulaID position, FormulaID lhs, FormulaID rhs);

        void _next_model();             // Start from the current Solver state and calculates a new model (if any)

        inline bool _check_contradiction_rule();
        inline bool _apply_conjunction_rule();
        inline bool _apply_always_rule();
        inline bool _apply_disjunction_rule();
        inline bool _apply_eventually_rule();
        inline bool _apply_until_rule();
        inline bool _apply_not_until_rule();

        inline void _rollback_to_latest_choice();
        inline void _update_eventualities_satisfaction();
};

}
}
 