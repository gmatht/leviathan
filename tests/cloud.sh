#!/bin/bash
set -e
make
set +e

[ -e benchmark.txt ] || (
  f tests/ pltl | while read f ; do timeout 11 time -p bin/checker_orig $f > /dev/null; echo $f; echo ---- ; done 2>&1 | tee benchmark.txt
)

for test in `< benchmark.txt tr '\n-' '\t\n' | grep ... | grep real.[1-9] | sed 's/.*\tt/t/'`
do
b=`basename $test|sed s/.pltl$//` 
for N in $@ 
do
echo $b:$N
[ -e out.$b.$N.txt ] ||
(
rm out.txt out.time || true
time -p (for i in `seq 1 $N`; do JOB_NO=$i/$N@9 timeout 30 bin/checker -v 4 $test & done; wait) 2> out.time > out.txt
cp out.txt out.$b.$N.txt
cp out.time out.$b.$N.time
)
done

(
echo ---- $b ---
echo -ne JOBS'\t'TIME'\t'
(echo 0 | average.sh > /dev/null) 2>&1
for N in $@
do
echo -ne $N'\t'`grep real out.$b.$N.time|sed 's/real\ *//'`'\t'
< out.$b.$N.txt grep Total.fram out.$b.$N.txt | sed 's/.*:.//' | average.sh 2> /dev/null
done
) | column -t | tee column.$b.txt
done



for N in $@ 99 999 
do
echo $N
[ -e out$N.txt ] ||
(
time -p (for i in `seq 1 $N`; do JOB_NO=$i/$N@9 timeout 30  bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X  ~a' & done; wait) 2> out.time > out.txt
cp out.txt out$N.txt
cp out.time out$N.time
)
done

echo ---- UNSAT ---
(
echo -ne JOBS'\t'TIME'\t'
(echo 0 | average.sh > /dev/null) 2>&1
for N in $@ 99 999
do
echo -ne $N'\t'`grep real out$N.time|sed 's/real\ *//'`'\t'
< out$N.txt grep Total.fram out$N.txt | sed 's/.*:.//' | average.sh 2> /dev/null
done
) | column -t | tee column.txt
