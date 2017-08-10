i=1
NAME=`echo "$*"|tr ' ' -`
NAME=REPRODUCE

rm non_trivial_polsat_*_$NAME.txt
rm $2.

while read f 
do
        OUT=out$NAME.$i.txt
        timeout $1 ./polsat "$f" > $OUT

        read issat < $OUT
        echo -- $i $issat $f    
        cat $OUT

        if [ "$issat" = "sat" -o "$issat" = "unsat" ]
        then
                rm $OUT
        else
                #echo $i >> non_trivial_polsat_n_$NAME.txt
                echo $f >> $2. #>> non_trivial_polsat_f_$NAME.txt
        fi
        i=$((i+1))
done  < $2 > $2.log

