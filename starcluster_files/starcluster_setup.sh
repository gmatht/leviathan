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
starcluster lc small > starcluster_lc.txt
grep -o "ec2-[^-]*-[^-]*-[^-]*-[^.-]*" starcluster_lc.txt | sed s/ec..// | tr - . | head -n1 > ip.txt
IP=`cat ip.txt`
set | grep IP
exit
#gzip -9 < ../bin/checker > checker.gz
#starcluster put small checker.gz .
mkdir -p tar/usr/bin; mkdir -p tar/root/.ssh/
cp ~/.gitconfig tar/root/
cp ../bin/checker tar/usr/bin

(cd tar;
 chmod 700 root root/.ssh; chmod 600 root/.ssh/*
 tar -zcf ../tar.gz . --owner=0 --group=0)
starcluster put small tar.gz .
#(cd tar; tar -zc .) | bash ssh.sh "cd / && tee tar.gz | 
_ssh "cd /; tar -zxf ~/tar.gz; chown root /root/.ssh /root/.ssh/* /root; chgrp root /root /root/.ssh /root/.ssh/*; chmod 600 ~/.ssh/*; chmod 700 ~/.ssh /root"
#starcluster put small config ~/.ssh/config
_ssh 'ssh-add ~/.ssh/id_rsa
set -x; NODES="$(grep -o node... /etc/hosts)"; set | grep ^NODES | tee nodes.sh
for n in $NODES; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 ; done
for n in $NODES; do scp /usr/bin/checker $n:/usr/bin/checker ; done
for n in $(grep -o node... /etc/hosts); do echo ssh $n; done > ssh.txt; echo "sh -c" >> ssh.txt; cat ssh.txt; wc -l ssh.txt
#while read SSH ; do < checker.gz $SSH "gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; scp echo XXX; done < ssh.txt
sudo apt-get install -y git-core	
git clone https://github.com/gmatht/leviathan.git
