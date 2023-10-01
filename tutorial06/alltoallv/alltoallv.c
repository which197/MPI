#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create data arrays for sending and receiving
    int* send_data = (int*)malloc(world_size * world_size * sizeof(int));
    int* recv_data = (int*)malloc(world_size * world_size * sizeof(int));

    // Initialize send_data with some values
    for (int i = 0; i < world_size; i++) {
        for (int j = 0; j < world_size; j++) {
            send_data[i * world_size + j] = world_rank * world_size + j;
        }
    }

    // Print the data to send
    printf("Process %d to send:\n", world_rank);
    for (int i = 0; i < world_size; i++) {
        for (int j = 0; j < world_size; j++) {
            printf("%4d ", send_data[i * world_size + j]);
        }
        printf("\n");
    }
    sleep(1);

    // Define send counts, send displacements, receive counts, and receive displacements
    int send_counts[world_size];
    int send_displacements[world_size];
    int recv_counts[world_size];
    int recv_displacements[world_size];

    for (int i = 0; i < world_size; i++) {
        send_counts[i] = world_size;
        send_displacements[i] = i * world_size;
        recv_counts[i] = world_size;
        recv_displacements[i] = i * world_size;
    }

    // Perform the all-to-all communication
    MPI_Alltoallv(send_data, send_counts, send_displacements, MPI_INT,
                  recv_data, recv_counts, recv_displacements, MPI_INT,
                  MPI_COMM_WORLD);

    // Print the received data
    printf("Process %d received:\n", world_rank);
    for (int i = 0; i < world_size; i++) {
        for (int j = 0; j < world_size; j++) {
            printf("%4d ", recv_data[i * world_size + j]);
        }
        printf("\n");
    }

    // Free dynamically allocated memory
    free(send_data);
    free(recv_data);

    MPI_Finalize();
    return 0;
}
