#include "parser.hpp"
#include "Parser.h"
#include <sstream>

namespace LTL
{

namespace detail
{

std::pair<bool, LTL::FormulaPtr> parse(const std::string& formula)
{
	std::stringstream ss;
	ss << formula;

	Parser p(ss);
	p.parse();

	if (p.error())
		return { true, nullptr };
	else 
		return { false, p.result() };
}

}

}
