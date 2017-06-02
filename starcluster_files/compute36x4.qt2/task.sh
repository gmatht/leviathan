./backup w &
../ssh.sh '. ~/nodes.sh
cd ~/benchmarks
nNODE=$(echo $NODES | wc -w)
nNODE=$((nNODE+1))
nCPU=`cat /proc/cpuinfo | grep processor | wc -l`; nCPU=$((nCPU/2)); nJOB=$((nCPU*nNODE))
export nCPU
export nJOB
export nNODE
export NODES

for NODE in localhost $NODES
do ssh $NODE "apt-get update; apt-get install -y default-jre-headless" &
done
for NODE in $NODES; do rsync --progress time quicktab.jar $NODE:benchmarks/; done &
wait
#apt-get install ocaml-nox ocaml-interp -y

ln -s ~/store/log_filtered.txt  .
'

bash ./test_multi_depth_all.sh
echo finished. Sleeping
sleep 120
bash ../backup.sh
bash ../terminate_at_end_of_hour.sh
