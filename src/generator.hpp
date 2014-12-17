#pragma once

#include "formula.hpp"
#include "visitor.hpp"
#include <vector>

namespace LTL
{

namespace detail
{

class Generator : public Visitor
{
public:
        Generator()
                : _formulas()
                , simplifier()
        {
        }
        ~Generator()
        {
        }

        void generate(const FormulaPtr f);
        
        const std::vector<FormulaPtr>& formulas() const
        {
                return _formulas;
        }

protected:
        virtual void visit(const True* t) override;
        virtual void visit(const False* f) override;
        virtual void visit(const Atom* atom) override;
        virtual void visit(const Negation* negation) override;
        virtual void visit(const Tomorrow* tomorrow) override;
        virtual void visit(const Always* always) override;
        virtual void visit(const Eventually* eventually) override;
        virtual void visit(const Conjunction* conjunction) override;
        virtual void visit(const Disjunction* disjunction) override;
        virtual void visit(const Then* then) override;
        virtual void visit(const Iff* iff) override;
        virtual void visit(const Until* until) override;

private:
        std::vector<FormulaPtr> _formulas;
        Simplifier simplifier;
};
}
}
