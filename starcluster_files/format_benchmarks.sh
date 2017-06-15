i=15; ((echo Name; seq $i 25) | tr '\n' '\t'; cut -f2,$((2+i))- store/compute_serial.noclock/overhead.txt) | perl ./tsv2tex.pl
