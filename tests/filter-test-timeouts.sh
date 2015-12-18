#!/bin/bash

die() {
  echo \
This script must be executed from the root directory of leviathan\'s source \
tree. 1>&2
  exit 1
}

timeout() {
  if [ "$(uname)" = "Darwin" ]; then
    gtimeout $@
  else
    timeout $@
  fi
}

test -d .git || die

while IFS=", " read -r -a LINE; do
  [ ${LINE[8]} != "UNK" ] || continue
  
  timeout --foreground -s KILL 3s \
    ./bin/checker --verbosity 0 ${LINE[0]}

  # timeout returns 124 or 137 if it had to kill the process
  # Yes I love Unix...
  if [ $? -ne 124 -a $? -ne 137 ]; then
    echo "${LINE[0]};${LINE[8]}" | sed 's/UNS/UNSAT/'
  fi
done 
