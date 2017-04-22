mkdir -p ~/store
for L in U S H
do
cat ../tests/lists/$L | while read t f
#f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
 do echo --- $t $f
 i=$((i+1))
 time -p timeout 1000 bash parallel.sh "`cat ../tests/$f`" $L$i
 bash makelog.sh "`cat ../tests/$f`" $L$i "$f"
 done 
done 2>&1 | tee simplebench.txt

