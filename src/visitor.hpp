#pragma once

#include "formula.hpp"

#include <ostream>

namespace LTL
{

namespace detail
{

class Visitor
{
        friend class True;
        friend class False;
        friend class Atom;
        friend class Negation;
        friend class Tomorrow;
        friend class Always;
        friend class Eventually;
        friend class Conjunction;
        friend class Disjunction;
        friend class Then;
        friend class Iff;
        friend class Until;

protected:
        virtual void visit(const True* t) = 0;
        virtual void visit(const False* f) = 0;
        virtual void visit(const Atom* atom) = 0;
        virtual void visit(const Negation* negation) = 0;
        virtual void visit(const Tomorrow* tomorrow) = 0;
        virtual void visit(const Always* always) = 0;
        virtual void visit(const Eventually* eventually) = 0;
        virtual void visit(const Conjunction* conjunction) = 0;
        virtual void visit(const Disjunction* disjunction) = 0;
        virtual void visit(const Then* then) = 0;
        virtual void visit(const Iff* iff) = 0;
        virtual void visit(const Until* until) = 0;

public:
        virtual ~Visitor()
        {
        }
};

class PrettyPrinter : public Visitor
{
public:
    PrettyPrinter(std::ostream &out) : _out(out) { }
    
    virtual ~PrettyPrinter() = default;
    
    std::ostream& print(const FormulaPtr formula, bool newLine = false);
    std::ostream& print(const Formula   *formula, bool newLine = false);

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
        std::ostream &_out;
};

class Simplifier : public Visitor
{
public:
        Simplifier()
                : result(nullptr)
                , rulesApplied(true)
        {
        }
        virtual ~Simplifier()
        {
        }

        FormulaPtr simplify(FormulaPtr formula);

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
        FormulaPtr result;
        bool rulesApplied;
};
}
}
