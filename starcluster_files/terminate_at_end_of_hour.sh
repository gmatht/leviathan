CLUSTER=$(basename $PWD)
START=$(grep "^$CLUSTER " starcluster_lc.txt -A3 | grep Launch.time | perl -e 'while(<>){if (/:(..):/){print $1}}')

while true
do
	NOW=`date +%-M`
	DIFF=$(((START+60-NOW)%60))
	WASTE=10
	echo "  $START-$NOW=$DIFF  <$WASTE"
	if [ "$DIFF" -lt $WASTE ]
	then
		echo "Should terminate now, less than $WASTE minutes remaining till another hour is charged"
		starcluster terminate -c -f "$CLUSTER"
		exit
	fi
	sleep 60
	#grep $(basename `pwd`) starcluster_lc.txt -A3 | grep Launch.time | perl -e 'while(<>){if (/:(..):/){print $1}}'
done
