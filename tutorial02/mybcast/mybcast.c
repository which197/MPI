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

void print_double_array(float *arr, int len) {
    printf("[");
    for (int i = 0; i < len; i++) {
        if (i == 0) printf("%.5f", arr[i]);
        else printf(", %.5f", arr[i]);
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

    if (rank == root) {
        // TODO: Send data to all processes
        for (int j=0;j<size;j++)
        if (j!=root){
            MPI_Send(data, count, datatype, j, 111, comm);
        }
        
        
    } else {
        // TODO: Receive data from root process
        MPI_Status stat;
        MPI_Recv(data, count, datatype, root, 111, comm, &stat);
    }
}


int main(int argc , char* argv []) {
    MPI_Init (&argc , &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    int root = 1;

    float tab[N] = {0};
    if (rank == root) {
        for (int i = root; i < N; i++) tab[i] = i+0.11;
    }

    // MPI_Bcast(tab, N, MPI_INT, root, MPI_COMM_WORLD);
    my_bcast(&tab[0], N, MPI_FLOAT, root, MPI_COMM_WORLD);

    printf("rank = %d, tab = ", rank);
    print_double_array(tab, N);

    return MPI_Finalize();
}
