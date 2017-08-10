i=1
NAME=`echo "$*"|tr ' ' -`

rm non_trivial_polsat_*_$NAME.txt
mkdir -p out.tmp

ocaml ./make_random_ltl_formulas.ml $3 $2 $i 2147483647 | while read f
do
	OUT=out.tmp/out$NAME.$i.txt
	timeout $1 ./polsat "$f" > $OUT

	read issat < $OUT
	echo -- $i $issat $f	
	cat $OUT

	if [ $issat = "sat" -o $issat = "unsat" ]
	then
		rm $OUT
	else
		echo $i >> non_trivial_polsat_n_$NAME.txt
		echo $f >> non_trivial_polsat_f_$NAME.txt
	fi
	i=$((i+1))
done 2>&1 | tee > filter_formulas_$NAME.log
