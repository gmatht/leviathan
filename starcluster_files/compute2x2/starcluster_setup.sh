set -x
CLUSTER=$(dirname $PWD)
# SETUP CLIENT
starcluster -h 2> /dev/null > /dev/null || (
	sudo apt-get install build-essential libssl-dev libffi-dev python-dev
	sudo easy_install starcluster
)
[ -e ../tar/root/.ssh/id_rsa.pub ] || (
	cd ../tar/root/.ssh/ &&
	ssh-keygen -t ./id_rsa && 
	ssh-keygen -y -f id_rsa > id_rsa.pub
)

date > starttime.txt
starcluster start $CLUSTER 
starcluster lc $CLUSTER > starcluster_lc.txt
grep -o "ec2-[^-]*-[^-]*-[^-]*-[^.-]*" starcluster_lc.txt | sed s/ec..// | tr - . | head -n1 > ip.txt
IP=`cat ip.txt`
set | grep IP

_ssh() {
  ssh root@$IP "$@"
}

# SETUP SERVER
_ssh "echo $(cat remote.pub) >> ~/.ssh/authorized_keys"
_ssh "(echo $(cat remote.pub); echo $(cat ~/.ssh/id_rsa.pub)) >> ~/.ssh/authorized_keys"
(cd ../tar && 
	mkdir -p usr/bin; mkdir -p root/.ssh/; mkdir -p usr/lib/x86_64-linux-gnu/
	cp ~/.gitconfig root/
	cp ../bin/checker usr/bin
	cp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 usr/lib/x86_64-linux-gnu/
	chmod 700 root root/.ssh; chmod 600 root/.ssh/*
	tar -zcf ../tar.gz . --owner=0 --group=0
	scp ../tar.gz root@$IP: 
)
i=1
_ssh "cat /etc/hosts" > hosts
for n in `grep -o nodes??? hosts`
do
starcluster sshnode $CLUSTER $n "echo $(cat remote.pub) >> ~/.ssh/authorized_keys" &
done
wait
_ssh '
cd /; tar -zxf ~/tar.gz; chown root /root/.ssh /root/.ssh/* /root; chgrp root /root /root/.ssh /root/.ssh/*; chmod 600 ~/.ssh/*; chmod 700 ~/.ssh /root
for n in `grep -o nodes??? /etc/hosts`; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0; done
apt-get install mosh -y
ssh-add ~/.ssh/id_rsa
set -x; NODES="$(grep -o node... /etc/hosts)"; set | grep ^NODES | tee nodes.sh
for n in $NODES; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 ; done
for n in $NODES; do scp /usr/bin/checker $n:/usr/bin/checker ; done
for n in $(grep -o node... /etc/hosts); do echo ssh $n; done > ssh.txt; echo "sh -c" >> ssh.txt; cat ssh.txt; wc -l ssh.txt
#while read SSH ; do < checker.gz $SSH "gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; scp echo XXX; done < ssh.txt
sudo apt-get install -y git-core	
git clone https://github.com/gmatht/leviathan.git
'

_ssh '
cd leviathan/starcluster_files
bash benchmark.sh
' | tee sc_bench.txt

#nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE)); for j in `seq 1 $nCPU $nJOB`; do read SSH; echo $j $SSH; $SSH "JOB_NO=$j/$nJOB@9 checker -l 'X p'"  ;done < ssh.txt 
