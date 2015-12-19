#!/bin/bash

while IFS=", " read -r -a LINE; do

  filename=${LINE[0]}
  solver=${LINE[1]}
  status=${LINE[3]}
  time=${LINE[4]}
  memory=${LINE[6]}

  # Transform CSV header
  if [ "$filename" = "benchmark" ]; then
    echo benchmark solver status time memory
    continue
  fi

  # Convert status field into a boolena numeric value
  [ "$status" = "complete" ] && status=0 || status=1

  echo $filename $solver $status $time $memory

done | column -t