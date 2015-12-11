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

#include <sstream>
#include "Parser.h"
#include "parser.hpp"

#include "format.hpp"

namespace LTL {

namespace detail {

optional<LTL::FormulaPtr> parse(const std::string &formula)
{
  format::debug("Parsing...");
  std::stringstream ss;
  ss << formula;

  Parser p(ss);
  p.parse();

  if (p.error())
    return {};

  return p.result();
}
}
}
