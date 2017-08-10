OPWD=$PWD
bash reports/hybrid.sh > store/hybrid.txt
cd store
for d in *; do (cd $d && echo -- $d -- && zgrep 'VOTE.*is.sat' *T1.gz | grep -o "[0-9]*@[0-9]*" | sort | uniq -c); done | tee summary/one_second.txt
< summary/summary_1@1.tsv grep S[0-3] | cut -f2- | sort -n > S.txt
zgrep -H  'VOTE.*is.sat' *T1.gz  > S_.txt
perl -pe '$i=$i+1;print "$i\t"'< S.txt > Si.txt
egrep '(@64 )' summary/one_second.txt | sort -n | sed s/@64// > S_parallel.txt
gnuplot $OPWD/SatisfiableFormulas.gnuplot
