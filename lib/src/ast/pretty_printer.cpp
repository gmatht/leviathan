/*
  Copyright (c) 2014, Matteo Bertello
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * The names of its contributors may not be used to endorse or promote
    products derived from this software without specific prior written
    permission.
*/

#include "pretty_printer.hpp"

namespace LTL {
namespace detail {

#define UNARY_VISIT(_Type, _Symbol)         \
  void PrettyPrinter::visit(const _Type *u) \
  {                                         \
    _stream << _Symbol << "(";              \
    u->formula()->accept(*this);            \
    _stream << ")";                         \
  }

#define BINARY_VISIT(_Type, _Symbol)        \
  void PrettyPrinter::visit(const _Type *b) \
  {                                         \
    _stream << "(";                         \
    b->left()->accept(*this);               \
    _stream << ") " << _Symbol << " (";     \
    b->right()->accept(*this);              \
    _stream << ")";                         \
  }

std::string PrettyPrinter::to_string(const FormulaPtr formula)
{
  return to_string(formula.get());
}

std::string PrettyPrinter::to_string(const Formula *formula)
{
  _stream.str(std::string{});
  formula->accept(*this);
  return _stream.str();
}

void PrettyPrinter::visit(const True *)
{
  _stream << "\u22a4";
}

void PrettyPrinter::visit(const False *)
{
  _stream << "\u22a5";
}

void PrettyPrinter::visit(const Atom *atom)
{
  _stream << atom->name();
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
