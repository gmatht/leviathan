#pragma once

#include "visitor.hpp"

namespace LTL {
namespace detail {

class Simplifier : public Visitor {
public:
  Simplifier() : result(nullptr), rulesApplied(true) {}
  virtual ~Simplifier() {}
  FormulaPtr simplify(FormulaPtr formula);

protected:
  virtual void visit(const True *t) override;
  virtual void visit(const False *f) override;
  virtual void visit(const Atom *atom) override;
  virtual void visit(const Negation *negation) override;
  virtual void visit(const Tomorrow *tomorrow) override;
  virtual void visit(const Always *always) override;
  virtual void visit(const Eventually *eventually) override;
  virtual void visit(const Conjunction *conjunction) override;
  virtual void visit(const Disjunction *disjunction) override;
  virtual void visit(const Then *then) override;
  virtual void visit(const Iff *iff) override;
  virtual void visit(const Until *until) override;

private:
  FormulaPtr result;
  bool rulesApplied;
};
}
}