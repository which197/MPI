#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


#define NUMBERS_PER_LINE 10
#define NUMBER_WIDTH 6
void printArrayToFile(const char *filename, int *array, int size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        // Print each number with fixed width
        fprintf(file, "%*d", NUMBER_WIDTH, array[i]);

        // Print a space between numbers
        if ((i + 1) % NUMBERS_PER_LINE != 0) {
            fprintf(file, " ");
        } else {
            fprintf(file, "\n");  // Start a new line after printing a fixed number of numbers
        }
    }

    fclose(file);
}



int isPrime(int num) {
    if (num <= 1) return 0; // Not prime

    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0; // Not prime
    }

    return 1; // Prime
}


/**
 * @brief Find prime numbers within [a,b].
 * 
 * @param a Lower bound.
 * @param b Upper bound.
 * @param num_prime Number of primes.
 * @return The list of primes.
 */
int* prime_numbers_within_interval_seq(
    int a, int b, int *num_prime)
{
    int *primes = malloc((b-a+1) * sizeof(*primes));
    int count = 0;
    for (int i = a; i <= b; i++) {
        if (isPrime(i)) {
            primes[count] = i;
            count++;
        }
    }
    primes = realloc(primes, count * sizeof(*primes));
    *num_prime = count;
    return primes;
}


/**
 * @brief Given n tasks, find the start index assigned to each process.
 * 
 * @param n Total number of tasks. 
 * @param nproc Number of processes.
 * @param rank Rank of the process.
 * @return long 
 */
long find_i_start(long n, int nproc, int rank) {
    int p = nproc;
    return n / p * rank + ((rank < n % p) ? rank : (n % p));
}


long find_i_end(long n, int nproc, int rank) {
    return find_i_start(n, nproc, rank+1) - 1;
}


/**
 * @brief Find prime numbers within [a,b] in parallel.
 * 
 * @param a Lower bound.
 * @param b Upper bound.
 * @param num_prime Number of primes.
 * @return The list of primes.
 */
int* prime_numbers_within_interval_paral(
    int a, int b, int *num_prime)
{
    int rank, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD , &nproc);

    int ntask = b - a + 1;
    long i_start = find_i_start(ntask, nproc, rank) + a;
    long i_end = find_i_end(ntask, nproc, rank) + a;
    printf("rank/nproc = %d/%d, i_start = %ld, i_end = %ld\n", rank, nproc, i_start, i_end);
    
    int num_prime_local = 0;
    int *primes_local = prime_numbers_within_interval_seq(i_start, i_end, &num_prime_local);
    
    printf("rank = %d, num_prime_local = %d\n", rank, num_prime_local);
    print_int_array(primes_local, num_prime_local);

    // gather primes_local into primes in rank 0 using MPI_Gatherv
    int *primes = NULL;
    // TODO: use MPI_Gather to find the numbers of prime numbers in each rank
    int* recvCounts = NULL;
    int* displacements = NULL;
    if (rank == 0) {
        recvCounts = (int*)malloc(nproc * sizeof(int));
        displacements = (int*)malloc(nproc * sizeof(int));
    }


    MPI_Gather(&num_prime_local, 1, MPI_INT, recvCounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    printf("recv:\n");
    if (rank == 0) print_int_array(recvCounts,nproc);

    if (rank == 0) {
        displacements[0] = 0;
        for (int i = 1; i < nproc; i++) {
            displacements[i] = displacements[i - 1] + recvCounts[i - 1];
        }
    }

    // TODO: use MPI_Gatherv to gather the result
    if (rank == 0) {
        primes = (int*)malloc((displacements[nproc - 1] + recvCounts[nproc - 1]) * sizeof(int));
        print_int_array(primes_local, num_prime_local);
        print_int_array(displacements, nproc);
    }
    MPI_Gatherv(primes_local, num_prime_local, MPI_INT, primes, recvCounts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) 
        *num_prime = displacements[nproc-1] + recvCounts[nproc-1];
    else
        *num_prime = 0;

    // clean up
    free(primes_local);

    return primes;
}


void write_result(int *primes, int num_prime) {
    char filename[128];
    strcpy(filename, "result.txt");
    if (num_prime < 20) {
        // print result on screen if it's short
        printf("prime numbers = ");
        print_int_array(primes, num_prime);
    } else {
        printf("result printed to: %s\n", filename);
    }
    // write result to file
    printArrayToFile(filename, primes, num_prime);
}


int main(int argc , char* argv []) {
    MPI_Init (&argc , &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    // find prime numbers within the given interval
    int a = 1, b = 1;
    if (argc >= 3) {
        a = atoi(argv[1]);
        b = atoi(argv[2]);
    }

    int num_prime = 0;
    int *primes = prime_numbers_within_interval_paral(a, b, &num_prime);
    
    if (rank == 0) printf("rank = %d, num_prime = %d\n", rank, num_prime); 
    
    // write result on screen and/or to file
    if (rank == 0)
        write_result(primes, num_prime);

    free(primes);
    MPI_Finalize();
    return 0;
}
