#include "clause_counter.hpp"
#include <cassert>

namespace LTL
{
namespace detail
{

uint64_t ClauseCounter::count(const FormulaPtr f)
{
        assert(isa<Disjunction>(f) && "The root of the formula is not a disjunction!");

        _count = 1;
        f->accept(*this);
        return _count;
}

void ClauseCounter::visit(const True*)
{
        assert(false && "True node found in the AST!");
}

void ClauseCounter::visit(const False*)
{
        assert(false && "False node found in the AST!");
}

void ClauseCounter::visit(const Atom*) { }

void ClauseCounter::visit(const Negation* t)
{
        assert((isa<Atom>(t->formula()) || isa<Until>(t->formula())) && "Formula may have not been simplified!");
}

void ClauseCounter::visit(const Tomorrow* t) { }

void ClauseCounter::visit(const Always* t) { }

void ClauseCounter::visit(const Eventually* t) { }

void ClauseCounter::visit(const Conjunction* t) { }

void ClauseCounter::visit(const Disjunction* t)
{
        t->left()->accept(*this);
        t->right()->accept(*this);
        _count += 1;
}

void ClauseCounter::visit(const Then*)
{
        assert(false && "Then node found in the AST!");
}

void ClauseCounter::visit(const Iff*)
{
        assert(false && "Iff node found in the AST!");
}

void ClauseCounter::visit(const Until* t) { }

}
}