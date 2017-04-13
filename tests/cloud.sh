#!/bin/bash
set -e
make
set +e

mkdir -p out
[ -e benchmark.txt ] || 
(
  f tests/ pltl | while read f ; do timeout 1000 time -p bin/checker_orig -v 4 $f | egrep '(WIDTH|Total|depth|The formula is)'; echo $f; echo ---- ; done 2>&1 | tee benchmark.txt
)

mkdir tests/john 2> /dev/null || true
[ -e tests/john/XX4.pltl ] || echo '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X  X X  X X  X X  ~a' > tests/john/XX4.pltl


do_set () {
N=$1
depth=$2
test=$3
rm out.time
rm out.txt
rm out?.txt
min4N=`(echo 4; echo $N) | sort -n | head -n1`
echo min4N $min4N
time -p (
for i in `seq 1 $min4N`; do JOB_NO=$i/$N@$depth time -p timeout 1000 bin/checker -v 4 $test 2> out$i.time > out$i.txt & done 
for i in `seq 5 $N` 0; do JOB_NO=$i/$N@$depth time -p timeout 1000 bin/checker -v 4 $test 2> out$i.time > out$i.txt & wait -n; done
wait
) 2> out.time > out.txt
#time -p (for i in `seq 1 $N`; do JOB_NO=$i/$N@$depth timeout 11 bin/checker -v 4 -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X  ~a' & done; wait) 2> out.time > out.txt
echo $N $depth $test
cat out.time
grep "X" out?.txt | wc -l
for i in `seq 0 $N`; do (grep user out$i.time; grep Total out$i.txt; grep ^"X" out$i.txt | wc -l; grep formula.is out$i.txt) | tr '\n' ':'; echo; done
for i in `seq 0 $N`; do  grep Total out$i.txt; done | sed s/Total.frames:.// | average.sh 2>&1
echo @
}

#for N in 1 2 4 8 16 32 64 128 256 512
for N in 1 2 5 9 # 17 22 65 129 # 513 
do
do_set $N 11 tests/john/XX4.pltl 
done

exit

#do_set 1 9 "-l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X  X X  X X  X X  ~a'"
#do_set 9 9 "-l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X  ~a'"

for d in 1 2 0 
do
do_set 1 $d tests/rozier/pattern/Uformula/Uformula300.pltl
do_set 2 $d tests/rozier/pattern/Uformula/Uformula300.pltl
do_set 4 $d tests/rozier/pattern/Uformula/Uformula300.pltl
do_set 9 $d tests/rozier/pattern/Uformula/Uformula300.pltl
done | tee test.txt

exit

if false
then
test=./tests/rozier/pattern/C2formula/C2formula100.pltl

N=$2
time -p (for i in `seq 1 $N`; do JOB_NO=$i/$N@$1 timeout 1 bin/checker -v 4 $test > out.t$i.txt & done; wait) # 2> out.time > out.txt

#for i in `seq 1 $N`; do grep is.sat out.t$i.txt > /dev/null && grep Total out.t$i.txt; done
for i in `seq 1 $N`; do echo `grep Total out.t$i.txt` `grep is.sat out.t$i.txt`; done

#cp out.txt out_$N.txt
#cp out.txt out_$N.time

exit
fi

for test in `< benchmark.txt tr '\n-' '\t\n' | grep ... | grep Total\ frames:\ ..... | sed 's/.*\tt/t/'`
do
b=`basename $test|sed s/.pltl$//` 
for N in $@ 
do
echo $b:$N
[ -e out.$b.$N.txt ] ||
(
rm out.txt out.time || true
time -p (for i in `seq 1 $N`; do JOB_NO=$i/$N@9 timeout 3 bin/checker -v 4 $test & done; wait) 2> out.time > out.txt
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
