#!/bin/bash

if [ "$1" = "check" ]; then
  command="clang-format --style=file"
elif [ "$1" = "format" ]; then
  command="clang-format -i --style=file"
else
  echo 'Please specify an action (check or format)' 1>&2
  exit 1
fi

export command
find ./lib ./checker \
  \( -name '*.h' -or -name '*.cpp' \) -exec \
  sh -c 'if [ ! -z "$($command {})" ]; then echo {}; fi' \
  \; \