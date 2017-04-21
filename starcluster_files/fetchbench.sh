ssh root@`cat ip.txt` "cd store && tail -f ready.txt | while read f; do cat $f; done" > ../store/$(dirname $PWD).txt
