if [ ! -z "$@" ]
then 
	../ssh.sh "cd leviathan; git commit -m '$1' ."
fi
git pull root@`cat ip.txt`:leviathan
