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

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <type_traits>
#include <iostream>

#ifndef MSVC
#include <unistd.h>
#endif

#include "cppformat/format.h"

namespace LTL {
namespace detail {
namespace format {

using namespace fmt;

/*
 * POSIX-dependent solution to tell if the stream is attached to a
 * terminal device (so we can output colors).
 * Note that we should ideally test the actual stream we are writing to,
 * but since C++Format uses an intermediate stringbuffer, we do not have
 * access to the actual output device from the operator<< overload of
 * the colored_t class. At this point we do not know the verbosity level of the
 * message, neither.
 * For this reason, the best we can do is to shut down colors if any of the two
 * output device is not a tty.
 *
 * TODO: implement on windows
 */
inline bool isatty(std::ostream &os)
{
#ifdef _MSC_VER
  return false;
#else
  return ::isatty(fileno(stdout)) && ::isatty(fileno(stderr));
#endif
}

/*
 * Facilities to produce colored output.
 * Usage:
 *
 * using namespace format::colors;
 *
 * std::cout << set_color(Red) << "Colored!" << set_color(Reset) << std::endl;
 *
 * format::message("Hello: {}\n", colored(Red, "Colored!"));
 */

namespace colors {

enum Color {
  Reset = 0,

  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  Cyan = 36,
  Transparent = 39,

  BgRed = 41,
  BgGreen = 42,
  BgYellow = 43,
  BgBlue = 44,
  BgMagenta = 45,
  BgCyan = 46,
  BgTransparent = 49,
};

/*
 * I/O stream manipulator to set colors
 */
class color_t {
public:
  color_t(Color color) : _color(color) {}
  friend std::ostream &operator<<(std::ostream &os, color_t c)
  {
    if (isatty(os)) {
      os << "\033[" << (int)c._color << "m";
    }
    return os;
  }

private:
  Color _color;
};

inline color_t set_color(Color c)
{
  return color_t{c};
};

inline std::ostream &set_color(std::ostream &os, Color c)
{
  return os << set_color(c);
};

template <typename T>
class colored_t {
public:
  colored_t(Color color, T const &obj) : _color(color), _obj(obj) {}
  friend std::ostream &operator<<(std::ostream &os, colored_t c)
  {
    return os << set_color(c._color) << c._obj << set_color(Reset);
  }

private:
  T const &_obj;
  Color _color;
};

/*
 * wrapper object to set colors in format::log's output
 */
template <typename T>
colored_t<T> colored(Color color, T &&obj)
{
  return colored_t<T>(color, std::forward<T>(obj));
}

inline colored_t<const char *> colored(Color color, const char *str)
{
  return colored_t<const char *>(color, str);
}

}  // namespace colors

using namespace colors;

/*
 * Printing functions
 */
enum LogLevel : uint8_t { Error = 0, Warning, Message, Debug, Verbose };

extern LogLevel max_log_level;

template <typename CharTy, typename... Args>
auto log(LogLevel level, const CharTy *fmt, Args &&... args)
  -> decltype(fmt::print(std::declval<std::ostream &>(), fmt,
                         std::forward<Args>(args)...))
{
  std::ostream &out = uint8_t(level) >= Message ? std::cout : std::cerr;

  if (uint8_t(level) <= max_log_level) {
    fmt::print(out, fmt, std::forward<Args>(args)...);
    out << std::endl;
  }
}

// Same as error() but terminates the process. Don't use lightly
template <typename CharTy, typename... Args>
auto fatal(const CharTy *fmt, Args &&... args)
  -> decltype(log(Error, fmt, std::forward<Args>(args)...))
{
  log(Error, fmt, std::forward<Args>(args)...);
  exit(EXIT_FAILURE);
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
}  // namespace format
}  // namespace detail
}  // namespace LTL