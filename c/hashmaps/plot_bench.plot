set terminal pdf
set output "map_bnch.pdf"
set autoscale
set xlabel "# Elements"
set ylabel "Micro Sec."
set key left

set title "Map Benchmark: Chained vs. Linear"
plot 	"./map_chained_tree_bench.txt" w linespoints lw 2 t "Chain w/Tree", \
		"./map_linear_bench.txt" w linespoints lw 2 t "Linear Probing"