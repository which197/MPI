#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define GRN "\x1B[32m"
#define RESET "\x1B[0m"

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
    int a, int b, int *num_prime, MPI_Comm comm)
{
    int rank, size;
    MPI_Comm_rank(comm , &rank);
    MPI_Comm_size(comm , &size);

    int ntask = b - a + 1;
    long i_start = find_i_start(ntask, size, rank) + a;
    long i_end = find_i_end(ntask, size, rank) + a;
    
    int num_prime_local = 0;
    int *primes_local = prime_numbers_within_interval_seq(i_start, i_end, &num_prime_local);
    // printf("rank/nproc = %d/%d, i_start = %ld, i_end = %ld, num_prime_local = %d\n", rank, size, i_start, i_end, num_prime_local);

    // gather primes_local into primes in rank 0 using MPI_Gatherv
    int *primes = NULL;
    // use MPI_Gather to find the numbers of prime numbers in each rank
    int* recvcount = NULL;
    int* displs = NULL;
    if (rank == 0) {
        recvcount = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
    }
    MPI_Gather(&num_prime_local, 1, MPI_INT, recvcount, 1, MPI_INT, 0, comm);
    // if (rank == 0) print_int_array(recvcount, size);

    // use MPI_Gatherv to gather the result
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i - 1] + recvcount[i - 1]; 
        }
        primes = (int*)malloc((displs[size - 1] + recvcount[size - 1]) * sizeof(int));
    }
    MPI_Gatherv(primes_local, num_prime_local, MPI_INT, primes,
        recvcount, displs, MPI_INT, 0, comm);

    if (rank == 0) *num_prime = displs[size-1] + recvcount[size-1];

    // clean up
    if (rank == 0) {
        free(recvcount);
        free(displs);
    }
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


int task2(int argc , char* argv [], MPI_Comm comm) {
    int rank;
    MPI_Comm_rank(comm, &rank);

    // find prime numbers within the given interval
    int a = 1, b = 100;
    if (argc >= 4) {
        a = atoi(argv[2]);
        b = atoi(argv[3]);
    }

    int num_prime = 0;
    double t1 = MPI_Wtime();
    int *primes = prime_numbers_within_interval_paral(a, b, &num_prime, comm);
    double t2 = MPI_Wtime();
    if (rank == 0) printf("rank = %d, num_prime = %d\n", rank, num_prime); 

    // write result on screen and/or to file
    if (rank == 0) {
        write_result(primes, num_prime);
        printf(GRN "Finding primes took: %.3f ms\n" RESET, (t2-t1)*1e3);
    }

    free(primes);
    return 0;
}
