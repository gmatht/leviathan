=$1
make && (for i in `seq 0 $N`; do JOB_NO=$i/$N@9 timeout 30 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a' & done; wait)  2>&1 | tee out$N.txt; grep user out.txt ; grep Total out$N.txt
