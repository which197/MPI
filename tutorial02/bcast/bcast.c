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


int main(int argc , char* argv []) {
    MPI_Init (&argc , &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    int tab[N] = {0};   //8个数，都是0
    if (rank == 0) {
        for (int i = 0; i < N; i++) tab[i] = i;
    }

    printf("rank = %d, tab = ", rank);
    print_int_array(tab, N);
    MPI_Barrier(MPI_COMM_WORLD);

    
    MPI_Bcast(tab, N, MPI_INT, 0, MPI_COMM_WORLD);  //tab等价 &tab[0]

    // printf("rank = %d, tab[%d] = %d\n", rank, N-1, tab[N - 1]);
    printf("rank = %d, tab = ", rank);
    print_int_array(tab, N);

    MPI_Finalize();
    return 0;
}
