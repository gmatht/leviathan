#!/usr/bin/env bash

if [ "x$TRAVIS_CLANG_VERSION" != "x" ]; then

  if [ "$CC" = "clang" ]; then
    CC=clang-$TRAVIS_CLANG_VERSION
  fi

  if [ "$CXX" = "clang" ]; then
    CXX=clang++-$TRAVIS_CLANG_VERSION
  fi

fi

if [ "x$TRAVIS_GNU_VERSION" != "x" ]; then

  if [ "$CC" = "gcc" -a  ]; then
    CC=gcc-$TRAVIS_GNU_VERSION
  fi

  if [ "$CXX" = "g++" -a  ]; then
    CXX=g++-$TRAVIS_GNU_VERSION
  fi
fi

