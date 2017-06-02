set -x
TIMEOUT=2
#[ -e make_random_ctls_formulas ] || (
#	ocamlopt ./make_random_ctls_formulas.ml -o ./make_random_ctls_formulas
#)
#apt-get install ocaml-interp

../ssh.sh "mkdir -p /root/out"

cat  ~/benchmarks/non_triv_50_2.txt | (
while read i
do
	#f=`./make_random_ctls_formulas 50 2 $i $i`
	f=`ocaml ~/benchmarks/make_random_ctls_formulas.ml 50 2 $i $i`
	echo -n -e "$i" >> log_filtered.txt

	FNAME=../store/$(basename $PWD)/$i.gz
	if ! [ -e $FNAME ]
	then 
		< /dev/null ../ssh.sh "cd benchmarks; bash test_multi_depth1.sh $i '$f' | gzip" > $FNAME.tmp &&
		mv $FNAME.tmp $FNAME
		echo ___ $FNAME ___
		zcat $FNAME | head
	fi
done
)
	
#grep Unsat log_fuzz.txt | column -t -s"`echo -e '\t'`"	
#for t in . null @4 @9; do echo -ne "$t\t"; grep $t log_fuzz.txt | wc -l; done
