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

#pragma once

#include <memory>
#include <vector>
#include <set>

namespace LTL {
namespace detail {

struct Literal {
  Literal(const std::string &formula, bool positive = true)
    : _positive(positive), _atomic(formula)
  {
  }

  bool positive() const { return _positive; }
  bool negative() const { return !_positive; }
  std::string atomic_formula() const { return _atomic; }
  friend bool operator==(const Literal &l1, const Literal &l2)
  {
    return l1.positive() == l2.positive() &&
           l1.atomic_formula() == l2.atomic_formula();
  }

  friend bool operator!=(const Literal &l1, const Literal &l2)
  {
    return !(l1 == l2);
  }

  friend bool operator<(const Literal &l1, const Literal &l2)
  {
    if (l1._atomic == l2._atomic) {
      if (l1.positive() && l2.negative())
        return true;
      else
        return false;
    }

    return std::lexicographical_compare(l1._atomic.begin(), l1._atomic.end(),
                                        l2._atomic.begin(), l2._atomic.end());
  }

private:
  bool _positive;
  std::string _atomic;
};

using State = std::set<Literal>;

struct Model {
  std::vector<State> states;
  uint64_t loop_state{};
};

using ModelPtr = std::shared_ptr<Model>;
}
}
