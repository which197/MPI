#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>


int lottery(int *pick) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand(rank + 1 + (int)time(NULL));
    // roll a dice to generate a random number between 1 and 10
    int dice = rand() % 10 + 1;
    // if the number is 1, 4, or 7, then it's qualified
    int is_qualified = (dice % 3 == 1);
    // randomly pick a gift, 0 - gift A, 1 - gift B
    *pick = rand() % 2;
    return is_qualified;
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int newRank, newSize;
    MPI_Comm newComm;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int pick = -1;
    int is_qualified = lottery(&pick);
    printf("rank = %d, is_qualified = %d, pick = %d\n", rank, is_qualified, pick);

    // TODO: find the total number of gift A's and B's
    // hint: create a sub-communicator that includes all the qualified processes
    
 // Method 1
 /*   MPI_Comm_split(MPI_COMM_WORLD, is_qualified, rank, &newComm);

    if (is_qualified) {
        int num_gift_a = 0, num_gift_b = 0;
        MPI_Comm_rank(newComm, &newRank);
        MPI_Comm_size(newComm, &newSize);

       // MPI_Allreduce(&pick, &num_gift_b, 1, MPI_INT, MPI_SUM, newComm);
        MPI_Reduce(&pick, &num_gift_b, 1, MPI_INT, MPI_SUM, 0,newComm);
        num_gift_a = newSize-num_gift_b;
        
 */ 
 // Method 2
    int num_gift = 0;
    MPI_Comm_split(MPI_COMM_WORLD, pick, rank, &newComm);
    MPI_Allreduce(&is_qualified, &num_gift, 1, MPI_INT, MPI_SUM, newComm);

    if (pick==0){
        printf("#Gift A required: %d,\n", num_gift);
    }
    if (pick==1){
        printf("#Gift B required: %d,\n", num_gift);
    }
   // printf("#Gift A required: %d,", num_gift);




        // print out the results
//    if (newRank == 0){
//        printf("#Gift A required: %d, #gift B required: %d\n", num_gift_a, num_gift_b);}

    
    MPI_Comm_free(&newComm);

    MPI_Finalize();
    return 0;
}
