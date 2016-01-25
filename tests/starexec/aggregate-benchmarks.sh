#!/bin/bash

[ "$1" != "" ] || (echo "Give me the args, bro" && exit 1)

solvers="aalta trp++ ls4 Leviathan-06d4951 NuSMV-2.6.0,BMC NuSMV-2.6.0,BDD pltl,graph pltl,tree"

formulae=$(find acacia alaska/lift schuppan trp -depth 1; find rozier anzu -depth 2)

filt() {
  cat $1 | grep $formula | grep $solver | grep $2 | wc -l
}

memory() {
  cat $1 | grep $formula | grep $solver | grep -v timeout | grep -v memout | \
    cut -d',' -f 7 | awk '{s+=$1;n+=1} END {if (n == 0) {print "-"} else {print s/n}}'
}

(
echo -n "formula "
for solver in $solvers; do
  #echo -n "$solver-memouts $solver-timeouts "
  echo -n "$solver-memory "
done
echo

for formula in $formulae; do
  echo -n "$formula "
  for solver in $solvers; do
    #echo -n "$(filt $1 memout) $(filt $1 timeout) "
    echo -n "$(memory $1) "
  done
  echo
done
) | column -t