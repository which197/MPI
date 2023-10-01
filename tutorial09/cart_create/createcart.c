#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int dims[2] = {2, 2};  // Size of the grid
    int periods[2] = {1, 0};  // Periodic boundary in the first dimension, non-periodic in the second
    int reorder = 1;  // Allow reordering of process ranks

    if (argc >= 3) {
        dims[0] = atoi(argv[1]);
        dims[1] = atoi(argv[2]);
    }

    if (world_size < dims[0] * dims[1]) {
        fprintf(stderr, "Error: Number of processes (=%d) must not be less than the grid size (=%dx%d).\n",
            world_size, dims[0], dims[1]);
        MPI_Finalize();
        return 1;
    }

    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart_comm);

    if (cart_comm == MPI_COMM_NULL) {
        // these processes are not in the topology
        printf("World Rank %2d, not in Cartesian topology\n", world_rank);
    } else {
        int cart_rank, cart_coords[2];
        MPI_Comm_rank(cart_comm, &cart_rank);
        MPI_Cart_coords(cart_comm, cart_rank, 2, cart_coords);

        printf("World Rank %2d, Cartesian Rank %2d, Cart Coordinates (%2d, %2d)\n",
            world_rank, cart_rank, cart_coords[0], cart_coords[1]);

        MPI_Comm_free(&cart_comm);
    }

    MPI_Finalize();
    return 0;
}
