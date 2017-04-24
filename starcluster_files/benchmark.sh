git diff .. > ~/store/diff.txt
. ~/nodes.sh
for n in $NODES localhost; do ssh $n 'killall    checker'; done
for n in $NODES localhost; do ssh $n 'killall -9 checker'; done

if [ -z "$nCPU"  ]
then nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ~/ssh.txt`
fi
nJOB=$((nCPU*nNODE))

export nCPU
export nNODE
export nJOB

renice -10 $$

mkdir -p ~/store
for L in U0 U1 U2 U3 S0 S1 S2 S3 S4 H
do
cat ../tests/lists/$L | while read t f
#f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
 do echo --- $t $f
 i=$((i+1))
 NAME="$L"_`printf %3d  $i | tr \  0`
 time -p timeout 1000 bash parallel.sh "`cat ../tests/$f`" $NAME
 bash makelog.sh "`cat ../tests/$f`" $NAME "$f"
 done 
done 2>&1 | tee ~/store/summary.txt
