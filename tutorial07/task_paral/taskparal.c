#include <stdio.h>
#include <mpi.h>

#define GRN "\x1B[32m"
#define RESET "\x1B[0m"
int task1(int argc, char **argv, MPI_Comm comm);
int task2(int argc, char** argv, MPI_Comm comm);


int main(int argc, char** argv) {
    int rank, size, color, rank_taskcomm, size_taskcomm;
    MPI_Comm taskcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    // Determine the color based on the rank
    if (rank < size/2) {
        color = 0;  // first half
    } else {
        color = 1;  // second half
    }

    // Split the communicator based on the color
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &taskcomm); // color, rank should be nonnegative !!!

    // Get the new rank and size in the new communicator
    MPI_Comm_rank(taskcomm, &rank_taskcomm);
    MPI_Comm_size(taskcomm, &size_taskcomm);

    // Print the information in the original and new communicators
    // printf("worldcomm rank/size: %d/%d => taskcomm rank/size: %d/%d (color %d)\n", rank, size, rank_taskcomm, size_taskcomm, color);

    // Perform different tasks within different taskcomm
    if (color == 0) {
        task1(argc, argv, taskcomm);
    } else if (color == 1) {
        task2(argc, argv, taskcomm);
    }

    MPI_Comm_free(&taskcomm);

    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();
    if (rank == 0) printf(GRN "The whole program took %.3f ms\n" RESET, (t2-t1)*1e3);

    MPI_Finalize();
    return 0;
}
