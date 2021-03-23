/* Author: Marius Ingebrigtsen */

#include "quicksort.h"


/* Choose right-most item as pivot. 
 * Sort all items less than pivot to the left, and vice versa to the right. 
 * Return index number of pivot-item. */
static int partition(int *array, int low, int high) {
	int pivot, pividx, tmp;

	pivot	= array[high];
	pividx	= low;

	for(int left = low; left < high; left++) {
		if(array[left] < pivot) {
			tmp				= array[left];
			array[left]		= array[pividx];
			array[pividx]	= tmp;

			pividx++;
		}
	}
	array[high]		= array[pividx];
	array[pividx]	= pivot;

	return pividx;
}

/* Recursively sort section-by-section. */
static void _quicksort(int *array, int low, int high) {
	int pivot;

	if(low < high) {
		pivot = partition(array, low, high);

		_quicksort(array, low, pivot - 1);
		_quicksort(array, pivot + 1, high);
	}
}

/* Quicksort algorithm. */
void quicksort(int *array, int num) {
	_quicksort(array, 0, num - 1);
}