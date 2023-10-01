#include <stdio.h>
#include <mpi.h>
#include <time.h>

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
 * @brief You can do any work without involving the data in communication.
 */
void do_some_work() {
    printf("mid\n");
    time_t t;
    time(&t);
    printf("Today's date and time: %s", ctime(&t));

}


int main(int argc , char* argv []) {
    MPI_Init (&argc , &argv);
    
    int rank, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD , &nproc);

    int tab[N] = {0};
    if (rank == 0) {
        for (int i = 0; i < N; i++) tab[i] = i;
    }

    int dest = 1;
    if (nproc < 2) dest = 0;
    int source = 0;

    MPI_Request req;
    if (rank == source) {
        MPI_Isend(tab, N, MPI_INT, dest, 111, MPI_COMM_WORLD, &req);
    } else if (rank == dest) {
        MPI_Irecv(tab, N, MPI_INT, 0, 111, MPI_COMM_WORLD, &req);
    }
    //printf("rank = %d, tab = ", rank);
   // print_int_array(tab, N);
    // DO SOME WORK IN BETWEEN
    do_some_work();

    // Before using the received data or changing the sent data, make sure Irecv/Isend is done
    if (rank == source) {
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    } else if (rank == dest) {
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    // printf("rank = %d, tab[%d] = %d\n", rank, N-1, tab[N - 1]);
    printf("rank = %d, tab = ", rank);
    print_int_array(tab, N);

    return MPI_Finalize();
}
