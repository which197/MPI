#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "tools.h"

/**
 * @brief Swap values in a and b.
 * 
 * @param a Pointer to 1st value.
 * @param b Pointer to 2nd value.
 */
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


/**
 * @brief Select a pivot element (in this case, the last element of the array) 
 * and partitions the array such that all elements smaller than the pivot are
 * placed to its left and all elements greater than the pivot are placed to its
 * right. The function returns the index of the pivot element.
 * 
 * @param arr Array to be partitioned.
 * @param low Start index of the section of the array to be partitioned.
 * @param high End index of the section of the array to be partitioned.
 * @return int Index of the pivot element after partition.
 */
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);
    return (i); // return index of pivot
}


void quicksort_seq(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quicksort_seq(arr, low, pi - 1);
        quicksort_seq(arr, pi + 1, high);
    }
}



void quicksort(int arr[], int low, int high, MPI_Comm comm) {
    if (low >= high) return;

    int pivotInd = partition(arr, low, high); // do MPI

    // TODO: create sub-communicators to solve the problem
    MPI_Comm subcomm;
    
    // MPI_Comm_split( ... );

    quicksort(arr, low, pivotInd - 1, subcomm);
    quicksort(arr, pivotInd + 1, high, subcomm);

    MPI_Comm_free(&subcomm);
}


int main(int argc, char** argv) {
    MPI_Init (&argc , &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    // specify the array to be sorted
    int len;
    int *arr = NULL;
    // int arr[] = {10, 7, 8, 9, 1, 5, 3, 7, 11};
    // int len = sizeof(arr) / sizeof(arr[0]);
    
    char filename[128], outfname[128];
    strcpy(filename, "inputs/input1.txt");
    strcpy(outfname, "result.txt");

    if (rank == 0) {
        // read numbers from input file
        if (argc >= 2) strcpy(filename, argv[1]);

        arr = readNumbersFromFile(filename, &len);
        assert(arr != NULL);

        printf("Numbers read from the file: %d\n", len);
        printf("Original array: ");
        show_int_array(arr, len);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    // quicksort(arr, 0, len - 1, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();

    printf("Sorted array: ");
    write_result(arr, len, outfname);

    if (rank == 0) printf("quicksort took %.3f ms\n", (t2-t1)*1000);

    if (rank == 0) free(arr);
    MPI_Finalize();
    return 0;
}
