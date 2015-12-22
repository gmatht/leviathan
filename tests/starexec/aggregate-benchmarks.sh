#!/bin/bash

columns() {
  fields=$1
  file=$2

  cat $file | sed '1 d' | cut -d , -f $fields
}

if [ $# -lt 2 ]; then
   cat <<LOL 1>&2
Usage:
   aggregate-benchmarks.sh <output file> <file1> [<file2> <file3> ...]
LOL
  exit 1
fi

output=$1

shift

echo "benchmark" > $output
columns 1 $1 >> $output

while [ ! -z "$1" ]; do
  
  file=$1
  shift

  [ $output != $file ] || continue

  echo "Aggregating $file..."

  solver=$(basename "$file" .csv)
  
  header="$solver-status,$solver-time,$solver-memory"
  
  paste -d , $output <(echo $header; columns 4,5,7 $file) > $output.tmp

  mv $output.tmp $output

done

# Convert to space-separated table
cat $output                       \
  | sed "s/timeout (.*)/timeout/" \
  | column -t -s ,                \
  | tee $output > /dev/null

