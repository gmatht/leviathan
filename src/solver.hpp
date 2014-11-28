#pragma once

#include <vector>
#include <tuple>
#include <bitset>
#include <limits>
#include <unordered_map>
#include <stack>
#include <queue>
#include "generator.hpp"

namespace LTL
{

namespace detail
{

#ifndef MAX_FORMULA_SIZE
#define MAX_FORMULA_SIZE 64
#endif

const uint64_t MAX_FRAME = std::numeric_limits<uint64_t>::max();
const uint64_t MAX_FORMULA = std::numeric_limits<uint64_t>::max();

using Bitset = std::bitset<MAX_FORMULA_SIZE * 4>;

template <class T, class S>
const S& Container(const std::stack<T, S>& s)
{
        struct Stack : private std::stack<T, S>
        {
                static const S& Container(const std::stack<T, S>& _s)
                {
                        return _s.*&Stack::c;
                }
        };
        return Stack::Container(s);
}

std::tuple<bool, std::vector<FormulaSet>, uint64_t> is_satisfiable(const FormulaPtr formula, bool model);
    
}
}
