#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc,&argv);
    int rank, size;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    printf("Hello world from %d/%d\n",rank,size);
    
    // test allreduce
    int sum = 0;
    MPI_Allreduce(&rank, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    printf("sum = %d in rank %d\n",sum, rank);
    
    MPI_Finalize();
    return 0;
}
