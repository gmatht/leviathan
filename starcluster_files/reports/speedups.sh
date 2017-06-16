#echo -e "vCPU\tName\tsum\tn\tmean\tmedian\tmin\tmax"; for CPU in 8 144; do for L in U0 U1 U2 U3 S0 S1 S2 S3; do echo -ne "$CPU\t$L\t"; grep "$L"_ summary_$CPU@18.tsv | perl -e 'while(<>){if (/(\S*)\s(\S*)/) {print $1/$2."\n"}}'| average.sh 2> /dev/null; done; done
DEPTH=$1
cd store/summary
(
echo -e "jobs\tset\tsum\tn\tmean\tmedian\tmin\tmax"
	#for L in U0 U1 U2 U3 S0 S1 S2 S3; do
for TYPE in U S; do
for L_ in 0 1 2 3; do
for i in 2 8 32 88; do
#for F in summary_*[2-9]@*tsv; do
	L=$TYPE$L_
	F=summary_"$i"@"$DEPTH".tsv
	echo -ne "$F\t$L\t" | sed 's/summary_//' | sed 's/.tsv//'
	paste <(grep "$L"_ summary_1@1.tsv | cut -f2)  <(grep "$L"_ $F | cut -f2)  | perl -e 'while(<>){if (/(\S*)\s(\S*)/) {print $1/$2."\n"}}'| average.sh 2> /dev/null
done
done
done  
) | sed s/@$DEPTH// | cut -f1,2,5-

exit 
| sed 's/^2/02/
s/^8/08/
s/^088/88/' | sort -n | sort  -t'	' -k2 | sed s/@20//
