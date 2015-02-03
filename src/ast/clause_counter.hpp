#pragma once

#include "visitor.hpp"

namespace LTL
{
namespace detail
{

class ClauseCounter : public Visitor
{
public:
    ClauseCounter() : _count(1) { }
    virtual ~ClauseCounter() = default;
    
    uint64_t count(const FormulaPtr formula);

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
    uint64_t _count;
};

}
}
