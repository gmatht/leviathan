[ -e ../store/$(basename $PWD)/summary.txt ] ||
for f in ../store/$(basename $PWD)/*gz; do zcat $f | head -n 30 | grep ^10 | tail -n1 ; done > ../store/$(basename $PWD)/summary.txt

for i in {0..9}
do
	echo $i: $(grep "@$i " ../store/$(basename $PWD)/summary.txt | wc -l)
done

echo ../store/$(basename $PWD)/summary.txt

