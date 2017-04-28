#mkdir -p ~/store/bak; (cd ~/store; mv * bak || true); (cd ~/store/; mkdir -p sav; for f in `cat bak/ready.txt`; do mv bak/$f sav/$f; done)
#(cd ~/store/; mkdir -p sav; for f in `cat bak/ready.txt`; do mv bak/$f sav/$f; done)

SETS="${B_SETS:-U0 U1 U2 U3 H S0 S1 S2 S3}"
DEPTHS="${B_DEPTHS:-16 18 19 24 40 48 56 64 72 80}"

ls ~/store/summary*txt && (
	OLD=~/store/old.`date -Ins`
	mkdir -p "$OLD"
	mv ~/store/*txt $OLD/
)

git diff .. > ~/store/diff.txt
mkdir -p ~/out
. ~/nodes.sh
for n in $NODES localhost; do ssh $n 'killall    checker'; done
for n in $NODES localhost; do ssh $n 'killall -9 checker'; done

if [ -z "$nCPU"  ]
then nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ~/ssh.txt`
fi
nJOB=$((nCPU*nNODE))

export nCPU
export nNODE
export nJOB

renice -10 $$

mkdir -p ~/store
(
for L in $SETS 
do
	cat ../tests/lists/$L | while read t f
	do
		#f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
		i=$((i+1))
	 	#for DEPTH in 2 4 8 16 17 18 32 64 128 256 512
	 	#for DEPTH in 4 8 16 17 18 19 32 64 128 
	 	for DEPTH in $DEPTHS
#4 8 16 17 18 19 32 64 128 
	 	do
	 		NAME="$L"_`printf %3d  $i | tr \  0`_$nJOB"@"$DEPTH
			if [ -e ~/store/sav/$NAME.gz ]
			then continue
			fi
			echo --- $t $f 
			export DEPTH
			time -p timeout 1 bash parallel.sh "`cat ../tests/$f`" $NAME
	        	bash makelog.sh "`cat ../tests/$f`" $NAME "$f"
			date -Ins
 		done
	done 
done
exit
-----------------------------------
L=H
cat ../tests/lists/H | while read t f
do
	i=$((i+1))
	echo -e "$i\t$f"
done | shuf | while read i f
do echo --- $i $f
	NAME="$L"_`printf %3d  $i | tr \  0`
	for DEPTH in 2 4 8 16 32 64 128 256 512
	do
		export DEPTH
		time -p timeout 1 bash parallel.sh "`cat ../tests/$f`" $NAME
	done
	bash makelog.sh "`cat ../tests/$f`" $NAME "$f"
done
) 2>&1 | tee ~/store/summary.txt
