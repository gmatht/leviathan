#include "generator.hpp"
#include <cassert>

namespace LTL
{

namespace detail
{

void Generator::generate(const FormulaPtr f)
{
        FormulaPtr simplified = _simplifier.simplify(f);

        _formulas.push_back(simplified);

        if (!isa<True>(simplified) && !isa<False>(simplified))
                simplified->accept(*this);
}

void Generator::visit(const True*)
{
        assert(false && "True node found in the AST!");
}

void Generator::visit(const False*)
{
        assert(false && "False node found in the AST!");
}

void Generator::visit(const Atom*) { }

void Generator::visit(const Negation* t)
{
        if (isa<Until>(t->formula()))
        {
                _formulas.push_back(make_tomorrow(make_negation(t->formula())));

                auto lneg = _simplifier.simplify(make_negation(fast_cast<Until>(t->formula())->left()));
                auto rneg = _simplifier.simplify(make_negation(fast_cast<Until>(t->formula())->right()));

                _formulas.push_back(lneg);
                _formulas.push_back(rneg);

                lneg->accept(*this);
                rneg->accept(*this);
        }
        else
        {
                _formulas.push_back(t->formula());
                t->formula()->accept(*this);
        }
}

void Generator::visit(const Tomorrow* t)
{
        _formulas.push_back(t->formula());
        t->formula()->accept(*this);
}

void Generator::visit(const Always* t)
{
        _formulas.push_back(t->formula());
        _formulas.push_back(make_tomorrow(make_always(t->formula())));
        t->formula()->accept(*this);
}

void Generator::visit(const Eventually* t)
{
        _formulas.push_back(t->formula());
        _formulas.push_back(make_tomorrow(make_eventually(t->formula())));
        t->formula()->accept(*this);
}

void Generator::visit(const Conjunction* t)
{
        _formulas.push_back(t->left());
        _formulas.push_back(t->right());
        t->left()->accept(*this);
        t->right()->accept(*this);
}

void Generator::visit(const Disjunction* t)
{
        _formulas.push_back(t->left());
        _formulas.push_back(t->right());
        t->left()->accept(*this);
        t->right()->accept(*this);
}

void Generator::visit(const Then*)
{
        assert(false && "Then node found in the AST!");
}

void Generator::visit(const Iff* t)
{
        _formulas.push_back(t->left());
        _formulas.push_back(t->right());
        
        auto lneg = _simplifier.simplify(make_negation(t->left()));
        auto rneg = _simplifier.simplify(make_negation(t->right()));

        _formulas.push_back(lneg);
        _formulas.push_back(rneg);

        lneg->accept(*this);
        rneg->accept(*this);

        //assert(false && "Iff node found in the AST!");
}

void Generator::visit(const Until* t)
{
        _formulas.push_back(t->left());
        _formulas.push_back(t->right());
        _formulas.push_back(make_tomorrow(make_until(t->left(), t->right())));
        t->left()->accept(*this);
        t->right()->accept(*this);
}

}
}
