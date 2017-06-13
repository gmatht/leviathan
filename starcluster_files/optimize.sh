#DEPTH=17; gcc parse_clock.cpp; for f in store/compute_serial/U[123]_*; do echo $f; zcat $f | PC_FLOAT=y ./a.out $DEPTH:719 | average.sh; zcat $f | PC_FLOAT=y ./a.out $DEPTH:720 | average.sh 2> /dev/null; zcat $f | PC_FLOAT=y ./a.out $DEPTH:718 | average.sh 2> /dev/null; done
gcc parse_clock.cpp -O2 -o parse_clock
DEPTHS="`seq 1 25`"
zcat $(ls store/compute_serial/U?_*gz) > /tmp/zcat
(
for DEPTH in $DEPTHS; do echo -ne "\t$DEPTH"; done; echo
for JOB in `seq 1 720`; do
echo  -n "$JOB:" 
for DEPTH in $DEPTHS; do
	echo -ne '\t'
	#printf "%7s" `< /tmp/zcat PC_FLOAT=y ./parse_clock $DEPTH:$JOB | average.sh 2> /dev/null | cut -f6 | tr -d '\n' | sed s/.$//`
	#printf "%7s" `< /tmp/zcat PC_FLOAT=y ./parse_clock $DEPTH:$JOB | average.sh 2> /dev/null | cut -f6 | tr -d '\n' | sed s/.$//`
	echo -n `< /tmp/zcat PC_SUM=y ./parse_clock $DEPTH:$JOB`
done
echo
done

for DEPTH in $DEPTHS; do echo -ne "\t$DEPTH"; done; echo
) | tee optimize.txt

echo 2 | perl -pe 'sub even {$j=1;$k=$i_[0];while(($k%2==0) and ($k>0)){$k=$k/2;$j++} return $j } ;s/([0-9]*)/$1,even($1)/e' < optimize.txt > optimize_witheveness.sh
#paste <(for f in store/compute_serial/U[123]_*.gz; do  zcat $f | PC_FLOAT=y PC_RR=Y ./parse_clock 17:4 | sed s/...$// | tr '\n' '\t'; echo; done) <(yes '|' |head -n99) <(grep 'user$' < store/summary/compute2x2_rr_summary.txt) | less | grep ^[0-9] | sed s/user/\\t/g | tee store/summary/compute2x2_rr_summary_vs_estimate.txt
#paste <(for f in store/compute_serial/U[123]_*.gz; do  echo `zcat $f | PC_FLOAT=y PC_RR=Y ./parse_clock 17:4`; done) <(grep 'user$' < store/summary/compute2x2_rr_summary.txt) | less > store/summary/compute2x2_rr_summary_vs_estimate.txt
