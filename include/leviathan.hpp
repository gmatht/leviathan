#pragma once

#include "../src/formula.hpp"
#include "../src/parser/parser.hpp"
#include "../src/solver.hpp"
#include "../src/visitor.hpp"

namespace LTL
{

using detail::parse;
using detail::is_satisfiable;

using detail::FormulaPtr;
using detail::PrettyPrinter;
using detail::FormulaSet;

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

}
