#pragma once

#include <vector>
#include <tuple>
#include <bitset>
#include <limits>
#include <unordered_map>
#include <stack>
#include <queue>
#include "boost/dynamic_bitset.hpp"
#include "boost/pool/pool_alloc.hpp"
#include "generator.hpp"

namespace LTL
{

namespace detail
{

const uint64_t MAX_FRAME = std::numeric_limits<uint64_t>::max();
const uint64_t MAX_FORMULA = std::numeric_limits<uint64_t>::max();

using Bitset = boost::dynamic_bitset<uint64_t>;
//using Bitset = boost::dynamic_bitset<uint64_t, boost::fast_pool_allocator<uint64_t>>;

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
