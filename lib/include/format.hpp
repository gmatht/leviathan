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

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <utility>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#pragma clang diagnostic push

#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wweak-vtables"

#include "cppformat/format.h"

#pragma clang diagnostic pop

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
inline bool isatty()
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
    if (isatty()) {
      os << "\033[" << int(c._color) << "m";
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
  colored_t(Color color, T const &obj) : _obj(obj), _color(color) {}
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
 * Verbosity level
 */
enum LogLevel : uint8_t { Silent = 0, Error, Info, Message, Debug, Verbose };

inline std::istream &operator>>(std::istream &s, LogLevel &level)
{
  unsigned in;
  s >> in;
  if (in > Verbose) {
    s.setstate(std::ios::failbit);
    return s;
  }

  level = static_cast<LogLevel>(in);
  return s;
}

/*
 * Functions to set and probe the verbosity level
 */
void set_verbosity_level(LogLevel level);
LogLevel verbosity_level();

template <typename CharTy, typename... Args>
auto log(LogLevel level, Color color, const CharTy *fmt, Args &&... args)
  -> decltype(fmt::print(std::declval<std::ostream &>(), fmt,
                         std::forward<Args>(args)...))
{
  std::ostream &out = uint8_t(level) >= Message ? std::cout : std::cerr;

  if (uint8_t(level) <= verbosity_level()) {
    out << set_color(color);
    fmt::print(out, fmt, std::forward<Args>(args)...);
    out << set_color(Reset) << std::endl;
  }
}

template <typename CharTy, typename... Args>
auto log(LogLevel level, const CharTy *fmt, Args &&... args)
  -> decltype(log(level, Reset, fmt, std::forward<Args>(args)...))
{
  return log(level, Reset, fmt, std::forward<Args>(args)...);
}

// Same as error() but terminates the process. Don't use lightly
template <typename CharTy, typename... Args>
auto fatal(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Error, fmt, std::forward<Args>(args)...))
{
  log(Error, color, fmt, std::forward<Args>(args)...);
  exit(EXIT_FAILURE);
}

template <typename CharTy, typename... Args>
auto fatal(const CharTy *fmt, Args &&... args)
  -> decltype(fatal(Reset, fmt, std::forward<Args>(args)...))
{
  return fatal(Reset, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto error(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Error, fmt, std::forward<Args>(args)...))
{
  log(Error, color, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto error(const CharTy *fmt, Args &&... args)
  -> decltype(error(Reset, fmt, std::forward<Args>(args)...))
{
  return error(Reset, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto message(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Message, fmt, std::forward<Args>(args)...))
{
  log(Message, color, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto message(const CharTy *fmt, Args &&... args)
  -> decltype(message(Reset, fmt, std::forward<Args>(args)...))
{
  return message(Reset, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto info(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Info, fmt, std::forward<Args>(args)...))
{
  log(Info, color, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto info(const CharTy *fmt, Args &&... args)
  -> decltype(info(Reset, fmt, std::forward<Args>(args)...))
{
  return info(Reset, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto debug(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Debug, fmt, std::forward<Args>(args)...))
{
  log(Debug, color, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto debug(const CharTy *fmt, Args &&... args)
  -> decltype(debug(Reset, fmt, std::forward<Args>(args)...))
{
  return debug(Reset, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto verbose(Color color, const CharTy *fmt, Args &&... args)
  -> decltype(log(Verbose, fmt, std::forward<Args>(args)...))
{
  log(Verbose, color, fmt, std::forward<Args>(args)...);
}

template <typename CharTy, typename... Args>
auto verbose(const CharTy *fmt, Args &&... args)
  -> decltype(verbose(Reset, fmt, std::forward<Args>(args)...))
{
  return verbose(Reset, fmt, std::forward<Args>(args)...);
}

}  // namespace format
}  // namespace detail
}  // namespace LTL
