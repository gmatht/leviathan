#!/bin/bash
FORMULA=$1
NAME=$2
. ./nodes.sh
echo > pids
nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE))
rm ~/out/result.$NAME.txt || true 2> /dev/null
j=1; ( for n in $NODES master
do
        < /dev/null ssh -t -t $n "for i in `seq $j $((j+nCPU-1))`; do JOB_NO=\$i/$nJOB@9 checker -l '$FORMULA' > log.$NAME.\$i.txt; done; sleep 1" &
        echo $! >> pids
        j=$((j+$nCPU))
done ; wait ) 2>&1 | if read L; then echo $L >  ~/out/result.$NAME.txt; fi
kill `cat pids`
ps gaux | grep leep
echo -n "VOTE: "
cat  ~/out/result.$NAME.txt || echo VOTE: formula is unsatisfiable
