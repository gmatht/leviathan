TL=`git rev-parse --show-toplevel`
head -n$2 $TL/tests/lists/$1 | tail -n1 | while read a b
do
	echo $TL/tests/$b
done
