#!/bin/bash
FORMULA=$1
NAME=$2
. ./nodes.sh
echo > pids
nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE))
rm ~/out/result.$NAME.txt 2> /dev/null || true 2> /dev/null
j=1; ( for n in $NODES master
do
        < /dev/null ssh -q -t -t $n "ulimit -Sv 850000; for i in `seq $j $((j+nCPU-1))`; do JOB_NO=\$i/$nJOB@9 checker -l '$FORMULA' > log.$NAME.\$i.txt; done; sleep 1" &
        echo $! >> pids
        j=$((j+$nCPU))
done ; wait ) 2>&1 | while read L
do
case "$L" in
IsSat*)
	echo VOTE: formula is satisfiable $L
	break
	;;
Unsat*)
	unsat=$((unsat+1))
	if [ $unsat -ge $nJOB ]
	then
		echo VOTE: formula is unsatisfiable $L
		break
	fi`
	;;
*)
	echo UNKNOWN WARNING: $:

esac
done
