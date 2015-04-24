#pragma once

#include "boost/pool/pool_alloc.hpp"
#include "formula.hpp"
#include "identifiable.hpp"
#include "frame.hpp"
#include "model.hpp"
#include "minisat/core/Solver.h"
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

using Stack = std::stack<Frame, std::deque<Frame, boost::fast_pool_allocator<Frame>>>;
using Clause = Minisat::vec<Minisat::Lit>;

class Solver
{
public:
        enum class Result : uint8_t
        {
                UNDEFINED = 0,
                SATISFIABLE,
                UNSATISFIABLE
        };

        enum class State : uint8_t
        {
                UNINITIALIZED = 0,
                INITIALIZED,
                RUNNING,
                PAUSED,
                DONE
        };

        Solver() = delete;
        ~Solver() {}

        Solver(const Solver&) = delete;
        Solver(Solver&&) = delete;

        Solver& operator=(const Solver&) = delete;
        Solver& operator=(Solver&&) = delete;

        Solver(FormulaPtr formula, FrameID maximum_depth = FrameID::max(), uint32_t backtrack_probability = 100, uint32_t min_backtrack = 100, uint32_t max_backtrack = 100, bool use_sat = false);

        FormulaPtr inline Formula() const;

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

        inline uint8_t backtrack_probability() const
        {
                return _backtrack_probability;
        }

        inline uint8_t minimum_backtrack() const
        {
                return _minimum_backtrack;
        }

        inline uint8_t maximum_backtrack() const
        {
                return _maximum_backtrack;
        }

        Result solution();
        ModelPtr model();

private:
        FormulaPtr _formula;

        FrameID _maximum_depth;
        uint32_t _backtrack_probability;
        uint32_t _minimum_backtrack;
        uint32_t _maximum_backtrack;
        bool _use_sat_solver;

        State _state;
        Result _result;

        struct
        {
                Bitset atom;
                Bitset negation;
                Bitset tomorrow;
                Bitset always;
                Bitset eventually;
                Bitset conjunction;
                Bitset disjunction;
                Bitset until;
                Bitset not_until;
                Bitset iff;
                Bitset temporary;
        } _bitset;

        std::vector<FormulaID> _lhs;
        std::vector<FormulaID> _rhs;
        std::unordered_map<FormulaID, std::string> _atom_set;
        std::vector<uint64_t> _clause_size;
        std::vector<FormulaID> _fw_eventualities_lut;
        std::vector<FormulaID> _bw_eventualities_lut;
        std::vector<Clause> _clauses;

        size_t _number_of_formulas;
        FormulaID _start_index;
        FrameID _loop_state;

        std::vector<FormulaPtr> _subformulas;
        Stack _stack;

        std::mt19937 _mt;
        std::uniform_int_distribution<uint32_t> _backtrack_probability_rand;
        //std::uniform_int_distribution<uint32_t> _backtrack_percentage_rand;

        bool _has_eventually;
        bool _has_until;
        bool _has_not_until;

        void _initialize();

        void _add_formula_for_position(const FormulaPtr formula, FormulaID position, FormulaID lhs, FormulaID rhs);

        inline bool _check_contradiction_rule();
        inline bool _apply_conjunction_rule();
        inline bool _apply_always_rule();
        inline bool _apply_disjunction_rule();
        inline bool _apply_iff_rule();
        inline bool _apply_eventually_rule();
        inline bool _apply_until_rule();
        inline bool _apply_not_until_rule();

        inline void _rollback_to_latest_choice();
        inline void _update_eventualities_satisfaction();
        inline void _update_history();
    
        inline std::tuple<bool, FrameID> _check_loop_rule() const;
        inline bool _check_prune0_rule() const;
        inline bool _check_prune_rule() const;
        inline bool _check_my_prune() const;

        inline bool _should_use_sat_solver();
    
        void __dump_current_formulas() const;
        void __dump_current_eventualities() const;
        void __dump_eventualities(FrameID id) const;
};

}
}
 