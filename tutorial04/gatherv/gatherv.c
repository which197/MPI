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


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Generate local data for each process
    int localDataSize = rank + 1;
    int* localData = (int*)malloc(localDataSize * sizeof(int));
    for (int i = 0; i < localDataSize; i++) {
        localData[i] = rank;
    }


    // Gather information about the local data size from all processes #知道每个进程有多少数
    int* recvCounts = NULL;
    int* displacements = NULL;
    if (rank == 0) {
        recvCounts = (int*)malloc(size * sizeof(int));
        displacements = (int*)malloc(size * sizeof(int));
    }
    MPI_Gather(&localDataSize, 1, MPI_INT, recvCounts, 1, MPI_INT, 0, MPI_COMM_WORLD);


    // Calculate displacements for each process
    if (rank == 0) {
        displacements[0] = 0;
        for (int i = 1; i < size; i++) {
            displacements[i] = displacements[i - 1] + recvCounts[i - 1];
        }
    }

    // Gather local data from all processes into the root process 

    int* gatheredData = NULL;
    if (rank == 0) {
        gatheredData = (int*)malloc((displacements[size - 1] + recvCounts[size - 1]) * sizeof(int));
    }
    MPI_Gatherv(localData, localDataSize, MPI_INT, gatheredData, recvCounts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the gathered data in the root process
    if (rank == 0) {
        printf("Gathered data:\n");
        for (int i = 0; i < displacements[size - 1] + recvCounts[size - 1]; i++) {
            printf("%d ", gatheredData[i]);
        }
        printf("\n");
    }

    // Clean up
    free(localData);
    if (rank == 0) {
        free(recvCounts);
        free(displacements);
        free(gatheredData);
    }

    MPI_Finalize();
    return 0;
}

