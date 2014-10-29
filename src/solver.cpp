#include "solver.hpp"

#include "generator.hpp"
#include <stack>
#include <iostream>
#include <cassert>
#include <unordered_map>

namespace LTL
{

namespace detail
{

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
                : formulas()
                , toProcess()
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
        Frame(const uint64_t _id, const std::unordered_map<uint64_t, uint64_t> _eventualities, const Frame* chainPtr)
                : formulas()
                , toProcess()
                , eventualities(_eventualities)
                , id(_id)
                , choosenFormula(MAX_FORMULA)
                , choice(false)
                , chain(chainPtr)
        {
                toProcess.set();
        }
};

// static PrettyPrinter printer; // Why this one?
static Simplifier simplifier;

static std::unordered_map<uint64_t, std::string> atom_set;
static Bitset not_bitset;
static Bitset tom_bitset;
static Bitset alw_bitset;
static Bitset ev_bitset;
static Bitset and_bitset;
static Bitset or_bitset;
static Bitset until_bitset;
static Bitset nuntil_bitset;
static std::vector<uint64_t> lhs_set(MAX_FORMULA_SIZE * 4, MAX_FORMULA);
static std::vector<uint64_t> rhs_set(MAX_FORMULA_SIZE * 4, MAX_FORMULA);

static uint64_t cycles_bound = 0;

static void add_formula_to_bitset(const FormulaPtr f, uint64_t pos, uint64_t lhs, uint64_t rhs)
{
        assert(pos < MAX_FORMULA_SIZE);

        switch (f->type())
        {
                case Formula::True:
                        assert(false);

                case Formula::False:
                        assert(false);

                case Formula::Atom:
                        atom_set[pos] = fast_cast<Atom>(f)->name();
                        break;

                case Formula::Negation:
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

                case Formula::Tomorrow:
                        tom_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Always:
                        alw_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Eventually:
                        ev_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        break;

                case Formula::Conjunction:
                        and_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                case Formula::Disjunction:
                        or_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                case Formula::Then:
                        assert(false);

                case Formula::Iff:
                        assert(false);

                case Formula::Until:
                        until_bitset[pos] = true;
                        lhs_set[pos] = lhs;
                        rhs_set[pos] = rhs;
                        break;

                default:
                        assert(false);
        }
}

static std::pair<std::vector<FormulaPtr>, uint64_t> initialize(const FormulaPtr f)
{
        if (isa<True>(f))
                return { { make_true() }, 0 };
        if (isa<False>(f))
                return { { make_false() }, 0 };

        Generator gen;
        gen.generate(f);
        std::vector<FormulaPtr> formulas = gen.formulas();

        // TODO: Check if a Not Until special case is needed
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
                        lhs = std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == left; }) - formulas.begin();
                        assert(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == left; }) != formulas.end());
                }
                if (right)
                {
                        rhs = std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == right; }) - formulas.begin();
                        assert(std::find_if(formulas.begin(), formulas.end(), [&](FormulaPtr a)
                        { return a == right; }) != formulas.end());
                }

                add_formula_to_bitset(_f, currentFormula++, lhs, rhs);
        }

        cycles_bound = currentFormula;
        return { formulas, start };
}

static inline bool check_x_rule(const Frame& f)
{
        return (((f.formulas & not_bitset) >> 1) & f.formulas).any();
}

static inline bool apply_and_rule(Frame& f)
{
        uint64_t count = f.formulas.count();

        Bitset res = f.formulas & and_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.formulas[lhs_set[i]] = true;
                        f.formulas[rhs_set[i]] = true;
                        f.toProcess[i] = false;
                }
        }

        if (count != f.formulas.count())
                return true;

        return false;
}

static inline bool apply_always_rule(Frame& f)
{
        uint64_t count = f.formulas.count();

        Bitset res = f.formulas & alw_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.formulas[lhs_set[i]] = true;
                        assert(tom_bitset[i + 1] && lhs_set[i + 1] == i);
                        f.formulas[i + 1] = true;
                        f.toProcess[i] = false;
                }
        }

        if (count != f.formulas.count())
                return true;

        return false;
}

static inline bool apply_or_rule(Frame& f)
{
        Bitset res = f.formulas & or_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.toProcess[i] = false;
                        f.choosenFormula = i;
                        f.choice = true;
                        return true;
                }
        }

        return false;
}

static inline bool apply_ev_rule(Frame& f)
{
        Bitset res = f.formulas & ev_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.toProcess[i] = false;
                        f.choosenFormula = i;
                        f.choice = true;
                        return true;
                }
        }

        return false;
}

static inline bool apply_until_rule(Frame& f)
{
        Bitset res = f.formulas & until_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.toProcess[i] = false;
                        f.choosenFormula = i;
                        f.choice = true;
                        return true;
                }
        }

        return false;
}

static inline bool apply_not_until_rule(Frame& f)
{
        Bitset res = f.formulas & nuntil_bitset & f.toProcess;
        for (uint64_t i = 0; i < cycles_bound; ++i)
        {
                if (res[i])
                {
                        f.toProcess[i] = false;
                        f.choosenFormula = i;
                        f.choice = true;
                        return true;
                }
        }

        return false;
}

static inline void rollback_to_choice_point(std::stack<Frame>& stack, uint64_t& frameID)
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
                                        newFrame.formulas[stack.top().choosenFormula + 1];
                                        assert(lhs_set[stack.top().choosenFormula + 1] == stack.top().choosenFormula);
                                }
                                else
                                {
                                        newFrame.formulas[stack.top().choosenFormula + 2];
                                        assert(lhs_set[stack.top().choosenFormula + 2] == stack.top().choosenFormula);
                                }
                        }
                        else
                        {
                                //std::cout << "Something went wrong. We shouldn't be there...................." << std::endl;
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

static inline std::pair<std::vector<FormulaSet>, uint64_t> exhibit_model(const std::vector<FormulaPtr>& fs,
                                                                         const std::stack<Frame>& stack, const Frame& loopTo)
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

std::tuple<bool, std::vector<FormulaSet>, uint64_t> is_satisfiable(const FormulaPtr formula, bool model)
{
        FormulaPtr simplified = simplifier.simplify(formula);

        if (isa<True>(simplified))
                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(true, { FormulaSet() }, 0);
        else if (isa<False>(simplified))
                return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(false, {}, 0);

        not_bitset.reset();
        tom_bitset.reset();
        alw_bitset.reset();
        ev_bitset.reset();
        and_bitset.reset();
        or_bitset.reset();
        until_bitset.reset();
        nuntil_bitset.reset();
        lhs_set = std::vector<uint64_t>(MAX_FORMULA_SIZE * 4, MAX_FORMULA);
        rhs_set = std::vector<uint64_t>(MAX_FORMULA_SIZE * 4, MAX_FORMULA);
        atom_set.clear();

        uint64_t start;
        std::vector<FormulaPtr> formulas;
        std::tie(formulas, start) = initialize(simplified);

        std::stack<Frame> stack;

        uint64_t frameID = 0;
        stack.emplace(frameID, start);

        bool rulesApplied;

loop:
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
                const Frame* currFrame = frame.chain;
                while (currFrame)
                {

                        if ((frame.formulas | currFrame->formulas) == currFrame->formulas)
                        {
                                // TODO: Can this be done in a cheaper way? Probably yes
                                if (std::all_of(currFrame->eventualities.begin(), currFrame->eventualities.end(), [&](std::pair<uint64_t, uint64_t> p)
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
                        }
                        currFrame = currFrame->chain;
                }

                // REP rule
                const Frame* framePtr = nullptr;
                currFrame = frame.chain;
                while (currFrame)
                {
                        if (currFrame->formulas == frame.formulas)
                        {
                                if (!framePtr)
                                        framePtr = currFrame;
                                else
                                {
                                        rollback_to_choice_point(stack, frameID);
                                        goto loop;
                                }
                        }

                        currFrame = currFrame->chain;
                }

                // STEP rule
                Frame newFrame(++frameID, frame.eventualities, &frame);
                auto res = frame.formulas & tom_bitset;
                for (uint64_t i = 0; i < cycles_bound; ++i)
                {
                        if (res[i])
                                newFrame.formulas[lhs_set[i]] = true;
                }

                stack.push(newFrame);
        }

        return std::tuple<bool, std::vector<FormulaSet>, uint64_t>(false, {}, 0);
}
}
}
