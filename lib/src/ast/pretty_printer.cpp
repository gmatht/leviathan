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

#define UNARY_VISIT(_Type, _Symbol)          \
  void PrettyPrinter::visit(const _Type *u)  \
  {                                          \
    format::log(_logLevel, "{}( ", _Symbol); \
    u->formula()->accept(*this);             \
    format::log(_logLevel, ") ");            \
  }

#define BINARY_VISIT(_Type, _Symbol)           \
  void PrettyPrinter::visit(const _Type *b)    \
  {                                            \
    format::log(_logLevel, "(");               \
    b->left()->accept(*this);                  \
    format::log(_logLevel, ") {} (", _Symbol); \
    b->right()->accept(*this);                 \
    format::log(_logLevel, ")");               \
  }

void PrettyPrinter::print(const FormulaPtr formula, bool newLine)
{
  print(formula.get(), newLine);
}

void PrettyPrinter::print(const Formula *formula, bool newLine)
{
  formula->accept(*this);
  if (newLine)
    format::log(_logLevel, "\n");
}

void PrettyPrinter::visit(const True *)
{
  format::log(_logLevel, "\u22a4");
}

void PrettyPrinter::visit(const False *)
{
  format::log(_logLevel, "\u22a5");
}

void PrettyPrinter::visit(const Atom *atom)
{
  format::log(_logLevel, "{}", atom->name());
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
