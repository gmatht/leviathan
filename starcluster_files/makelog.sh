#!/bin/bash
FORMULA="$1"
NAME=$2
FILE=$3
mkdir -p ~/store
. ~/nodes.sh
j=1; for n in $NODES localhost
do
	#echo $j $((j+nCPU-1))
        < /dev/null ssh -q $n "for i in `seq $j $((j+nCPU-1))| tr '\n' ' '`; do echo; echo @.$NAME.\$i; cat ~/out/time.$NAME.\$i.txt ~/out/log.$NAME.\$i.txt ; echo rm ~/out/time.$NAME.\$i.txt ~/out/log.$NAME.\$i.txt ; done | gzip -9" > tmp.$NAME.$j.gz &
        j=$((j+$nCPU))
done ; wait 
((
echo NAME=$NAME
echo FILE=$FILE
echo "FORMULA=\"$FORMULA\""
echo

cat ~/out/result.$NAME.txt) | gzip -9
j=1; for n in $NODES localhost
do
	cat tmp.$NAME.$j.gz
	rm tmp.$NAME.$j.gz
        j=$((j+$nCPU))
done ; wait 
) > ~/store/$NAME.gz
echo `zgrep -o ^.*user < ~/store/$NAME.gz`
echo $NAME.gz >> ~/store/ready.txt
