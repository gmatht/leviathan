#You can update the lists by running this after making benchmark.txt
< benchmark.txt sed s/----/\#/ | tr '\n#' ' \n' > benchmark_oneline.txt
sed 's/.* user //' < benchmark_oneline.txt | grep is.sat | sed 's/ .* tests./\t/' | grep .. | sort -n > tests/lists/S
sed 's/.* user //' < benchmark_oneline.txt | grep is.unsat | sed 's/ .* tests./\t/' | grep .. | sort -n > tests/lists/U
grep -v satisfiable  benchmark_oneline.txt | grep .. | sed 's/^ .*tests./?\t/' | shuf > tests/lists/H
