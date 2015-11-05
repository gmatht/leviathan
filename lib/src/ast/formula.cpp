#include "formula.hpp"
#include "visitor.hpp"
#include <iostream>

namespace LTL {

namespace detail {

#define MAKE_UNARY(_Type, _type)               \
  _Type##Ptr make_##_type(const FormulaPtr &f) \
  {                                            \
    return std::make_shared<_Type>(f);         \
  }

#define MAKE_BINARY(_Type, _type)                                     \
  _Type##Ptr make_##_type(const FormulaPtr &f1, const FormulaPtr &f2) \
  {                                                                   \
    return std::make_shared<_Type>(f1, f2);                           \
  }

#define ACCEPT_VISITOR(_Type) \
  void _Type::accept(Visitor &v) const { v.visit(this); }
TruePtr make_true()
{
  return std::make_shared<True>();
}

FalsePtr make_false()
{
  return std::make_shared<False>();
}

AtomPtr make_atom(const std::string &name)
{
  return std::make_shared<Atom>(name);
}

MAKE_UNARY(Negation, negation)
MAKE_UNARY(Tomorrow, tomorrow)
MAKE_UNARY(Always, always)
MAKE_UNARY(Eventually, eventually)

MAKE_BINARY(Conjunction, conjunction)
MAKE_BINARY(Disjunction, disjunction)
MAKE_BINARY(Then, then)
MAKE_BINARY(Iff, iff)
MAKE_BINARY(Until, until)

ACCEPT_VISITOR(True)
ACCEPT_VISITOR(False)
ACCEPT_VISITOR(Atom)
ACCEPT_VISITOR(Negation)
ACCEPT_VISITOR(Tomorrow)
ACCEPT_VISITOR(Always)
ACCEPT_VISITOR(Eventually)
ACCEPT_VISITOR(Conjunction)
ACCEPT_VISITOR(Disjunction)
ACCEPT_VISITOR(Then)
ACCEPT_VISITOR(Iff)
ACCEPT_VISITOR(Until)

#undef MAKE_UNARY
#undef MAKE_BINARY
#undef ACCEPT_VISITOR

bool operator==(const FormulaPtr f1, const FormulaPtr f2)
{
  if (f1->type() != f2->type())
    return false;

  if (isa<Atom>(f1))
    return fast_cast<Atom>(f1)->name() == fast_cast<Atom>(f2)->name();

  if (isa<Negation>(f1))
    return fast_cast<Negation>(f1)->formula() ==
           fast_cast<Negation>(f2)->formula();

  if (isa<Tomorrow>(f1))
    return fast_cast<Tomorrow>(f1)->formula() ==
           fast_cast<Tomorrow>(f2)->formula();

  if (isa<Always>(f1))
    return fast_cast<Always>(f1)->formula() ==
           fast_cast<Always>(f2)->formula();

  if (isa<Eventually>(f1))
    return fast_cast<Eventually>(f1)->formula() ==
           fast_cast<Eventually>(f2)->formula();

  if (isa<Conjunction>(f1))
    return fast_cast<Conjunction>(f1)->left() ==
             fast_cast<Conjunction>(f2)->left() &&
           fast_cast<Conjunction>(f1)->right() ==
             fast_cast<Conjunction>(f2)->right();

  if (isa<Disjunction>(f1))
    return fast_cast<Disjunction>(f1)->left() ==
             fast_cast<Disjunction>(f2)->left() &&
           fast_cast<Disjunction>(f1)->right() ==
             fast_cast<Disjunction>(f2)->right();

  if (isa<Iff>(f1))
    return fast_cast<Iff>(f1)->left() == fast_cast<Iff>(f2)->left() &&
           fast_cast<Iff>(f1)->right() == fast_cast<Iff>(f2)->right();

  if (isa<Then>(f1))
    return fast_cast<Then>(f1)->left() == fast_cast<Then>(f2)->left() &&
           fast_cast<Then>(f1)->right() == fast_cast<Then>(f2)->right();

  if (isa<Until>(f1))
    return fast_cast<Until>(f1)->left() == fast_cast<Until>(f2)->left() &&
           fast_cast<Until>(f1)->right() == fast_cast<Until>(f2)->right();

  assert(isa<True>(f1) || isa<False>(f1));

  return true;
}

bool operator!=(const FormulaPtr f1, const FormulaPtr f2)
{
  return !(f1 == f2);
}
}
}
