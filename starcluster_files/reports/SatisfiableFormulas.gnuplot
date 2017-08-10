set xlabel "CPU time limit"
set ylabel "formulas satisfied"
#set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
#set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"
set grid ytics mytics  # draw lines for each ytics and mytics
set mytics 2           # set the spacing for the mytics
set grid               # enable the grid
#set mytics 10
set autoscale
#set terminal postscript portrait enhanced mono dashed lw 1 'Helvetica' 14
set terminal pdf enhanced dashed lw 2 lw 3
# enhanced mono
# dashed lw 1 'Helvetica' 14
#set style line 1 lt 1 lw 3 pt 3 linecolor rgb "red"
#set output 'out.eps'
set output 'out.pdf'
set key right bottom
#set term x11 persist
#set style line 2 lt 2 lc rgb "orange" lw 2
#set dashtype 11 (2,4,4,7)
#set style line 1 lt 1 lw 3 pt 3 linecolor rgb "red"
#plot 'Si.txt' using 2:1 w line title "serial: x seconds, 1 job", 'S_parallel.txt' using 2:1 w linespoints lt 2 title "{/:bold split\\_depth}=64, parallel: {/:Italic x} jobs, 1 second" 
plot 'store/Si.txt' using 2:1 w line title "serial: run 1 job for x seconds", 'store/S_parallel.txt' using 2:1 w linespoints lt 2 title "{/:bold split\\_depth}=64, parallel: run {/:Italic x} jobs for 1 second", 'store/hybrid.txt' using 1:2 w line lt 3 title "hybrid: run {/:Italic 32} jobs for 1 second AND 1 job for x-32 seconds"
