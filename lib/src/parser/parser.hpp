#pragma once

#include <tuple>
#include "formula.hpp"

namespace LTL
{

namespace detail
{

std::pair<bool, FormulaPtr> parse(const std::string& formula);

}

}
