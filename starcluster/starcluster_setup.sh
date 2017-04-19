set -x

# SETUP CLIENT
command -v starcluster || (
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
starcluster put small /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0
i=1
while starcluster sshnode small node`printf "%3d" $i | sed s/\ /0/g` "echo $(cat remote.pub) >> ~/.ssh/authorized_keys"
do
	echo adding ssh to $i
	i=$((i+1))
done
starcluster sshmaster small "for n in `grep -o nodes??? /etc/hosts`; do scp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $n:/usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0; done"
