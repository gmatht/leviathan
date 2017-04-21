for L in U S H
do
cat ../tests/lists/$L | cut -f2 | while read f
#f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
 do echo --- $f
 i=$((i+1))
 time timeout 1000 bash parallel.sh "`cat ../tests/$f`" $L$i
 bash makelog.sh "`cat $f`" $L$i "$f"
 done 
done 2>&1 | tee simplebench.txt

