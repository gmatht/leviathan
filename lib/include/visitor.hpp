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
