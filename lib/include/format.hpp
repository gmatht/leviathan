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

#ifndef FMT_USE_VARIADIC_TEMPLATES
#error "C++ Format must be compiled with support for variadic templates"
#endif

#include <cstdint>
#include <utility>
#include <iostream>

#include "cppformat/format.h"

namespace LTL {
namespace detail {
namespace format {

enum LogLevel : uint8_t { Error = 0, Warning, Message, Debug, Verbose };

extern LogLevel max_log_level;

template <typename CharTy, typename... Args>
auto log(LogLevel level, const CharTy *fmt, Args &&... args)
  -> decltype(fmt::print(std::declval<std::ostream &>(), fmt,
                         std::forward<Args>(args)...))
{
  std::ostream &out = uint8_t(level) >= Message ? std::cout : std::cerr;

  if (uint8_t(level) <= max_log_level)
    fmt::print(out, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto error(const CharTy *fmt, Args &&... args)
  -> decltype(log(Error, fmt, std::forward<Args>(args)...))
{
  log(Error, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto warning(const CharTy *fmt, Args &&... args)
  -> decltype(log(Warning, fmt, std::forward<Args>(args)...))
{
  log(Warning, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto message(const CharTy *fmt, Args &&... args)
  -> decltype(log(Message, fmt, std::forward<Args>(args)...))
{
  log(Message, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto debug(const CharTy *fmt, Args &&... args)
  -> decltype(log(Debug, fmt, std::forward<Args>(args)...))
{
  log(Debug, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto verbose(const CharTy *fmt, Args &&... args)
  -> decltype(log(Verbose, fmt, std::forward<Args>(args)...))
{
  log(Verbose, fmt, std::forward<Args>(args)...);
}
}
}
}