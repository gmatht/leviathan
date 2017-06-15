#mkdir -p ~/store/bak; (cd ~/store; mv * bak || true); (cd ~/store/; mkdir -p sav; for f in `cat bak/ready.txt`; do mv bak/$f sav/$f; done)
#(cd ~/store/; mkdir -p sav; for f in `cat bak/ready.txt`; do mv bak/$f sav/$f; done)

SETS="${B_SETS:-U0 U1 U2 U3 H S0 S1 S2 S3}"
#DEPTHS="${B_DEPTHS:-4 6 8 12 16 18 19 20 21 22 24 32 48 64 96 128}"
DEPTHS="${B_DEPTHS:-20 64}"
TIMEOUT="${B_TIMEOUT:-1}"

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
then 
	nCPU=`cat /proc/cpuinfo | grep processor | wc -l`
	nCPU=$((nCPU/2)) #AWS tends to use hyperthreading so physical CPUs = vCPUs/2
fi

nNODE=$(echo $NODES | wc -w)
set | grep nNODE
nNODE=$((nNODE+1)) #include master/localhost
nJOB=$((nCPU*nNODE))
export nCPU
export nNODE
export nJOB
export NODES

renice -10 $$

mkdir -p ~/store
(
echo "nCPU=$nCPU nNODE=$nNODE nJOB=$nJOB B_TIMEOUT=$TIMEOUT SOLVER=$SOLVER B_DEPTHS='$DEPTHS' B_SETS='$SETS'"
cat /proc/cpuinfo  | grep model.name | tail -n1
echo `lsb_release -d` `arch`

echo SOLVER=$SOLVER
for L in $SETS 
do
	echo SET $L
	cat ../tests/lists/$L | while read t f
	do
		#f in `find ~/leviathan/tests/rozier/ | grep pltl\$`
		i=$((i+1))
		if [ -z "$f" ]; then f=$t; t="?"; fi
	 	#for DEPTH in 2 4 8 16 17 18 32 64 128 256 512
	 	#for DEPTH in 4 8 16 17 18 19 32 64 128 
	 	for DEPTH in $DEPTHS
#4 8 16 17 18 19 32 64 128 
	 	do
	 		NAME="$SOLVER$L"_`printf %4d  $i | tr \  0`_$nJOB"@"$DEPTH.T$TIMEOUT
			if [ -e ~/store/sav/$NAME.gz ]
			then continue
			fi
			echo --- $t $f 
			export DEPTH
			#echo "[[$t]]"
			#if echo "$t" | grep Fmla > /dev/null
			if [ "$L" = R ]
			then fmla="$f"; t="?"
			else fmla="`cat ../tests/$f`"
			#else fmla="NA"
			fi

			time -p timeout $TIMEOUT bash "$SOLVER"parallel.sh "$fmla" $NAME
			bash makelog.sh "$fmla" $NAME "$f"

			date -Ins
 		done
	done 
done
) 2>&1 | tee ~/store/summary.txt

