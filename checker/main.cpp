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

#include "input.hpp"

#include <limits>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>

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

TCLAP::SwitchArg test(
  "t", "test",
  "Test the checker answer. In this mode, the '-f' flag is mandatory. The "
  "given filename will be read for the formula, together with a .answer file "
  "named after it, containing the correct answer. The .answer file must "
  "contain the syntactic representation of the correct model of the given "
  "formula, if the formula is satisfiable, or be empty otherwise.",
  false);

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

void solve(InputFormula const &input, int i)
{
  print_progress_status(input.formula, i);
  format::message("Parsing...");

  optional<LTL::FormulaPtr> parsed = LTL::parse(input.formula);

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

  if (sat && Args::model.getValue()) {
    LTL::ModelPtr model = solver.model();

    format::message("The following model was found:\n{}", model);
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
  cmd.add(ltl);
  cmd.add(model);
  cmd.add(sat);
  cmd.add(filename);

  cmd.parse(argc, argv);

  InputData data;

  if (filename.isSet())
    data = readInput(filename.getValue(), test.getValue());
  else
    data = ltl.getValue();

  int i = 0;
  for (auto &input : data.inputs()) {
    solve(input, ++i);
  }
  return 0;
}
// std::vector<std::string> readFile(std::istream &input);
// void readableOutput(std::string const &f, bool modelFlag);
// void parsableOutput(std::string const &f);
//
// std::vector<std::string> readFile(std::istream &input)
//{
//  std::string line;
//  std::vector<std::string> lines;
//
//  while (std::getline(input, line)) {
//    if (line.empty() || line[0] == '#')
//      continue;
//
//    lines.push_back(line);
//  }
//
//  return lines;
//}
//
// void readableOutput(std::string const &f, bool modelFlag, uint64_t depth,
//                    uint32_t backtrack_probability, uint32_t min_backtrack,
//                    uint32_t max_backtrack, bool use_sat)
//{
//  LTL::PrettyPrinter printer;
//  format::message("Parsing formula\n");
//
//  LTL::FormulaPtr formula = nullptr;
//  bool error = false;
//
//  auto p1 = Clock::now();
//  std::tie(error, formula) = LTL::parse(f);
//  auto p2 = Clock::now();
//
//  if (!error)
//    format::message("Parsing time: {} us\n",
//                    duration_cast<microseconds>(p2 - p1).count());
//  else {
//    format::error("Syntax error! Skipping formula: {}\n", f);
//    return;
//  }
//
//  LTL::Solver solver(formula, LTL::FrameID(depth), backtrack_probability,
//                     min_backtrack, max_backtrack, use_sat);
//  format::message("Checking satisfiability...\n");
//  auto t1 = Clock::now();
//  solver.solution();
//  auto t2 = Clock::now();
//
//  format::message("Is satisfiable: ");
//  if (solver.satisfiability() == LTL::Solver::Result::SATISFIABLE)
//    format::message("\033[0;32mTrue\033[0m\n");
//  else
//    format::message("\033[0;31mFalse\033[0m\n");
//
//  format::message("Time taken: ");
//  auto time = (t2 - t1).count();
//  if (time > 5000000000)
//    format::message("{} s\n", duration_cast<seconds>(t2 - t1).count());
//  else if (time > 5000000)
//    format::message("{} ms\n", duration_cast<milliseconds>(t2 - t1).count());
//  else
//    format::message("{} us\n", duration_cast<microseconds>(t2 - t1).count());
//
//  if (modelFlag &&
//      solver.satisfiability() == LTL::Solver::Result::SATISFIABLE) {
//    LTL::ModelPtr model = solver.model();
//
//    uint64_t i = 0;
//    for (auto &state : model->states) {
//      format::message("State {}:\n", i);
//      for (auto &lit : state)
//        format::message("{}{}, \n", lit.positive() ? "" : "\u00AC",
//                        lit.atom());
//      ++i;
//    }
//
//    format::message("The model is looping to state {}\n", model->loop_state);
//  }
//
//  format::message("\n");
//}
//
// int main(int argc, char *argv[])
//{
//  TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ',
//                     leviathan_version);
//  TCLAP::ValueArg<std::string> filenameArg(
//    "f", "filename", "The name of the file to load the formulas from", false,
//    "test", "string", cmd);
//  TCLAP::ValueArg<std::string> ltlArg("l", "ltl", "The formula to test",
//  false,
//                                      "p && Xq", "string", cmd);
//
//  TCLAP::SwitchArg modelArg(
//    "m", "model",
//    "Generates and prints a model of the formula, when satisfiable", cmd,
//    false);
//  TCLAP::SwitchArg satArg(
//    "s", "sat",
//    "Uses sat solver to speed up propositional subformulas processing", cmd,
//    false);
//
//  TCLAP::ValueArg<uint64_t> depthArg(
//    "", "maximum-depth",
//    "The maximum depth to descend into the tableaux (aka the maximum size of
//    "
//    "the model)",
//    false, std::numeric_limits<uint64_t>::max(), "uint64_t", cmd);
//  TCLAP::ValueArg<uint32_t> backtrackPropArg(
//    "", "backtrack-probability",
//    "The probability of doing a complete backtrack of the tableaux to check "
//    "the LOOP and PRUNE rules (between 0 and 100)",
//    false, 100, "uint32_t", cmd);
//  TCLAP::ValueArg<uint32_t> minBacktrackArg(
//    "", "min-backtrack",
//    "The minimum percentage of the tableaux depth to backtrack during the "
//    "check of LOOP and PRUNE rules (between 0 and 100)",
//    false, 100, "uint32_t", cmd);
//  TCLAP::ValueArg<uint32_t> maxBacktrackArg(
//    "", "max-backtrack",
//    "The maximum percentage of the tableaux depth to backtrack during the "
//    "check of LOOP and PRUNE rules (between 0 and 100)",
//    false, 100, "uint32_t", cmd);
//
//  TCLAP::SwitchArg testArt(
//    "t", "test",
//    "Test the checker. In this mode, the '-f' flag is mandatory. The given "
//    "filename will be read for the formula, together with a .answer file "
//    "named after it, containing the correct answer. The .answer file must "
//    "contain the syntactic representation of the correct model of the given "
//    "formula, if the formula is satisfiable, or be empty otherwise.",
//    cmd, false);
//
//  cmd.parse(argc, argv);
//
//  std::vector<std::string> formulas;
//
//  if (filenameArg.isSet() && filenameArg.getValue() != "-") {
//    std::ifstream file(filenameArg.getValue(), std::ios::in);
//    if (!file.is_open()) {
//      format::error("File not found!\n");
//      return 1;
//    }
//
//    formulas = readFile(file);
//  }
//
//  if (!ltlArg.isSet() &&
//      (!filenameArg.isSet() || filenameArg.getValue() == "-"))
//    formulas = readFile(std::cin);
//
//  if (ltlArg.isSet())
//    formulas.push_back(ltlArg.getValue());
//
//  for (const auto &f : formulas) {
//    readableOutput(f, modelArg.getValue(), depthArg.getValue(),
//                   backtrackPropArg.getValue(), minBacktrackArg.getValue(),
//                   maxBacktrackArg.getValue(),
//                   satArg.getValue());  // For some definition of "readable"
//  }
//
//  // std::this_thread::sleep_until(time_point<system_clock>::max());
//  return 0;
//}
