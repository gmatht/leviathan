#include "pretty_printer.hpp"

namespace LTL {
namespace detail {

#define UNARY_VISIT(_Type, _Symbol)         \
  void PrettyPrinter::visit(const _Type *u) \
  {                                         \
    _out << _Symbol << "( ";                \
    u->formula()->accept(*this);            \
    _out << " )";                           \
  }

#define BINARY_VISIT(_Type, _Symbol)        \
  void PrettyPrinter::visit(const _Type *b) \
  {                                         \
    _out << "(";                            \
    b->left()->accept(*this);               \
    _out << ") " << _Symbol << " (";        \
    b->right()->accept(*this);              \
    _out << ")";                            \
  }

std::ostream &PrettyPrinter::print(const FormulaPtr formula, bool newLine)
{
  return print(formula.get(), newLine);
}

std::ostream &PrettyPrinter::print(const Formula *formula, bool newLine)
{
  formula->accept(*this);
  return newLine ? _out << std::endl : _out;
}

void PrettyPrinter::visit(const True *)
{
  _out << "\u22a4";
}

void PrettyPrinter::visit(const False *)
{
  _out << "\u22a5";
}

void PrettyPrinter::visit(const Atom *atom)
{
  _out << atom->name();
}

UNARY_VISIT(Negation, "\u00AC")
UNARY_VISIT(Tomorrow, "\u25CB")
UNARY_VISIT(Always, "\u25A1")
UNARY_VISIT(Eventually, "\u25C7")

BINARY_VISIT(Conjunction, "\u2227")
BINARY_VISIT(Disjunction, "\u2228")
BINARY_VISIT(Then, "\u2192")
BINARY_VISIT(Iff, "\u2194")
BINARY_VISIT(Until, "\u222a")

#undef UNARY_VISIT
#undef BINARY_VISIT
}
}
