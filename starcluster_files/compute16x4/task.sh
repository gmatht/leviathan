../ssh.sh "
cd leviathan/starcluster_files
echo B_SETS=S3 B_DEPTHS='`seq 5 100 | tr '\n' ' '`' nohup ./benchmark.sh
B_SETS=S3 B_DEPTHS='`seq 5 100`' ./benchmark.sh
" | tee sc_bench.txt
../backup.sh

../ssh.sh "
cd leviathan/starcluster_files
B_SETS='H S1 S2' nohup ./benchmark.sh
" | tee sc_bench2.txt
../backup.sh
