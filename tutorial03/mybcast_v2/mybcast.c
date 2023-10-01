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
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    int dest = 1;
    if (nproc < 2) dest = 0;
    int source = 0;

    MPI_Request req;

    // TODO: implement your own Bcast routine
    

    MPI_Isend(*data, count, datatype, dest, tag, comm,  *req);


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
