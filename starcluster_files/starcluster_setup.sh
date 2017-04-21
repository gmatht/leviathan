set -x

# SETUP CLIENT
starcluster -h 2> /dev/null > /dev/null || (
	sudo apt-get install build-essential libssl-dev libffi-dev python-dev
	sudo easy_install starcluster
)
[ -e remote.pub ] || (
	ssh-keygen -t remote
	ssh-keygen -y -f remote > remote.pub
)

# SETUP SERVER
starcluster put small remote .ssh/id_rsa
starcluster put small remote.pub .ssh/id_rsa.pub
starcluster sshmaster small "echo $(cat remote.pub) >> ~/.ssh/authorized_keys"
starcluster sshmaster small "(echo $(cat remote.pub); echo $(cat ~/.ssh/id_rsa.pub)) >> ~/.ssh/authorized_keys"
starcluster put small /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0
i=1
while starcluster sshnode small node`printf "%3d" $i | sed s/\ /0/g` "echo $(cat remote.pub) >> ~/.ssh/authorized_keys"
do
	echo adding ssh to $i
	i=$((i+1))
done
starcluster sshmaster small "for n in `grep -o nodes??? /etc/hosts`; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0; done
apt-get install mosh -y"
starcluster sshmaster small "cat /etc/hosts" > hosts

_ssh() {
  starcluster sshmaster small "$@"
}
#gzip -9 < ../bin/checker > checker.gz
#starcluster put small checker.gz .
mkdir -p tar/usr/bin; mkdir -p tar/root/.ssh/
#(cd tar; tar -zc .) | _ssh "cd / && tar -zx; chown root /root/.ssh/ /root/.ssh/* /root; chmod 600 ~/.ssh/*; chmod 700 ~/.ssh /root:"
#starcluster put small config ~/.ssh/config
_ssh 'ssh-add ~/.ssh/id_rsa
set -x; NODES="$(grep -o node... /etc/hosts)";
echo NODES="'$NODES'" > ~/nodes.sh
for n in $NODES; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 ; done
for n in $(grep -o node... /etc/hosts); do echo ssh $n; done > ssh.txt; echo "sh -c" >> ssh.txt; cat ssh.txt; wc -l ssh.txt
while read SSH ; do < checker.gz $SSH "gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; scp echo XXX; done < ssh.txt
sudo apt-get install -y git-core	
git clone https://github.com/gmatht/leviathan.git
'
#nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE)); for j in `seq 1 $nCPU $nJOB`; do read SSH; echo $j $SSH; $SSH "JOB_NO=$j/$nJOB@9 checker -l 'X p'"  ;done < ssh.txt 
