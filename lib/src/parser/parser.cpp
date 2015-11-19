#include "parser.hpp"
#include "Parser.h"
#include <sstream>

namespace LTL {

namespace detail {

optional<LTL::FormulaPtr> parse(const std::string &formula)
{
  std::stringstream ss;
  ss << formula;

  Parser p(ss);
  p.parse();

  if (p.error())
    return {};
  else
    return p.result();
}
}
}
