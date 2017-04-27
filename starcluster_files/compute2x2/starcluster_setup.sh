#!/bin/bash
set -x
set -e
CLUSTER=$(basename $PWD)
echo $CLUSTER 
(cd ../..
make -j4)
# SETUP CLIENT
starcluster -h 2> /dev/null > /dev/null || (
	sudo apt-get install build-essential libssl-dev libffi-dev python-dev
	sudo easy_install starcluster

	cd /usr/local/lib/python2.7/dist-packages/StarCluster-0.95.6-py2.7.egg/starcluster && (
		mv static.pyc static.pyc.bak
		cp static.py  static.py.bak
		patch -p0 < ../static.py.patch
  		pycompile static.py 
	)
)
[ -e ../tar/root/.ssh/id_rsa.pub ] || (
	cd ../tar/root/.ssh/ &&
	ssh-keygen -t ./id_rsa && 
	ssh-keygen -y -f id_rsa > id_rsa.pub
)

date > starttime.txt
starcluster start -c $(echo $CLUSTER | sed s/[.].*// ) $CLUSTER || true
starcluster lc $CLUSTER > starcluster_lc.txt
grep -o "ec2-[^-]*-[^-]*-[^-]*-[^.-]*" starcluster_lc.txt | sed s/ec..// | tr - . | head -n1 > ip.txt
IP=`cat ip.txt`
set | grep IP

_ssh() {
  ssh root@$IP "$@"
}

# SETUP SERVER
#"echo $(cat remote.pub) >> ~/.ssh/authorized_keys"
starcluster sshmaster $CLUSTER "(echo $(cat ../tar/root/.ssh/id_rsa.pub); echo $(cat ~/.ssh/id_rsa.pub)) >> ~/.ssh/authorized_keys"
(cd ../tar && 
	mkdir -p usr/bin; mkdir -p root/.ssh/; mkdir -p usr/lib/x86_64-linux-gnu/
	cp ~/.gitconfig root/
	cp ../../bin/checker usr/bin
	cp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 usr/lib/x86_64-linux-gnu/
	chmod 700 . root root/.ssh usr usr/bin usr/lib usr/lib/x86_64-linux-gnu
	chmod 600 root/.ssh/*
	tar -zcf ../tar.gz . --owner=0 --group=0
	rsync -a --progress ../tar.gz root@$IP:tar.gz
	#scp ../tar.gz root@$IP: 
)
i=1
_ssh "cat /etc/hosts" > hosts
for n in `grep -o node... hosts`; do starcluster sshnode $CLUSTER $n "echo $(cat ../tar/root/.ssh/id_rsa.pub) >> ~/.ssh/authorized_keys" & done
wait
_ssh '
mkdir -p store
mkdir -p out
cd /; tar -zxf ~/tar.gz; chown root /root/.ssh /root/.ssh/* /root; chgrp root /root /root/.ssh /root/.ssh/*; chmod 600 ~/.ssh/*; chmod 700 ~/.ssh /root
'
_ssh '
cd
#for n in `grep -o node... /etc/hosts`; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0; done
apt-get install mosh -y
ssh-add ~/.ssh/id_rsa
set -x; NODES="$(grep -o node... /etc/hosts)"; set | grep ^NODES | tee nodes.sh
for n in $NODES; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 ; done
for n in $NODES; do scp /usr/bin/checker $n:/usr/bin/checker ; done
for n in $NODES; do ssh $n mkdir ~/out; done
for n in $(grep -o node... /etc/hosts); do echo ssh $n; done > ssh.txt; echo "sh -c" >> ssh.txt; cat ssh.txt; wc -l ssh.txt
#while read SSH ; do < checker.gz $SSH "gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; scp echo XXX; done < ssh.txt
apt-get install -y git-core	
if [ -e leviathan ] 
then
 cd leviathan
 git pull
else
 git clone https://github.com/gmatht/leviathan.git
fi
'

(ssh root@`cat ip.txt` "cd store && sleep 10 && tail -f ready.txt | while read f; do cat $f; done" > ../store/$CLUSTER.txt) &

#if [ "$CLUSTER" = compute_serial ]
#then SERIAL=Y; CLOCK_DEPTH=20
#else SERIAL=N; CLOCK_DEPTH=20
#fi

#export SERIAL=$SERIAL
#export CLOCK_DEPTH=$CLOCK_DEPTH

_ssh "
cd leviathan/starcluster_files
$* bash benchmark.sh
" | tee sc_bench.txt

if bash ../backup.sh
then

echo PREPARING TO DESTROY CLUSTER
for i in `seq 99 -1 0`; do echo -ne "$i \r"; sleep 1; done
echo ABOUT TO DESTROY CLUSTER
sleep 5
echo REALLY DESTROYING CLUSTER

#starcluster -c -f terminate "$CLUSTER"
starcluster -cf terminate "$CLUSTER"

else
	echo BACKUP FAILED! manually terminate cluster

fi
#nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE)); for j in `seq 1 $nCPU $nJOB`; do read SSH; echo $j $SSH; $SSH "JOB_NO=$j/$nJOB@9 checker -l 'X p'"  ;done < ssh.txt 
