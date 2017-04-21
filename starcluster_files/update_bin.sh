_ssh() {
  starcluster sshmaster small "$@"
}
(cd ..; make -j4)
gzip -9 < ../bin/checker > checker.gz
starcluster put small checker.gz .
_ssh 'ssh-add ~/.ssh/id_rsa
while read SSH ; do < checker.gz $SSH "gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; echo XXX; done < ssh.txt
'
#nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE)); for j in `seq 1 $nCPU $nJOB`; do read SSH; echo $j $SSH; $SSH "JOB_NO=$j/$nJOB@9 checker -l 'X p'"  ;done < ssh.txt 
