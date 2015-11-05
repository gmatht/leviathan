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

template <class T, class S>
inline const S &Container(const std::stack<T, S> &s)
{
  struct Stack : private std::stack<T, S> {
    static const S &Container(const std::stack<T, S> &_s)
    {
      return _s.*&Stack::c;
    }
  };
  return Stack::Container(s);
}

template <class Cont>
class const_reverse_wrapper {
  const Cont &container;

public:
  const_reverse_wrapper(const Cont &cont) : container(cont) {}
  inline decltype(container.rbegin()) begin() const
  {
    return container.rbegin();
  }

  inline decltype(container.rend()) end() const { return container.rend(); }
};

template <class Cont>
class reverse_wrapper {
  Cont &container;

public:
  reverse_wrapper(Cont &cont) : container(cont) {}
  inline decltype(container.rbegin()) begin() { return container.rbegin(); }
  inline decltype(container.rend()) end() { return container.rend(); }
};

template <class Cont>
const_reverse_wrapper<Cont> reverse(const Cont &cont)
{
  return const_reverse_wrapper<Cont>(cont);
}

template <class Cont>
reverse_wrapper<Cont> reverse(Cont &cont)
{
  return reverse_wrapper<Cont>(cont);
}
