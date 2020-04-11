TEST=H
for DEPTH in `seq 10 20`
do
for f in `cut -f2 ../tests/lists/$TEST`
do
  echo -n "$DEPTH	" #echo [$f] # `cat ../tests/$f`
  export DEPTH
  nCPU=4 nJOB=4 timeout 1 ./parallel_pltl_tree.sh "`cat ../tests/$f`" $(echo $f | sed s,/,_,g) 
  killall pltl
  ps gaux | grep pltl
done | tee out.$TEST.$DEPTH.txt
done
