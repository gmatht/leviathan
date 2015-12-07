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

#include "leviathan.hpp"

#include <limits>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>

#include "optional.hpp"

#include "tclap/CmdLine.h"

/*
 Rework the driver program.
 - Command-line interface and console issues:
 * Colored output (factor out into the format namespace)
 * Choose the level of verbosity on the command line
 * --timings option to enable time measurements output
 - Actions:
 * Parse the formula
 * Parse the test data associated to the formula, if the --test option
   was given
 * Initialize the solver and solve
 * Give the output (parsable output should have same format as test files)
 - Time measurement:
 * On parsing
 * On solving
 * Parsable output
*/

namespace format = LTL::format;
using namespace format::colors;
using std::experimental::optional;
using std::experimental::nullopt;

const std::string leviathan_version = "0.2.2";

/*
 * Command line arguments. It is not so bad to make them global objects
 * since no one will ever write to them excepting the CmdLine::parse()
 * function.
 * Being able to access to command line arguments from everywhere aids
 * modularity in the rest of the file.
 *
 * If you add a new parameter, remember to register it in main()
 */
namespace Args {

TCLAP::UnlabeledValueArg<std::string> filename(
  "filename", "The name of the file to load the formulas from", false, "-",
  "path");

TCLAP::ValueArg<std::string> ltl(
  "l", "ltl",
  "The LTL formula to solve, provided directly on the command line", false, "",
  "LTL formula");

TCLAP::SwitchArg model(
  "m", "model",
  "Generates and prints a model of the formula, when satisfiable", false);

TCLAP::SwitchArg parsable(
  "p", "parsable", "Generates machine-parsable output when printing the model",
  false);

TCLAP::SwitchArg test(
  "t", "test",
  "Test the checker answer. In this mode, the '-f' flag is mandatory. The "
  "given filename will be read for the formula, together with a .answer "
  "file "
  "named after it, containing the correct answer. The .answer file must "
  "contain the syntactic representation of the correct model of the given "
  "formula, if the formula is satisfiable, or be empty otherwise.",
  false);

TCLAP::ValueArg<uint8_t> verbosity(
  "v", "verbosity",
  "The level of verbosity of solver's output."
  "The higher the value, the more verbose the output will be. A verbosity of "
  "zero means total silence, even for error messages. Five or higher means "
  "total annoyance.",
  false, uint8_t{format::Message}, "number between 0 and 5");

TCLAP::SwitchArg sat(
  "s", "sat",
  "Uses sat solver to speed up propositional subformulas processing", false);

TCLAP::ValueArg<uint64_t> depth(
  "", "maximum-depth",
  "The maximum depth to descend into the tableaux (aka the maximum size of "
  "the model)",
  false, std::numeric_limits<uint64_t>::max(), "number");

TCLAP::ValueArg<uint32_t> backtrackProb(
  "", "backtrack-probability",
  "The probability of doing a complete backtrack of the tableau to check "
  "the LOOP and PRUNE rules (between 0 and 100)",
  false, 100, "percentage");

TCLAP::ValueArg<uint32_t> minBacktrack(
  "", "min-backtrack",
  "The minimum percentage of the tableau depth to backtrack during the "
  "check of LOOP and PRUNE rules (between 0 and 100)",
  false, 100, "percentage");

TCLAP::ValueArg<uint32_t> maxBacktrack(
  "", "max-backtrack",
  "The maximum percentage of the tableau depth to backtrack during the "
  "check of LOOP and PRUNE rules (between 0 and 100)",
  false, 100, "percentage");
}

// We suppose 80 columns is a good width
void print_progress_status(std::string formula, int i)
{
  std::string msg = format::format("Solving formula n° {}: ", i);

  formula.erase(std::remove(begin(formula), end(formula), '\n'), end(formula));

  std::string ellipses;
  if (formula.length() + msg.length() > 80) {
    formula = formula.substr(0, 80 - msg.length() - 3);
    ellipses = "...";
  }

  format::message("{}{}{}", msg, formula, ellipses);
}

void solve(std::string const &input, int i)
{
  print_progress_status(input, i);
  format::debug("Parsing...");

  optional<LTL::FormulaPtr> parsed = LTL::parse(input);

  if (!parsed) {
    format::error("Syntax error in formula n° {}. Skipping...", i);
    return;
  }

  LTL::FormulaPtr formula = *parsed;

  LTL::Solver solver(formula, LTL::FrameID(Args::depth.getValue()),
                     Args::backtrackProb.getValue(),
                     Args::minBacktrack.getValue(),
                     Args::maxBacktrack.getValue(), Args::sat.getValue());

  solver.solution();

  bool sat = solver.satisfiability() == LTL::Solver::Result::SATISFIABLE;
  format::message("The formula is {}!", sat ? colored(Green, "satisfiable")
                                            : colored(Red, "unsatisfiable"));

  if (sat && Args::model.isSet()) {
    LTL::ModelPtr model = solver.model();

    format::message("The following model was found:\n{}",
                    print_model(model, Args::parsable.isSet()));
  }
}

int main(int argc, char *argv[])
{
  TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ',
                     leviathan_version);

  using namespace Args;

  cmd.add(depth);
  cmd.add(backtrackProb);
  cmd.add(minBacktrack);
  cmd.add(maxBacktrack);
  cmd.add(sat);
  cmd.add(verbosity);
  cmd.add(parsable);
  cmd.add(model);
  cmd.add(ltl);
  cmd.add(filename);

  cmd.parse(argc, argv);

  // Setup the verbosity first of all
  auto level = static_cast<format::LogLevel>(
    std::min(verbosity.getValue(), uint8_t{format::Verbose}));

  format::max_log_level = level;

  format::verbose("Verbose message. I told you this would be very verbose.");

  // Begin to process inputs
  if (ltl.isSet())
    solve(ltl.getValue(), 1);
  else
    format::fatal("Batch solving unimplemented yet");

  return 0;
}
