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

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Define the size of the array
    int array_size = 12;

    // Root process initializes the array
    int* array = NULL;
    if (world_rank == 0) {
        array = (int*)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = i + 1;
        }
    }

    // Define the number of elements to be sent to each process
    int* sendcounts = (int*)malloc(world_size * sizeof(int));
    int* displs = (int*)malloc(world_size * sizeof(int));

    // Calculate the sendcounts and displs arrays
    int chunk_size = array_size / world_size;
    int remaining = array_size % world_size;
    
    if (world_rank == 0) {
        // find sendcounts
        for (int i = 0; i < world_size; i++) {
            sendcounts[i] = chunk_size;
            if (i < remaining) {
                sendcounts[i]++;
            }
        }
        // find displs
        displs[0] = 0;
        for (int i = 1; i < world_size; i++) {
            displs[i] = (displs[i - 1] + sendcounts[i - 1]);
        }
    }

    // Allocate memory to receive the scattered elements
    // int local_size = sendcounts[world_rank]; // sendcounts not always accessible from all ranks
    int local_size = 0;
    MPI_Scatter(sendcounts, 1, MPI_INT, &local_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int* local_array = (int*)malloc(local_size * sizeof(int));

    // Scatter the array from the root process to all processes
    MPI_Scatterv(array, sendcounts, displs, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the received elements on each process
    printf("rank = %d, received elements: ", world_rank);
    print_int_array(local_array, local_size);

    // Clean up
    if (world_rank == 0) {
        free(array);
    }
    free(sendcounts);
    free(displs);
    free(local_array);

    MPI_Finalize();
    return 0;
}

