mkdir -p ~/store
DEPTHS="`seq 1 20`"
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
