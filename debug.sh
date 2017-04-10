N=5
#make && (for k in ` seq 1 $3` ; do for j in `seq 1 $2` ; do for i in `seq 1 $N`; do echo JOB_NO=$i/$N@10 JOB2_NO=$j/$2@20 JOB3_NO=$k/$3@30 timeout 30 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' ; done; done; done; wait) 
#exit
#make && (for k in ` seq 1 $3` ; do for j in `seq 1 $2` ; do for i in `seq 1 $N`; do JOB_NO=$i/$N@10 JOB2_NO=$j/$2@20 JOB3_NO=$k/$3@30 timeout 1 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' & done; done; done; wait)  2>&1 | tee out.txt; grep user out.txt ; grep Total out$N.txt
#make && (for k in ` seq 1 $3` ; do for j in `seq 1 $2` ; do for i in `seq 1 $N`; do JOB_NO=$i/$N@9 JOB2_NO=$j/$2@10 JOB3_NO=$k/$3@11 timeout 1 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' & done; done; done; wait)  2>&1 | tee out.txt; grep user out.txt ; grep Total out$N.txt

set -e
make

#JOB_NO=3/1@11 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' > debug.out 
#< debug.out grep Total.frames | sed s/.*:.// | average.sh
echo | average.sh > /dev/null
for D in `seq 1 25`
do
JOB_NO=0/$N@$D JOB2_NO=1/1@1 JOB3_NO=1/1@1 time -p bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' > debug__$D.out 2> debug__$D.err
echo "`< debug__$D.out grep Total.frames | sed s/.*:.// | average.sh 2> /dev/null`" `cat debug__$D.err`
done

echo ----------------------------------
JOB_NO=1/1@1 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' > debug.out 
< debug.out grep Total.frames | sed s/.*:.// | average.sh

for D in `seq 3 12`
do
for i in `seq 1 $N`
do JOB_NO=$i/$N@$D JOB2_NO=1/1@1 JOB3_NO=1/1@1 bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' &
done > debug$D.out 2> debug$D.err 
wait
< debug$D.out grep Total.frames | sed s/.*:.// | average.sh
done
