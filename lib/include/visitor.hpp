#pragma once

#include "formula.hpp"

namespace LTL {

namespace detail {

class Visitor {
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
  virtual void visit(const True *t) = 0;
  virtual void visit(const False *f) = 0;
  virtual void visit(const Atom *atom) = 0;
  virtual void visit(const Negation *negation) = 0;
  virtual void visit(const Tomorrow *tomorrow) = 0;
  virtual void visit(const Always *always) = 0;
  virtual void visit(const Eventually *eventually) = 0;
  virtual void visit(const Conjunction *conjunction) = 0;
  virtual void visit(const Disjunction *disjunction) = 0;
  virtual void visit(const Then *then) = 0;
  virtual void visit(const Iff *iff) = 0;
  virtual void visit(const Until *until) = 0;

public:
  virtual ~Visitor() {}
};
}
}
