_ssh() {
  ../ssh.sh "$@"
}
(cd ../..;
vim lib/src/solver.cpp
 make -j4)
gzip -9 < ../../bin/checker > ../checker.gz
rsync -a ../checker.gz root@`cat ip.txt`:
_ssh 'ssh-add ~/.ssh/id_rsa
while read SSH ; do < checker.gz $SSH "rm /usr/bin/checker; gunzip > /usr/bin/checker; chmod +x /usr/bin/checker; mkdir -p ~/out"; echo XXX; done < ssh.txt
'
#nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nNODE=`wc -l < ssh.txt`; nJOB=$((nCPU*nNODE)); for j in `seq 1 $nCPU $nJOB`; do read SSH; echo $j $SSH; $SSH "JOB_NO=$j/$nJOB@9 checker -l 'X p'"  ;done < ssh.txt 
