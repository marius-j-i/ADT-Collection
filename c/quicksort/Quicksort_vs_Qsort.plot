set terminal pdf 
set output "Quicksort_vs_Qsort.pdf" 
set autoscale 
set xlabel "Elements"
set ylabel "Time"
set key left

set title "Quicksort vs. Qsort"
plot	"quicksort.txt" w linespoints lw 2 t "Quicksort", \
		"qsort.txt" w linespoints lw 2 t "Qsort"

