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

#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <tuple>
#include <limits>

using namespace std::chrono;

#include "tclap/CmdLine.h"
#include "leviathan.hpp"

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

using Clock = high_resolution_clock;
const std::string leviathan_version = "0.2.2";

std::vector<std::string> readFile(std::istream &input);
void readableOutput(std::string const &f, bool modelFlag);
void parsableOutput(std::string const &f);

std::vector<std::string> readFile(std::istream &input)
{
  std::string line;
  std::vector<std::string> lines;

  while (std::getline(input, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    lines.push_back(line);
  }

  return lines;
}

void readableOutput(std::string const &f, bool modelFlag, uint64_t depth,
                    uint32_t backtrack_probability, uint32_t min_backtrack,
                    uint32_t max_backtrack, bool use_sat)
{
  LTL::PrettyPrinter printer;
  format::message("Parsing formula\n");

  LTL::FormulaPtr formula = nullptr;
  bool error = false;

  auto p1 = Clock::now();
  std::tie(error, formula) = LTL::parse(f);
  auto p2 = Clock::now();

  if (!error)
    format::message("Parsing time: {} us\n",
                    duration_cast<microseconds>(p2 - p1).count());
  else {
    format::error("Syntax error! Skipping formula: {}\n", f);
    return;
  }

  LTL::Solver solver(formula, LTL::FrameID(depth), backtrack_probability,
                     min_backtrack, max_backtrack, use_sat);
  format::message("Checking satisfiability...\n");
  auto t1 = Clock::now();
  solver.solution();
  auto t2 = Clock::now();

  format::message("Is satisfiable: ");
  if (solver.satisfiability() == LTL::Solver::Result::SATISFIABLE)
    format::message("\033[0;32mTrue\033[0m\n");
  else
    format::message("\033[0;31mFalse\033[0m\n");

  format::message("Time taken: ");
  auto time = (t2 - t1).count();
  if (time > 5000000000)
    format::message("{} s\n", duration_cast<seconds>(t2 - t1).count());
  else if (time > 5000000)
    format::message("{} ms\n", duration_cast<milliseconds>(t2 - t1).count());
  else
    format::message("{} us\n", duration_cast<microseconds>(t2 - t1).count());

  if (modelFlag &&
      solver.satisfiability() == LTL::Solver::Result::SATISFIABLE) {
    LTL::ModelPtr model = solver.model();

    uint64_t i = 0;
    for (auto &state : model->states) {
      format::message("State {}:\n", i);
      for (auto &lit : state)
        format::message("{}{}, \n", lit.positive() ? "" : "\u00AC",
                        lit.atomic_formula());
      ++i;
    }

    format::message("The model is looping to state {}\n", model->loop_state);
  }

  format::message("\n");
}

int main(int argc, char *argv[])
{
  TCLAP::CmdLine cmd("A simple LTL satisfiability checker", ' ',
                     leviathan_version);
  TCLAP::ValueArg<std::string> filenameArg(
    "f", "filename", "The name of the file to load the formulas from", false,
    "test", "string", cmd);
  TCLAP::ValueArg<std::string> ltlArg("l", "ltl", "The formula to test", false,
                                      "p && Xq", "string", cmd);

  TCLAP::SwitchArg modelArg(
    "m", "model",
    "Generates and prints a model of the formula, when satisfiable", cmd,
    false);
  TCLAP::SwitchArg satArg(
    "s", "sat",
    "Uses sat solver to speed up propositional subformulas processing", cmd,
    false);

  TCLAP::ValueArg<uint64_t> depthArg(
    "", "maximum-depth",
    "The maximum depth to descend into the tableaux (aka the maximum size of "
    "the model)",
    false, std::numeric_limits<uint64_t>::max(), "uint64_t", cmd);
  TCLAP::ValueArg<uint32_t> backtrackPropArg(
    "", "backtrack-probability",
    "The probability of doing a complete backtrack of the tableaux to check "
    "the LOOP and PRUNE rules (between 0 and 100)",
    false, 100, "uint32_t", cmd);
  TCLAP::ValueArg<uint32_t> minBacktrackArg(
    "", "min-backtrack",
    "The minimum percentage of the tableaux depth to backtrack during the "
    "check of LOOP and PRUNE rules (between 0 and 100)",
    false, 100, "uint32_t", cmd);
  TCLAP::ValueArg<uint32_t> maxBacktrackArg(
    "", "max-backtrack",
    "The maximum percentage of the tableaux depth to backtrack during the "
    "check of LOOP and PRUNE rules (between 0 and 100)",
    false, 100, "uint32_t", cmd);

  TCLAP::SwitchArg testArt(
    "t", "test",
    "Test the checker. In this mode, the '-f' flag is mandatory. The given "
    "filename will be read for the formula, together with a .answer file "
    "named after it, containing the correct answer. The .answer file must "
    "contain the syntactic representation of the correct model of the given "
    "formula, if the formula is satisfiable, or be empty otherwise.",
    cmd, false);

  cmd.parse(argc, argv);

  std::vector<std::string> formulas;

  if (filenameArg.isSet() && filenameArg.getValue() != "-") {
    std::ifstream file(filenameArg.getValue(), std::ios::in);
    if (!file.is_open()) {
      format::error("File not found!\n");
      return 1;
    }

    formulas = readFile(file);
  }

  if (!ltlArg.isSet() &&
      (!filenameArg.isSet() || filenameArg.getValue() == "-"))
    formulas = readFile(std::cin);

  if (ltlArg.isSet())
    formulas.push_back(ltlArg.getValue());

  for (const auto &f : formulas) {
    readableOutput(f, modelArg.getValue(), depthArg.getValue(),
                   backtrackPropArg.getValue(), minBacktrackArg.getValue(),
                   maxBacktrackArg.getValue(),
                   satArg.getValue());  // For some definition of "readable"
  }

  // std::this_thread::sleep_until(time_point<system_clock>::max());
  return 0;
}
