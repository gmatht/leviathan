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

#include <cassert>
#include "clause_counter.hpp"

namespace LTL {
namespace detail {

uint64_t ClauseCounter::count(const FormulaPtr f)
{
  assert(isa<Disjunction>(f) &&
         "The root of the formula is not a disjunction!");

  _count = 1;
  f->accept(*this);
  return _count;
}

void ClauseCounter::visit(const True *)
{
  assert(false && "True node found in the AST!");
}

void ClauseCounter::visit(const False *)
{
  assert(false && "False node found in the AST!");
}

void ClauseCounter::visit(const Atom *)
{
}

void ClauseCounter::visit(const Negation *t)
{
  assert((isa<Atom>(t->formula()) || isa<Until>(t->formula())) &&
         "Formula may have not been simplified!");
  (void)(t); // To silence unused variable warning
}

void ClauseCounter::visit(const Tomorrow *)
{
}

void ClauseCounter::visit(const Always *)
{
}

void ClauseCounter::visit(const Eventually *)
{
}

void ClauseCounter::visit(const Conjunction *)
{
}

void ClauseCounter::visit(const Disjunction *t)
{
  t->left()->accept(*this);
  t->right()->accept(*this);
  _count += 1;
}

void ClauseCounter::visit(const Then *)
{
  assert(false && "Then node found in the AST!");
}

void ClauseCounter::visit(const Iff *)
{
  assert(false && "Iff node found in the AST!");
}

void ClauseCounter::visit(const Until *)
{
}
}
}
