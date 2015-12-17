#!/bin/bash

while IFS=, read -r -a LINE; do
  [ ${LINE[8]} != "UNK" ] || continue
  
  if gtimeout --foreground -s KILL 5s \
    ./bin/checker --parsable --verbosity 0 ${LINE[0]}
  then
    echo ${LINE[0]} ${LINE[8]}
  fi
done 
