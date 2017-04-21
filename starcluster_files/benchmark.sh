for f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
 do echo --- $f
 i=$((i+1))
 time timeout 1 bash parallel.sh "`cat $f`" P$i
 bash makelog.sh "`cat $f`" P$i "$f"
 done 2>&1 | tee simplebench.txt
