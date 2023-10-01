#include <stdio.h>
#include <mpi.h>

#define N 8

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


/**
 * @brief Implement Bcast routine using only MPI_Send and MPI_Recv.
 * 
 * @param data Data to be broadcasted.
 * @param count Length of the data.
 * @param datatype Type of the data.
 * @param root Rank of the root process.
 * @param comm The communicator in which Bcast happens.
 */
void my_bcast(void *data, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int rank, comm_size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    int relative_rank = (rank >= root) ? rank - root : rank - root + comm_size;

    // Use binomial tree
    int mask = 1;
    while (mask < comm_size) {
        if (relative_rank & mask) {
            int src = rank - mask; // remove the least significant 1 bit
            if (src < 0) src += comm_size;
            MPI_Recv(data, count, datatype, src, 110, comm, MPI_STATUS_IGNORE);
            break;
        }
        mask *= 2; // mask <<= 1;
    }

    mask /= 2;
    while (mask > 0) {
        if (relative_rank + mask < comm_size) {
            int dest = rank + mask;
            if (dest >= comm_size) dest -= comm_size;
            MPI_Send (data, count, datatype, dest, 110, comm); 
        }
        mask /= 2; // mask >>= 1;
    }
}


int main(int argc , char* argv []) {
    MPI_Init (&argc , &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    int root = 0;

    int tab[N] = {0};
    if (rank == root) {
        for (int i = 0; i < N; i++) tab[i] = i;
    }

    // MPI_Bcast(tab, N, MPI_INT, root, MPI_COMM_WORLD);
    my_bcast(tab, N, MPI_INT, root, MPI_COMM_WORLD);

    printf("rank = %d, tab = ", rank);
    print_int_array(tab, N);

    return MPI_Finalize();
}
