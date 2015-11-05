#pragma once

#include <iostream>
#include "visitor.hpp"

namespace LTL {
namespace detail {

class Visitor;

class PrettyPrinter : public Visitor {
public:
  PrettyPrinter(std::ostream &out = std::cout) : _out(out) {}
  virtual ~PrettyPrinter() = default;

  std::ostream &print(const FormulaPtr formula, bool newLine = false);
  std::ostream &print(const Formula *formula, bool newLine = false);

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
  std::ostream &_out;
};
}
}
