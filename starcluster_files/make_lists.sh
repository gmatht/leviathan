#You can update the lists by running this after making benchmark.txt
< benchmark.txt sed s/----/\#/ | tr '\n#' ' \n' > benchmark_oneline.txt
sed 's/.* user //' < benchmark_oneline.txt | grep is.sat | sed 's/ .* tests./\t/' | grep .. | sort -n > tests/lists/S
sed 's/.* user //' < benchmark_oneline.txt | grep is.unsat | sed 's/ .* tests./\t/' | grep .. | sort -n > tests/lists/U
cd tests/lists/
for t in S U
do
	< $t grep '^0[.][1-9]' > "$t"0
	< $t grep '^[1-9][.]' > "$t"1
	< $t grep '^[1-9].[.]' > "$t"2
	< $t grep '^[1-9]..[.]' > "$t"3
done
	
#grep -v satisfiable  benchmark_oneline.txt | grep .. | sed 's/^ .*tests./?\t/' | shuf > tests/lists/H
