comm <((grep IsSat ~/store/summary.txt  -b3 | grep -o H_..._ ; echo 000) | sort -u)  <(grep :.sat /home/john/store/old.2017-04-29T14:45:08,956063594+08:00/*tree* -B 1 | grep H_..._ -o | sort -u )  | less

