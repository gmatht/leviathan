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
  Literal(const std::string &atom, bool positive = true)
    : _positive(positive), _atom(atom)
  {
  }

  bool positive() const { return _positive; }
  bool negative() const { return !_positive; }
  std::string atom() const { return _atom; }
  friend bool operator==(const Literal &l1, const Literal &l2)
  {
    return l1.positive() == l2.positive() && l1.atom() == l2.atom();
  }

  friend bool operator!=(const Literal &l1, const Literal &l2)
  {
    return !(l1 == l2);
  }

  friend bool operator<(const Literal &l1, const Literal &l2)
  {
    if (l1._atom == l2._atom) {
      if (l1.positive() && l2.negative())
        return true;
      else
        return false;
    }

    return std::lexicographical_compare(l1._atom.begin(), l1._atom.end(),
                                        l2._atom.begin(), l2._atom.end());
  }

private:
  bool _positive;
  std::string _atom;
};

using State = std::set<Literal>;

struct Model {
  std::vector<State> states;
  uint64_t loop_state{};
};

using ModelPtr = std::shared_ptr<Model>;
}
}
