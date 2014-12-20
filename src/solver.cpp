#include "solver.hpp"

#include "generator.hpp"
#include <stack>
#include <iostream>
#include <cassert>
#include <csignal>
#include <atomic>

namespace LTL
{

namespace detail
{

Solver::Solver(FormulaPtr formula, FrameID maximum_depth, uint32_t backtrace_probability, uint32_t backtrace_percentage) : _start_index(0)
{
        _formula = formula;
        _maximum_depth = maximum_depth;
        _backtrace_probability = backtrace_probability;
        _backtrace_percentage = backtrace_percentage;
        // TODO: Notice the user of the choosen parameters don't make sense and we switched to default

        _state = State::UNINITIALIZED;
        _result = Result::UNDEFINED;

        _mt = std::mt19937((std::random_device())());
        _rand = std::uniform_int_distribution<uint32_t>(0, 100);

        _initialize();
}

void Solver::_initialize()
{
        _atom_set.clear();

        Simplifier simplifier;
        _formula = simplifier.simplify(_formula);

        Generator gen;
        gen.generate(_formula);
        _subformulas = gen.formulas();

        if (_subformulas.size() == 1)
        {
                if (isa<True>(_subformulas[0]))
                {
                        _result = Result::SATISFIABLE;
                        _state = State::DONE;
                        return;
                }
                else if (isa<False>(_subformulas[0]))
                {
                        _result = Result::UNSATISFIABLE;
                        _state = State::DONE;
                        return;
                }
        }

        std::function<bool(FormulaPtr, FormulaPtr)> compareFunc = [&compareFunc](const FormulaPtr a, const FormulaPtr b)
        {
        if (isa<Atom>(a) && isa<Atom>(b))
            return std::lexicographical_compare(fast_cast<Atom>(a)->name().begin(), fast_cast<Atom>(a)->name().end(),
                                                fast_cast<Atom>(b)->name().begin(), fast_cast<Atom>(b)->name().end());
        
        if (isa<Negation>(a) && isa<Negation>(b))
            return compareFunc(fast_cast<Negation>(a)->formula(), fast_cast<Negation>(b)->formula());
        
        if (isa<Negation>(a))
        {
            if (fast_cast<Negation>(a)->formula() == b)
                return false;
            
            return compareFunc(fast_cast<Negation>(a)->formula(), b);
        }
        
        if (isa<Negation>(b))
        {
            if (fast_cast<Negation>(b)->formula() == a)
                return true;
            
            return compareFunc(a, fast_cast<Negation>(b)->formula());
        }
        
        if (isa<Tomorrow>(a) && isa<Tomorrow>(b))
            return compareFunc(fast_cast<Tomorrow>(a)->formula(), fast_cast<Tomorrow>(b)->formula());
        
        if (isa<Tomorrow>(a))
        {
            if (fast_cast<Tomorrow>(a)->formula() == b)
                return false;
            
            return compareFunc(fast_cast<Tomorrow>(a)->formula(), b);
        }
        
        if (isa<Tomorrow>(b))
        {
            if (fast_cast<Tomorrow>(b)->formula() == a)
                return true;
            
            return compareFunc(a, fast_cast<Tomorrow>(b)->formula());
        }
        
        if (isa<Always>(a) && isa<Always>(b))
            return compareFunc(fast_cast<Always>(a)->formula(), fast_cast<Always>(b)->formula());
        
        if (isa<Eventually>(a) && isa<Eventually>(b))
            return compareFunc(fast_cast<Eventually>(a)->formula(), fast_cast<Eventually>(b)->formula());
        
        if (isa<Conjunction>(a) && isa<Conjunction>(b))
        {
            if (fast_cast<Conjunction>(a)->left() != fast_cast<Conjunction>(b)->left())
                return compareFunc(fast_cast<Conjunction>(a)->left(), fast_cast<Conjunction>(b)->left());
            else
                return compareFunc(fast_cast<Conjunction>(a)->right(), fast_cast<Conjunction>(b)->right());
        }
        
        if (isa<Disjunction>(a) && isa<Disjunction>(b))
        {
            if (fast_cast<Disjunction>(a)->left() != fast_cast<Disjunction>(b)->left())
                return compareFunc(fast_cast<Disjunction>(a)->left(), fast_cast<Disjunction>(b)->left());
            else
                return compareFunc(fast_cast<Disjunction>(a)->right(), fast_cast<Disjunction>(b)->right());
        }
        
        if (isa<Until>(a) && isa<Until>(b))
        {
            if (fast_cast<Until>(a)->left() != fast_cast<Until>(b)->left())
                return compareFunc(fast_cast<Until>(a)->left(), fast_cast<Until>(b)->left());
            else
                return compareFunc(fast_cast<Until>(a)->right(), fast_cast<Until>(b)->right());
        }
        
        return a->type() < b->type();
        };

        std::sort(_subformulas.begin(), _subformulas.end(), compareFunc);

        auto last = std::unique(_subformulas.begin(), _subformulas.end());
        _subformulas.erase(last, _subformulas.end());

        FormulaID current_index(0);
    
        _number_of_formulas = _subformulas.size();
        _bitset.negation.resize(_number_of_formulas);
        _bitset.tomorrow.resize(_number_of_formulas);
        _bitset.always.resize(_number_of_formulas);
        _bitset.eventually.resize(_number_of_formulas);
        _bitset.conjunction.resize(_number_of_formulas);
        _bitset.disjunction.resize(_number_of_formulas);
        _bitset.until.resize(_number_of_formulas);
        _bitset.not_until.resize(_number_of_formulas);
        _bitset.temporary.resize(_number_of_formulas);
        
        _lhs = std::vector<FormulaID>(_number_of_formulas, FormulaID::max());
        _rhs = std::vector<FormulaID>(_number_of_formulas, FormulaID::max());

        for (auto& f : _subformulas)
        {
                if (f == _formula)
                        _start_index = current_index;

                FormulaID lhs(0), rhs(0);
                FormulaPtr left = nullptr, right = nullptr;
                if (isa<Negation>(f))
                {
                        if (isa<Until>(fast_cast<Negation>(f)->formula()))
                        {
                                left = simplifier.simplify(make_negation(fast_cast<Until>(fast_cast<Negation>(f)->formula())->left()));
                                right = simplifier.simplify(make_negation(fast_cast<Until>(fast_cast<Negation>(f)->formula())->right()));
                        }
                        else
                                left = fast_cast<Negation>(f)->formula();
                }
                else if (isa<Tomorrow>(f))
                        left = fast_cast<Tomorrow>(f)->formula();
                else if (isa<Always>(f))
                        left = fast_cast<Always>(f)->formula();
                else if (isa<Eventually>(f))
                        left = fast_cast<Eventually>(f)->formula();
                else if (isa<Conjunction>(f))
                {
                        left = fast_cast<Conjunction>(f)->left();
                        right = fast_cast<Conjunction>(f)->right();
                }
                else if (isa<Disjunction>(f))
                {
                        left = fast_cast<Disjunction>(f)->left();
                        right = fast_cast<Disjunction>(f)->right();
                }
                else if (isa<Until>(f))
                {
                        left = fast_cast<Until>(f)->left();
                        right = fast_cast<Until>(f)->right();
                }

                if (left)
                {
                        lhs = FormulaID(static_cast<uint64_t>(std::find_if(_subformulas.begin(), _subformulas.end(), [&](FormulaPtr a)
                        { return a == left; }) - _subformulas.begin()));
                        assert(std::find_if(_subformulas.begin(), _subformulas.end(), [&](FormulaPtr a)
                        { return a == left; }) != _subformulas.end());
                }
                if (right)
                {
                        rhs = FormulaID(static_cast<uint64_t>(std::find_if(_subformulas.begin(), _subformulas.end(), [&](FormulaPtr a)
                        { return a == right; }) - _subformulas.begin()));
                        assert(std::find_if(_subformulas.begin(), _subformulas.end(), [&](FormulaPtr a)
                        { return a == right; }) != _subformulas.end());
                }

                _add_formula_for_position(f, current_index++, lhs, rhs);
        }

        _stack.emplace(FrameID(0), _start_index, _number_of_formulas);
        _state = State::INITIALIZED;
}

void Solver::_add_formula_for_position(const FormulaPtr formula, FormulaID position, FormulaID lhs, FormulaID rhs)
{
         switch (formula->type())
        {
                case Formula::Type::Atom:
                        _atom_set[position] = fast_cast<Atom>(formula)->name();
                        break;

                case Formula::Type::Negation:
                        if (isa<Until>(fast_cast<Negation>(formula)->formula()))
                        {
                                _bitset.not_until[position] = true;
                                _lhs[position] = lhs;
                                _rhs[position] = rhs;
                                break;
                        }
                        _bitset.negation[position] = true;
                        _lhs[position] = lhs;
                        break;

                case Formula::Type::Tomorrow:
                        _bitset.tomorrow[position] = true;
                        _lhs[position] = lhs;
                        break;

                case Formula::Type::Always:
                        _bitset.always[position] = true;
                        _lhs[position] = lhs;
                        break;

                case Formula::Type::Eventually:
                        _bitset.eventually[position] = true;
                        _lhs[position] = lhs;
                        break;

                case Formula::Type::Conjunction:
                        _bitset.conjunction[position] = true;
                        _lhs[position] = lhs;
                        _rhs[position] = rhs;
                        break;

                case Formula::Type::Disjunction:
                        _bitset.disjunction[position] = true;
                        _lhs[position] = lhs;
                        _rhs[position] = rhs;
                        break;

                case Formula::Type::Until:
                        _bitset.until[position] = true;
                        _lhs[position] = lhs;
                        _rhs[position] = rhs;
                        break;

                case Formula::Type::True:
                case Formula::Type::False:
                case Formula::Type::Iff:
                case Formula::Type::Then:
                        assert(false);
                        break;
        }
}

bool Solver::_check_contradiction_rule()
{
        // TODO: Check if operator >>= works the same for 
        const Frame& frame = _stack.top();
        _bitset.temporary = frame.formulas;
        _bitset.temporary &= _bitset.negation;
        _bitset.temporary >>= 1;
        _bitset.temporary &= frame.formulas;
        return _bitset.temporary.any();
}

bool Solver::_apply_conjunction_rule()
{
        Frame& frame = _stack.top();
        _bitset.temporary = frame.formulas;
        _bitset.temporary &= _bitset.conjunction;
        _bitset.temporary &= frame.to_process;

        // TODO: This could be avoided as it's ~ a duplicate of find_first(), check if it is worth
        if (!_bitset.temporary.any())
                return false;

        // TODO: find_first and find_next don't use __builtin_clz/__builtin_ctz, find if a custom implementation using them is faster
        size_t one = _bitset.temporary.find_first();
        while (one != Bitset::npos)
        {
                assert(_bitset.conjunction[one]);
                assert(frame.formulas[one]);
                assert(frame.to_process[one]);

                frame.formulas[_lhs[one]] = true;
                frame.formulas[_rhs[one]] = true;
                frame.to_process[one] = false;
                one = _bitset.temporary.find_next(one + 1);
        }

        return true;
}

bool Solver::_apply_always_rule()
{
        Frame& frame = _stack.top();
        _bitset.temporary = frame.formulas;
        _bitset.temporary &= _bitset.always;
        _bitset.temporary &= frame.to_process;

        if (!_bitset.temporary.any())
                return false;

        size_t one = _bitset.temporary.find_first();
        while (one != Bitset::npos)
        {
                assert(_bitset.always[one]);
                assert(frame.formulas[one]);
                assert(frame.to_process[one]);

                frame.formulas[_lhs[one]] = true;
                assert(_bitset.tomorrow[one + 1] && _lhs[one + 1] == FormulaID(one));
                frame.formulas[one + 1] = true;
                frame.to_process[one] = false;
                one = _bitset.temporary.find_next(one + 1);
        }

        return true;
}

#define APPLY_RULE(rule) \
bool Solver::_apply_##rule##_rule() \
{ \
        Frame& frame = _stack.top(); \
        _bitset.temporary = frame.formulas; \
        _bitset.temporary &= _bitset.rule; \
        _bitset.temporary &= frame.to_process; \
\
        size_t one = _bitset.temporary.find_first(); \
        if (one != Bitset::npos) \
        { \
                assert(_bitset.rule[one]); \
                assert(frame.formulas[one]); \
                assert(frame.to_process[one]); \
\
                frame.to_process[one] = false; \
                frame.choosenFormula = FormulaID(one); \
                frame.choice = true; \
                return true; \
        } \
\
        return false; \
}

APPLY_RULE(disjunction)
APPLY_RULE(eventually)
APPLY_RULE(until)
APPLY_RULE(not_until)

#undef APPLY_RULE

Solver::Result Solver::solution()
{
        if (_state == State::RUNNING)
                return _result;

        _state = State::RUNNING;
        bool rules_applied;

loop:
        while (!_stack.empty())
        {
                Frame& frame = _stack.top();

                rules_applied = true;
                while (rules_applied)
                {
                        rules_applied = false;

                        if (__builtin_expect(frame.formulas.none(), 0))
                        {
                                _state = State::PAUSED;
                                _result = Result::SATISFIABLE;
                                return _result;
                        }

                        if (_check_contradiction_rule())
                        {
                                _rollback_to_latest_choice();
                                goto loop;
                        }

                        if (_apply_conjunction_rule())
                                rules_applied = true;
                        if (_apply_always_rule())
                                rules_applied = true;

                        if (_apply_disjunction_rule())
                        {
                                Frame new_frame(frame.id, frame);
                                new_frame.formulas[_lhs[frame.choosenFormula]] = true;
                                _stack.push(new_frame);

                                goto loop;
                        }

                        if (_apply_eventually_rule())
                        {
                                if (__builtin_expect(frame.eventualities.find(_lhs[frame.choosenFormula]) == frame.eventualities.end(), 0))
                                        frame.eventualities[_lhs[frame.choosenFormula]] = FrameID::max();

                                Frame new_frame(frame.id, frame);
                                new_frame.formulas[_lhs[frame.choosenFormula]] = true;
                                _stack.push(new_frame);

                                goto loop;
                        }

                        if (_apply_until_rule())
                        {
                                // TODO: Investigate if this is the right place to generate an eventuality
                                if (__builtin_expect(frame.eventualities.find(_rhs[frame.choosenFormula]) == frame.eventualities.end(), 0))
                                        frame.eventualities[_rhs[frame.choosenFormula]] = FrameID::max();

                                Frame new_frame(frame.id, frame);
                                new_frame.formulas[_rhs[frame.choosenFormula]] = true;
                                _stack.push(new_frame);

                                goto loop;
                        }

                        if (_apply_not_until_rule())
                        {
                                // TODO: This should generate an eventuality
                                Frame new_frame(frame.id, frame);
                                new_frame.formulas[_lhs[frame.choosenFormula]] = true;
                                new_frame.formulas[_rhs[frame.choosenFormula]] = true;
                                _stack.push(new_frame);

                                goto loop;
                        }
                }

                _update_eventualities_satisfaction();

                // LOOP rule
                const Frame* repFrame1 = nullptr, *repFrame2 = nullptr;
                const Frame* currFrame = frame.chain;

                //FrameID min_frame;

                // Heuristics: OCCASIONAL LOOKBACK
                if (_rand(_mt) > _backtrace_probability)
                        goto step_rule;
                
                // Heuristics: PARTIAL LOOKBACK
                // TODO: Not worth to lookback a certain %, move to min/max range lookback
                //min_frame = FrameID(static_cast<uint64_t>(static_cast<float>(_rand(_mt)) / 100.f * static_cast<uint64_t>(currFrame->id)));

                while (currFrame)
                {
                        // Heuristics: PARTIAL LOOKBACK
                        //if (currFrame->id < min_frame)
                                //break;

                        if (frame.formulas.is_subset_of(currFrame->formulas))
                        {
                                // TODO: Can this be done in a cheaper way? Probably yes
                                // if (eventualities_satisfied(frame, currFrame))
                                if (std::all_of(currFrame->eventualities.begin(), currFrame->eventualities.end(), [&] (const std::pair<FormulaID, FrameID>& p)
                                {
                                        const auto& ev = frame.eventualities.find(p.first);
                                        return ev->second != FrameID::max() && ev->second >= currFrame->id;
                                }))
                                {
                                        _result = Result::SATISFIABLE;
                                        _state = State::PAUSED;
                                        return _result;
                                }
                                
                                if (frame.formulas == currFrame->formulas) // STEP rule check
                                {
                                        if (!repFrame1)
                                                repFrame1 = currFrame;
                                        else if (!repFrame2)
                                                repFrame2 = currFrame;
                                }
                                //if (!frame.formulas.is_proper_subset_of(currFrame->formulas)) // Alternative: seems to be completely the same in terms of performance
                        }
                        currFrame = currFrame->chain;
                }

                // REP rule application
                if (repFrame1 && repFrame2)
                {
                        _rollback_to_latest_choice();
                        goto loop;
                }

// Heuristics: OCCASIONAL LOOKBACK
step_rule:

                if (__builtin_expect(frame.id >= _maximum_depth, 0))
                {
                        _rollback_to_latest_choice();
                        goto loop;
                }

                // STEP rule
                Frame new_frame(frame.id + 1, _number_of_formulas, frame.eventualities, &frame);
                _bitset.temporary = frame.formulas;
                _bitset.temporary &= _bitset.tomorrow;

                /* TODO: This doesn't work for w/e reason. Investigate
                size_t p = _bitset.temporary.find_first();
                while (p != Bitset::npos)
                {
                        assert(_bitset.tomorrow[p]);
                        assert(frame.formulas[p]);
                
                        new_frame.formulas[_lhs[p]] = true;
                        p = _bitset.temporary.find_next(p + 1);
                }
                */
                
                for (uint64_t i = 0; i < _number_of_formulas; ++i)
                {
                        if (_bitset.temporary[i])
                        {
                                assert(frame.formulas[i]);
                                assert(_bitset.tomorrow[i]);
                                new_frame.formulas[_lhs[i]] = true;
                        }
                }

                _stack.push(new_frame);
        }

        _state = State::DONE;
        if (_result == Result::UNDEFINED)
                _result = Result::UNSATISFIABLE;

        return _result;
}

 void Solver::_update_eventualities_satisfaction()
 {
        Frame& frame = _stack.top();
        for (auto& ev : frame.eventualities)
        {
                if (frame.formulas[ev.first])
                        ev.second = frame.id;
        }
 }

void Solver::_rollback_to_latest_choice()
{
        while (!_stack.empty())
        {
                if (_stack.top().choice && _stack.top().choosenFormula != FormulaID::max())
                {
                        Frame& top = _stack.top();;
                        Frame new_frame(top.id, top);

                        if (_bitset.disjunction[top.choosenFormula])
                                new_frame.formulas[_rhs[top.choosenFormula]] = true;
                        else if (_bitset.eventually[top.choosenFormula])
                        {
                                new_frame.formulas[top.choosenFormula + 1] = true;
                                assert(_bitset.tomorrow[top.choosenFormula + 1] && _lhs[top.choosenFormula + 1] == top.choosenFormula);
                        }
                        else if (_bitset.until[top.choosenFormula])
                        {
                                new_frame.formulas[_lhs[top.choosenFormula]] = true;
                                if (_bitset.tomorrow[top.choosenFormula + 1])
                                {
                                        new_frame.formulas[top.choosenFormula + 1] = true;
                                        assert(_lhs[top.choosenFormula + 1] == top.choosenFormula);
                                }
                                else
                                {
                                        new_frame.formulas[top.choosenFormula + 2] = true;
                                        assert(_lhs[top.choosenFormula + 2] == top.choosenFormula);
                                }
                        }
                        else if (_bitset.not_until[top.choosenFormula])
                        {
                                new_frame.formulas[_rhs[top.choosenFormula]] = true;
                                if (_bitset.tomorrow[top.choosenFormula + 1])
                                {
                                        new_frame.formulas[top.choosenFormula + 1] = true;
                                        assert(_lhs[top.choosenFormula + 1] == top.choosenFormula);
                                }
                                else
                                {
                                        new_frame.formulas[top.choosenFormula + 2] = true;
                                        assert(_lhs[top.choosenFormula + 2] == top.choosenFormula);
                                }
                        }
                        else
                        {
                                assert(false);
                        }

                        top.choosenFormula = FormulaID::max();
                        _stack.push(new_frame);

                        return;
                }

                _stack.pop();
        }
}

Model Solver::model()
{
        // TODO: Optimize model to compensate heuristics
        /* Heuristics: OPTIMIZE MODEL
        // TODO: Optimize away every 2 equal sets BEFORE the loop state
        std::vector<int64_t> toRemove;
        for (int64_t i = loopTo; i >= 0; --i) 
        {
                    if (std::all_of(model[loopTo].begin(), model[loopTo].end(), [&] (LTL::FormulaPtr f)
                    {
                            if (model[i].find(f) != model[i].end())
                                    return true;
                            return false;
                    }))
                    toRemove.push_back(i);
        }

        for (auto i : toRemove)
                model.erase(model.begin() + i);
        */

        return {};
}

/* Old implementation */
/*
static size_t number_of_formulas = 0;

struct Frame
{
        Bitset formulas;
        Bitset toProcess;
        std::unordered_map<uint64_t, uint64_t> eventualities;
        const uint64_t id;
        uint64_t choosenFormula;
        bool choice;
        const Frame* chain;

        // Builds a frame with a single formula in it (represented by the index in the table) -> Start of the process
        Frame(const uint64_t _id, const uint64_t _formula)
                : formulas(number_of_formulas)
                , toProcess(number_of_formulas)
                , eventualities()
                , id(_id)
                , choosenFormula(MAX_FORMULA)
                , choice(false)
                , chain(nullptr)
        {
                formulas.set(_formula);
                toProcess.set();
        }

        // Builds a frame with the same formulas of the given frame in it -> Choice point
        Frame(const uint64_t _id, const Frame& _frame)
                : formulas(_frame.formulas)
                , toProcess(_frame.toProcess)
                , eventualities(_frame.eventualities)
                , id(_id)
                , choosenFormula(MAX_FORMULA)
                , choice(false)
                , chain(_frame.chain)
        {
        }

        // Builds a frame with the given sets of eventualities (needs to be manually filled with the formulas) -> Step rule
        Frame(const uint64_t _id, const std::unordered_map<uint64_t, uint64_t>& _eventualities, const Frame* chainPtr)
                : formulas(number_of_formulas)
                , toProcess(number_of_formulas)
                , eventualities(_eventualities)
                , id(_id)
                , choosenFormula(MAX_FORMULA)
                , choice(false)
                , chain(chainPtr)
        {
                toProcess.set();
        }
};

static Simplifier simplifier;
static std::unordered_map<uint64_t, std::string> atom_set;

static Bitset not_bitset(0);
static Bitset tom_bitset(0);
static Bitset alw_bitset(0);
static Bitset ev_bitset(0);
static Bitset and_bitset(0);
static Bitset or_bitset(0);
static Bitset until_bitset(0);
static Bitset nuntil_bitset(0);
static Bitset res(0);

static std::vector<uint64_t> lhs_set(0, MAX_FORMULA);
static std::vector<uint64_t> rhs_set(0, MAX_FORMULA);

static void add_formula_to_bitset(const FormulaPtr f, uint64_t pos, uint64_t lhs, uint64_t rhs)
{
        switch (f->type())
        {
                case Formula::Type::Atom:
                        atom_set[pos] = fast_cast<Atom>(f)->name();
                        break;

                case Formula::Type::Negation:
                        if (isa<Until>(fast_cast<Negation>(f)->formula()))
                        {
                                nuntil_bitset[pos] = true;
                                lhs_set[pos] = lhs;
                                rhs_set[pos] = rhs;
                                break;
                        }
                        not_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Type::Tomorrow:
                        tom_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Type::Always:
                        alw_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Type::Eventually:
                        ev_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Type::Conjunction:
                        and_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                case Formula::Type::Disjunction:
                        or_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                case Formula::Type::Until:
                        until_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                case Formula::Type::True:
                case Formula::Type::False:
                case Formula::Type::Iff:
                case Formula::Type::Then:
                        assert(false);
                        break;
        }
}

static std::tuple<std::vector<FormulaPtr>, uint64_t> initialize(const FormulaPtr f)
{
        if (isa<True>(f))
                return std::tuple<std::vector<FormulaPtr>, uint64_t>{ { make_true() }, 0 };
        if (isa<False>(f))
                return std::tuple<std::vector<FormulaPtr>, uint64_t>{ { make_false() }, 0 };

        Generator gen;
        gen.generate(f);
        std::vector<FormulaPtr> formulas = gen.formulas();

        std::function<bool(FormulaPtr, FormulaPtr)> compareFunc = [&compareFunc](const FormulaPtr a, const FormulaPtr b)
        {
        if (isa<Atom>(a) && isa<Atom>(b))
            return std::lexicographical_compare(fast_cast<Atom>(a)->name().begin(), fast_cast<Atom>(a)->name().end(),
                                                fast_cast<Atom>(b)->name().begin(), fast_cast<Atom>(b)->name().end());
        
        if (isa<Negation>(a) && isa<Negation>(b))
            return compareFunc(fast_cast<Negation>(a)->formula(), fast_cast<Negation>(b)->formula());
        
        if (isa<Negation>(a))
        {
            if (fast_cast<Negation>(a)->formula() == b)
                return false;
            
            return compareFunc(fast_cast<Negation>(a)->formula(), b);
        }
        
        if (isa<Negation>(b))
        {
            if (fast_cast<Negation>(b)->formula() == a)
                return true;
            
            return compareFunc(a, fast_cast<Negation>(b)->formula());
        }
        
        if (isa<Tomorrow>(a) && isa<Tomorrow>(b))
            return compareFunc(fast_cast<Tomorrow>(a)->formula(), fast_cast<Tomorrow>(b)->formula());
        
        if (isa<Tomorrow>(a))
        {
            if (fast_cast<Tomorrow>(a)->formula() == b)
                return false;
            
            return compareFunc(fast_cast<Tomorrow>(a)->formula(), b);
        }
        
        if (isa<Tomorrow>(b))
        {
            if (fast_cast<Tomorrow>(b)->formula() == a)
                return true;
            
            return compareFunc(a, fast_cast<Tomorrow>(b)->formula());
        }
        
        if (isa<Always>(a) && isa<Always>(b))
            return compareFunc(fast_cast<Always>(a)->formula(), fast_cast<Always>(b)->formula());
        
        if (isa<Eventually>(a) && isa<Eventually>(b))
            return compareFunc(fast_cast<Eventually>(a)->formula(), fast_cast<Eventually>(b)->formula());
        
        if (isa<Conjunction>(a) && isa<Conjunction>(b))
        {
            if (fast_cast<Conjunction>(a)->left() != fast_cast<Conjunction>(b)->left())
                return compareFunc(fast_cast<Conjunction>(a)->left(), fast_cast<Conjunction>(b)->left());
            else
                return compareFunc(fast_cast<Conjunction>(a)->right(), fast_cast<Conjunction>(b)->right());
        }
        
        if (isa<Disjunction>(a) && isa<Disjunction>(b))
        {
            if (fast_cast<Disjunction>(a)->left() != fast_cast<Disjunction>(b)->left())
                return compareFunc(fast_cast<Disjunction>(a)->left(), fast_cast<Disjunction>(b)->left());
            else
                return compareFunc(fast_cast<Disjunction>(a)->right(), fast_cast<Disjunction>(b)->right());
        }
        
        if (isa<Until>(a) && isa<Until>(b))
        {
            if (fast_cast<Until>(a)->left() != fast_cast<Until>(b)->left())
                return compareFunc(fast_cast<Until>(a)->left(), fast_cast<Until>(b)->left());
            else
                return compareFunc(fast_cast<Until>(a)->right(), fast_cast<Until>(b)->right());
        }
        
        return a->type() < b->type();
        };

        std::sort(formulas.begin(), formulas.end(), compareFunc);

        auto last = std::unique(formulas.begin(), formulas.end());
        formulas.erase(last, formulas.end());

        uint64_t currentFormula = 0;
        uint64_t start = 0;
    
        number_of_formulas = formulas.size();
        not_bitset.resize(number_of_formulas);
        tom_bitset.resize(number_of_formulas);
        alw_bitset.resize(number_of_formulas);
        ev_bitset.resize(number_of_formulas);
        and_bitset.resize(number_of_formulas);
        or_bitset.resize(number_of_formulas);
        until_bitset.resize(number_of_formulas);
        nuntil_bitset.resize(number_of_formulas);
        res.resize(number_of_formulas);
        not_bitset.reset();
        tom_bitset.reset();
        alw_bitset.reset();
        ev_bitset.reset();
        and_bitset.reset();
        or_bitset.reset();
        until_bitset.reset();
        nuntil_bitset.reset();
        res.reset();
        lhs_set = std::vector<uint64_t>(number_of_formulas, MAX_FORMULA);
        rhs_set = std::vector<uint64_t>(number_of_formulas, MAX_FORMULA);

        for (auto& _f : formulas)
        {
                if (_f == f)
                        start = currentFormula;

                uint64_t lhs = 0, rhs = 0;
                FormulaPtr left = nullptr, right = nullptr;
                if (isa<Negation>(_f))
                {
                        if (isa<Until>(fast_cast<Negation>(_f)->formula()))
                        {
                                left = simplifier.simplify(make_negation(fast_cast<Until>(fast_cast<Negation>(_f)->formula())->left()));
                                right = simplifier.simplify(make_negation(fast_cast<Until>(fast_cast<Negation>(_f)->formula())->right()));
                        }
                        else
                                left = fast_cast<Negation>(_f)->formula();
                }
                else if (isa<Tomorrow>(_f))
                        left = fast_cast<Tomorrow>(_f)->formula();
                else if (isa<Always>(_f))
                        left = fast_cast<Always>(_f)->formula();
                else if (isa<Eventually>(_f))
                        left = fast_cast<Eventually>(_f)->formula();
                else if (isa<Conjunction>(_f))
                {
                        left = fast_cast<Conjunction>(_f)->left();
                        right = fast_cast<Conjunction>(_f)->right();
                }
                else if (isa<Disjunction>(_f))
                {
                        left = fast_cast<Disjunction>(_f)->left();
                        right = fast_cast<Disjunction>(_f)->right();
                }
                else if (isa<Until>(_f))
                {
                        left = fast_cast<Until>(_f)->left();
                        right = fast_cast<Until>(_f)->right();
                }

                if (left)
                {
                        lhs = static_cast<uint64_t>(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == left; }) - formulas.begin());
                        assert(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == left; }) != formulas.end());
                }
                if (right)
                {
                        rhs = static_cast<uint64_t>(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == right; }) - formulas.begin());

                        assert(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == right; }) != formulas.end());
                }

                add_formula_to_bitset(_f, currentFormula++, lhs, rhs);
        }

        return std::tuple<std::vector<FormulaPtr>, uint64_t>{ formulas, start };
}

static inline bool check_x_rule(const Frame& f)
{
        res = f.formulas;
        res &= not_bitset;
        res >>= 1;
        res &= f.formulas;
        return res.any();
}

static inline bool apply_and_rule(Frame& f)
{
        res = f.formulas;
        res &= and_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        // TODO: find_first and find_next don't use __builtin_clz/__builtin_ctz, find if a custom implementation using them is faster
        size_t p = res.find_first();
        while (p != Bitset::npos)
        {
                assert(and_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.formulas[lhs_set[p]] = true;
                f.formulas[rhs_set[p]] = true;
                f.toProcess[p] = false;
                p = res.find_next(p + 1);
        }

        return true;
}

static inline bool apply_always_rule(Frame& f)
{
        res = f.formulas;
        res &= alw_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        size_t p = res.find_first();
        while (p != Bitset::npos)
        {
                assert(alw_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.formulas[lhs_set[p]] = true;
                assert(tom_bitset[p + 1] && lhs_set[p + 1] == p);
                f.formulas[p + 1] = true;
                f.toProcess[p] = false;
                p = res.find_next(p + 1);
        }

        return true;
}

static inline bool apply_or_rule(Frame& f)
{
        res = f.formulas;
        res &= or_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        size_t p = res.find_first();
        if (p != Bitset::npos)
        {
                assert(or_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.toProcess[p] = false;
                f.choosenFormula = p;
                f.choice = true;
                return true;
        }

        return false;
}

static inline bool apply_ev_rule(Frame& f)
{
        res = f.formulas;
        res &= ev_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        size_t p = res.find_first();
        if (p != Bitset::npos)
        {
                assert(ev_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.toProcess[p] = false;
                f.choosenFormula = p;
                f.choice = true;
                return true;
        }

        return false;
}

static inline bool apply_until_rule(Frame& f)
{
        res = f.formulas;
        res &= until_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        size_t p = res.find_first();
        if (p != Bitset::npos)
        {
                assert(until_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.toProcess[p] = false;
                f.choosenFormula = p;
                f.choice = true;
                return true;
        }

        return false;
}

static inline bool apply_not_until_rule(Frame& f)
{
        res = f.formulas;
        res &= nuntil_bitset;
        res &= f.toProcess;

        if (!res.any())
            return false;

        size_t p = res.find_first();
        if (p != Bitset::npos)
        {
                assert(nuntil_bitset[p]);
                assert(f.formulas[p]);
                assert(f.toProcess[p]);

                f.toProcess[p] = false;
                f.choosenFormula = p;
                f.choice = true;
                return true;
        }

        return false;
}

static inline void rollback_to_choice_point(Stack& stack, uint64_t& frameID)
{
        while (!stack.empty())
        {
                if (stack.top().choice && stack.top().choosenFormula != MAX_FORMULA) // Only if i still have an available choice to do
                {
                        frameID = stack.top().id;
                        Frame newFrame(frameID, stack.top());

                        if (or_bitset[stack.top().choosenFormula])
                                newFrame.formulas[rhs_set[stack.top().choosenFormula]] = true;
                        else if (ev_bitset[stack.top().choosenFormula])
                        {
                                newFrame.formulas[stack.top().choosenFormula + 1] = true;
                                assert(tom_bitset[stack.top().choosenFormula + 1] && lhs_set[stack.top().choosenFormula + 1] == stack.top().choosenFormula);
                        }
                        else if (until_bitset[stack.top().choosenFormula])
                        {
                                newFrame.formulas[lhs_set[stack.top().choosenFormula]] = true;
                                if (tom_bitset[stack.top().choosenFormula + 1])
                                {
                                        newFrame.formulas[stack.top().choosenFormula + 1] = true;
                                        assert(lhs_set[stack.top().choosenFormula + 1] == stack.top().choosenFormula);
                                }
                                else
                                {
                                        newFrame.formulas[stack.top().choosenFormula + 2] = true;
                                        assert(lhs_set[stack.top().choosenFormula + 2] == stack.top().choosenFormula);
                                }
                        }
                        else if (nuntil_bitset[stack.top().choosenFormula])
                        {
                                newFrame.formulas[rhs_set[stack.top().choosenFormula]] = true;
                                if (tom_bitset[stack.top().choosenFormula + 1])
                                {
                                        newFrame.formulas[stack.top().choosenFormula + 1] = true;
                                        assert(lhs_set[stack.top().choosenFormula + 1] == stack.top().choosenFormula);
                                }
                                else
                                {
                                        newFrame.formulas[stack.top().choosenFormula + 2] = true;
                                        assert(lhs_set[stack.top().choosenFormula + 2] == stack.top().choosenFormula);
                                }
                        }
                        else
                        {
                                assert(false);
                        }

                        stack.top().choosenFormula = MAX_FORMULA;
                        stack.push(newFrame);

                        return;
                }

                stack.pop();
        }
}

static inline void check_eventualities(Frame& f)
{
        for (auto& e : f.eventualities)
        {
                if (f.formulas[e.first])
                        e.second = f.id;
        }
}

static inline std::pair<std::vector<FormulaSet>, uint64_t> exhibit_model(const std::vector<FormulaPtr>& fs, const Stack& stack, const Frame& loopTo)
{
        std::vector<FormulaSet> model;

        uint64_t i = 0;
        uint64_t loopIndex;
        for (const auto& frame : Container(stack))
        {
                if (frame.choice)
                        continue;

                FormulaSet set;
                for (uint64_t j = 0; j < frame.formulas.size(); ++j)
                {
                        if (frame.formulas[j] && (atom_set.find(j) != atom_set.end() || (not_bitset[j] && atom_set.find(lhs_set[j]) != atom_set.end())))
                                set.insert(fs[j]);
                }

                if (frame.id == loopTo.id)
                        loopIndex = i;

                model.push_back(set);
                ++i;
        }

        model.pop_back();

        return { model, loopIndex };
}

namespace
{
    volatile std::atomic_bool wants_info{false};
}

static void signal_handler(int signal)
{
        wants_info.store(true);
}

static inline bool eventualities_satisfied(const Frame& frame, const Frame* currFrame)
{
            for (auto it = currFrame->eventualities.begin(); it != currFrame->eventualities.end(); ++it)
            {
                    const auto& ev = frame.eventualities.find((*it).first);
                    if (ev->second == MAX_FRAME || ev->second < currFrame->id)
                            return false;
            }

            return true;
}

std::tuple<bool, std::vector<FormulaSet>, uint64_t> is_satisfiable(const FormulaPtr formula, bool model)
{
        std::signal(SIGTSTP, signal_handler);

        FormulaPtr simplified = simplifier.simplify(formula);

        if (isa<True>(simplified))
                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, { FormulaSet() }, 0);
        else if (isa<False>(simplified))
                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(false, {}, 0);

        atom_set.clear();

        uint64_t start;
        std::vector<FormulaPtr> formulas;
        std::tie(formulas, start) = initialize(simplified);

        Stack stack;

        uint64_t frameID = 0;
        stack.emplace(frameID, start);

        bool rulesApplied;

        std::mt19937 mt((std::random_device())());
        std::uniform_int_distribution<uint32_t> rand(0, 100);

        // Heuristics: MAX DEPTH
        //uint64_t max_depth = 2500;

loop:
        if (wants_info.load())
        {
            std::cout << "Stack size: " << stack.size() << std::endl;
            std::cout << "Number of formulas: " << stack.top().formulas.count() << std::endl;
            wants_info.store(false);
        }

        while (!stack.empty())
        {
                Frame& frame = stack.top();

                rulesApplied = true;
                while (rulesApplied)
                {
                        rulesApplied = false;

                        if (__builtin_expect(frame.formulas.none(), 0))
                        {
                                if (model)
                                {
                                        auto ex_model = exhibit_model(formulas, stack, *frame.chain);
                                        return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, ex_model.first, ex_model.second);
                                }
                                else
                                        return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, {}, 0);
                        }

                        if (check_x_rule(frame))
                        {
                                rollback_to_choice_point(stack, frameID);
                                goto loop;
                        }

                        if (apply_and_rule(frame))
                                rulesApplied = true;
                        if (apply_always_rule(frame))
                                rulesApplied = true;

                        if (apply_or_rule(frame))
                        {
                                Frame newFrame(frameID, frame);
                                newFrame.formulas[lhs_set[frame.choosenFormula]] = true;
                                stack.push(newFrame);

                                goto loop;
                        }

                        if (apply_ev_rule(frame))
                        {
                                if (__builtin_expect(frame.eventualities.find(lhs_set[frame.choosenFormula]) == frame.eventualities.end(), 0))
                                        frame.eventualities[lhs_set[frame.choosenFormula]] = MAX_FRAME;

                                Frame newFrame(frameID, frame);
                                newFrame.formulas[lhs_set[frame.choosenFormula]] = true;
                                stack.push(newFrame);

                                goto loop;
                        }

                        if (apply_until_rule(frame))
                        {
                                if (__builtin_expect(frame.eventualities.find(rhs_set[frame.choosenFormula]) == frame.eventualities.end(), 0))
                                        frame.eventualities[rhs_set[frame.choosenFormula]] = MAX_FRAME;

                                Frame newFrame(frameID, frame);
                                newFrame.formulas[rhs_set[frame.choosenFormula]] = true;
                                stack.push(newFrame);

                                goto loop;
                        }

                        if (apply_not_until_rule(frame))
                        {
                                Frame newFrame(frameID, frame);
                                newFrame.formulas[lhs_set[frame.choosenFormula]] = true;
                                newFrame.formulas[rhs_set[frame.choosenFormula]] = true;
                                stack.push(newFrame);

                                goto loop;
                        }
                }

                check_eventualities(frame);

                // LOOP rule
                const Frame* repFrame1 = nullptr, *repFrame2 = nullptr;
                const Frame* currFrame = frame.chain;

                // Heuristics: PARTIAL LOOKBACK
                //uint64_t minFrame = 0;

                // Heuristics: OCCASIONAL LOOKBACK
                //if (rand(mt) > 5)
                 //       goto step_rule;

                // Heuristics: PARTIAL LOOKBACK
                //else
                        //minFrame = static_cast<uint64_t>(static_cast<float>(rand(mt)) / 100.0 * currFrame->id);

                while (currFrame)
                {
                        // Heuristics: PARTIAL LOOKBACK
                        //if (currFrame->id < minFrame)
                                //break;

                        if (frame.formulas.is_subset_of(currFrame->formulas))
                        {
                                // TODO: Can this be done in a cheaper way? Probably yes
                                // if (eventualities_satisfied(frame, currFrame))
                                if (std::all_of(currFrame->eventualities.begin(), currFrame->eventualities.end(), [&] (const std::pair<uint64_t, uint64_t>& p)
                                {
                                        const auto& ev = frame.eventualities.find(p.first);
                                        return ev->second != MAX_FRAME && ev->second >= currFrame->id;
                                }))
                                {
                                        if (model)
                                        {
                                                auto ex_model = exhibit_model(formulas, stack, *currFrame);
                                                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, ex_model.first, ex_model.second);
                                        }
                                        else
                                                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, {}, 0);
                                }
                                else  if (frame.formulas == currFrame->formulas) // STEP rule check
                                {
                                        if (!repFrame1)
                                                repFrame1 = currFrame;
                                        else if (!repFrame2)
                                                repFrame2 = currFrame;
                                }
                                //if (!frame.formulas.is_proper_subset_of(currFrame->formulas)) // Alternative: seems to be completly the same in terms of performance
                        }
                        currFrame = currFrame->chain;
                }

                // REP rule application
                if (repFrame1 && repFrame2)
                {
                        rollback_to_choice_point(stack, frameID);
                        goto loop;
                }

// Heuristics: OCCASIONAL LOOKBACK
//step_rule:

                // Heuristics: MAX DEPTH
                //if (frameID > max_depth)
                //{
                //       rollback_to_choice_point(stack, frameID);
                //        goto loop;
                //}

                // STEP rule
                Frame newFrame(++frameID, frame.eventualities, &frame);
                res = frame.formulas;
                res &= tom_bitset;

                for (uint64_t i = 0; i < number_of_formulas; ++i)
                {
                        if (res[i])
                        {
                                assert(frame.formulas[i]);
                                assert(tom_bitset[i]);
                                newFrame.formulas[lhs_set[i]] = true;
                        }
                }

                stack.push(newFrame);
        }

    return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(false, {}, 0);
}
*/

}
}
