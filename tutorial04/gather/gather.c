#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * @brief Print an array of type int.
 * 
 * @param arr Array to be printed.
 * @param len Length of the int array.
 */
void print_int_array(int *arr, int len) {
    printf("[");
    for (int i = 0; i < len; i++) {
        if (i == 0) printf("%d", arr[i]);
        else printf(", %d", arr[i]);
    }
    printf("]\n");
}


int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int *array;
    int root = 0;
    if (rank == root) {
        array = malloc(size * sizeof(int));
    }

    MPI_Gather(&rank, 1, MPI_INT, &array[0], 1, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == 0) {
        print_int_array(array, size);
        free(array);
    }

    MPI_Finalize();
    return 0;
}
