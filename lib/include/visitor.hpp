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
  friend class Yesterday;
  friend class Always;
  friend class Eventually;
  friend class Conjunction;
  friend class Disjunction;
  friend class Then;
  friend class Iff;
  friend class Until;
  friend class Release;
  friend class Since;
  friend class Triggered;
  friend class Past;
  friend class Historically;

protected:
  virtual void visit(const True         *) = 0;
  virtual void visit(const False        *) = 0;
  virtual void visit(const Atom         *) = 0;
  virtual void visit(const Negation     *) = 0;
  virtual void visit(const Tomorrow     *) = 0;
  virtual void visit(const Yesterday    *) = 0;
  virtual void visit(const Always       *) = 0;
  virtual void visit(const Eventually   *) = 0;
  virtual void visit(const Conjunction  *) = 0;
  virtual void visit(const Disjunction  *) = 0;
  virtual void visit(const Then         *) = 0;
  virtual void visit(const Iff          *) = 0;
  virtual void visit(const Until        *) = 0;
  virtual void visit(const Release      *) = 0;
  virtual void visit(const Since        *) = 0;
  virtual void visit(const Triggered    *) = 0;
  virtual void visit(const Past         *) = 0;
  virtual void visit(const Historically *) = 0;

public:
  virtual ~Visitor();
};
}
}
