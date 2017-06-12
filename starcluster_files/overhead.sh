mkdir -p ~/store
DEPTHS="`seq 1 25`"
for L in U0 U1 U2 U3
do
	cat ../tests/lists/$L | while read t f
	do
		i=$((i+1))
		NAME="$L"_`printf %3d  $i | tr \  0`
		echo -ne "$t\t$NAME"
		for DEPTH in $DEPTHS
		do
			JOB_NO=0/1@$DEPTH checker ../tests/$f >> ~/store/overhead_$L.$DEPTH.full.txt 2> time.txt
			echo -ne "\t`grep -o 'SEC=[0-9.]*' < time.txt | sed s/SEC=// | sed s/.$//`"
		done
		echo
	done
done | tee ~/store/overhead.txt

#echo -ne "DEPTH\t"; echo 1 | average.sh > /dev/null ; for c in `seq 3 22`; do echo -ne "$((c-2))\t"; cut -f $c ../store/compute_serial/overhead.txt | average.sh 2> /dev/null ; done

