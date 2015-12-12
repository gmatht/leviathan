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
#include "parser.hpp"
#include "solver.hpp"
#include "identifiable.hpp"
#include "visitor.hpp"
#include "pretty_printer.hpp"
#include "simplifier.hpp"
#include "format.hpp"

namespace LTL {

using detail::parse;

using detail::Solver;
using detail::Model;
using detail::ModelPtr;
using detail::State;
using detail::Literal;
using detail::FrameID;

using detail::FormulaPtr;
using detail::PrettyPrinter;

using detail::isa;
using detail::fast_cast;

using detail::True;
using detail::False;
using detail::Atom;
using detail::Negation;
using detail::Always;
using detail::Eventually;
using detail::Tomorrow;
using detail::Conjunction;
using detail::Disjunction;
using detail::Then;
using detail::Iff;
using detail::Until;

using detail::TruePtr;
using detail::FalsePtr;
using detail::AtomPtr;
using detail::NegationPtr;
using detail::AlwaysPtr;
using detail::EventuallyPtr;
using detail::TomorrowPtr;
using detail::ConjunctionPtr;
using detail::DisjunctionPtr;
using detail::ThenPtr;
using detail::IffPtr;
using detail::UntilPtr;

using detail::make_true;
using detail::make_false;
using detail::make_atom;
using detail::make_negation;
using detail::make_always;
using detail::make_eventually;
using detail::make_tomorrow;
using detail::make_conjunction;
using detail::make_disjunction;
using detail::make_then;
using detail::make_iff;
using detail::make_until;

namespace format = detail::format;
}
