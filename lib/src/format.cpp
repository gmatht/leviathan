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

#include "format.hpp"

namespace LTL {
namespace detail {
namespace format {

namespace {
std::atomic<LogLevel> level{Message};
}

void set_verbosity_level(LogLevel l)
{
  level = l;
}

LogLevel verbosity_level()
{
  return level;
}
}
}
}