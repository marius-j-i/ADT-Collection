set terminal pdf 
set output "AVL_vs_RBT_Insert.pdf" 
set autoscale 
set xlabel "Elements"
set ylabel "Microsec."
set key left

set title "AVL- vs. RBT-Insert"
plot	"results/avl_insert.txt" w linespoints lw 2 t "AVL-Insert", \
		"results/rbt_insert.txt" w linespoints lw 2 t "RBT-Insert"

set title "AVL- vs. RBT-Search"
plot	"results/avl_search.txt" w linespoints lw 2 t "AVL-Insert", \
		"results/rbt_search.txt" w linespoints lw 2 t "RBT-Insert"

