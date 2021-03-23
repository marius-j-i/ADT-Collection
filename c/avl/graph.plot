set terminal pdf
set output "avl_vs_rbt.pdf"
set autoscale
set xlabel "# Elements"
set ylabel "Micro Sec."
set key left

set title "Insertion"
plot 	"./results/final_avl_insert_bnch.txt" w linespoints lw 2 t "AVL Insert", \
		"../rbt/results/final_rbt_insert_bnch.txt" w linespoints lw 2 t "RBT Insert"

set title "Search"
plot	"./results/final_avl_search_bnch.txt" w linespoints lw 2 t "AVL Search", \
		"../rbt/results/final_rbt_search_bnch.txt" w linespoints lw 2 t "RBT Search"

set title "Sort"
plot	"./results/final_avl_sort_bnch.txt" w linespoints lw 2 t "AVL Sort", \
		"../rbt/results/final_rbt_sort_bnch.txt" w linespoints lw 2 t "RBT Sort"

set title "Iteration"
plot	"./results/final_avl_iterator_bnch.txt" w linespoints lw 2 t "AVL Iterator", \
		"../rbt/results/final_rbt_iterator_bnch.txt" w linespoints lw 2 t "RBT Iterator"