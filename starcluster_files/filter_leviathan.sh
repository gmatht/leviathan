rm $1.LVeasy$2.txt
rm $1.LVhard$2.txt
while read f
do
	if LV_SAT_ERRCODE=0 JOB_NO=1/1@0 timeout $2 ../bin/checker -l "$f"
	then 
		echo LV: $i "$f"
		echo $f >> $1.LVeasy$2.txt
	else
		echo $f >> $1.LVhard$2.txt
		#echo $i >> non_trivial_n_$NAME.txt
	fi
done < $1
