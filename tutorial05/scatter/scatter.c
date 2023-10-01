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
    int rank, size, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double x = -1.0;
    double *buf;
    if (rank == root) {
        buf = malloc(size * sizeof(double));
        for (int i = 0; i < size; ++i) buf[i] = 1.111*i;
    }
    MPI_Scatter(&buf[0], 1, MPI_DOUBLE,
                &x, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    printf("rank = %d, x = %.3f\n", rank, x);
    MPI_Finalize();
    return 0;
}

