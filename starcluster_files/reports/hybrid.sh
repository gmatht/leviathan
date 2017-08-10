(cd store/compute16x4/
zgrep 'IsSat' *32@64.T1.gz 
) > 32@64.T1.solved.txt

for i in {33..128}
do
echo -en "$i\t"
(
cat 32@64.T1.solved.txt
< store/summary/summary_1@1.tsv perl -e 'while(<>){@a=split /\t/;if ($a[1]<'$((i-32))'){print "$a[2]\n"}}'
) | grep '^.......' -o | sort -u | wc -l
done

exit

(
< store/summary/summary_1@1.tsv perl -e 'while(<>){@a=split /\t/;if ($a[1]<32){print "$a[2]\n"}}'
cd store/compute16x4/
zgrep 'IsSat' *32@18.T1.gz
) | grep '^.......' -o | sort -u | wc -l

(
< store/summary/summary_1@1.tsv perl -e 'while(<>){@a=split /\t/;if ($a[1]<128){print "$a[2]\n"}}'
cd store/compute16x4/
#zgrep 'IsSat' *32@64.T1.gz
) | grep '^.......' -o | sort -u | wc -l
