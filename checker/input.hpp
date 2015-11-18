/*
 Copyright (c) 2014, Nicola Gigante
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

#include "leviathan.hpp"
#include "optional.hpp"

#include <fstream>

namespace format = LTL::format;

using std::experimental::optional;
using std::experimental::nullopt;
using std::experimental::make_optional;

/*
 * Intermediate data representing inputs
 */

// The test data associated with a formula
struct TestData {
  bool satisfiable = false;
  optional<std::string> model;

  TestData() = default;
  TestData(TestData const &) = default;
  TestData(TestData &&) = default;

  TestData(bool sat) : satisfiable(sat) {}
  TestData(bool sat, std::string m) : satisfiable(sat), model(m) {}
};

// An input formula
struct InputFormula {
  std::string formula;
  optional<TestData> testData;

  InputFormula() = default;
  InputFormula(InputFormula const &) = default;
  InputFormula(InputFormula &&) = default;
  InputFormula &operator=(InputFormula const &) = default;
  InputFormula &operator=(InputFormula &&) = default;

  InputFormula(std::string f) : formula(std::move(f)) {}
  InputFormula(std::string f, TestData t) : formula(std::move(f)), testData(t)
  {
  }
};

// A collection of input formulae and corresponding test data
class InputData {
public:
  InputData() = default;
  InputData(InputData const &) = default;
  InputData(InputData &&) = default;
  InputData &operator=(InputData const &) = default;
  InputData &operator=(InputData &&) = default;

  InputData(std::string formula) : _inputs{InputFormula(std::move(formula))} {}
  InputData(std::vector<std::string> formulae)
  {
    std::transform(begin(formulae), end(formulae), std::back_inserter(_inputs),
                   [](std::string s) { return InputFormula(std::move(s)); });
  }

  InputData(std::vector<InputFormula> inputs) : _inputs(std::move(inputs)) {}
  std::vector<InputFormula> const &inputs() const { return _inputs; }
private:
  std::vector<InputFormula> _inputs;
};

inline std::vector<std::string> readLines(std::istream &in)
{
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(in >> std::ws, line)) {
    line = line.substr(0, line.find('#'));

    if (!line.empty())
      lines.push_back(line);
  }

  return lines;
}

inline InputData readInput(std::string filename, bool test)
{
  InputData data;

  std::vector<std::string> formulae;
  {
    std::ifstream file(filename, std::ios::in);

    if (!file) {
      format::fatal("Unable to open input data from file '{}': {}", filename,
                    strerror(errno));
    }

    formulae = readLines(file);
  }

  if (test) {
    std::string testfilename = filename + ".answer";
    std::ifstream testfile(testfilename, std::ios::in);

    if (!testfile) {
      format::fatal("Unable to open test data from file '{}': {}",
                    testfilename, strerror(errno));
    }

    std::vector<std::string> tests = readLines(testfile);

    if (tests.size() < formulae.size())
      format::warning(
        "There are {} input formulae but only {} answers in the test file. "
        "The remaining {} formulae will not be tested.",
        formulae.size(), tests.size(), formulae.size() - tests.size());

    std::vector<InputFormula> inputs;
    for (int i = 0; i < std::min(tests.size(), formulae.size()); ++i) {
      inputs.push_back(InputFormula(formulae[i]));
      // TODO: read test cases
    }
  }
  else {
    data = InputData(std::move(formulae));
  }

  return data;
}
