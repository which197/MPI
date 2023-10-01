#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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


#define INITIAL_CAPACITY 10
#define GROWTH_FACTOR 2
int* readNumbersFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return NULL;
    }

    int capacity = INITIAL_CAPACITY;
    int* numbers = malloc(capacity * sizeof(int));
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    int numCount = 0;
    int number;

    // Read numbers from the file
    while (fscanf(file, "%d", &number) == 1) {
        // Double the capacity if needed
        if (numCount == capacity) {
            capacity *= GROWTH_FACTOR;
            int* temp = realloc(numbers, capacity * sizeof(int));
            if (temp == NULL) {
                printf("Memory reallocation failed.\n");
                fclose(file);
                free(numbers);
                return NULL;
            }
            numbers = temp;
        }

        numbers[numCount] = number;
        numCount++;
    }

    // Close the file
    fclose(file);

    // Set the count and return the numbers array
    *count = numCount;
    return numbers;
}


int isPrime(int num) {
    if (num <= 1) return 0; // Not prime

    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0; // Not prime
    }

    return 1; // Prime
}


/**
 * @brief Find prime numbers within given array.
 * 
 * @param arr Integer array.
 * @param len Length of integer array.
 * @param num_prime Number of primes.
 * @return The list of primes.
 */
int* prime_numbers_within_array_seq(
    int *arr, int len, int *num_prime)
{
    int *primes = malloc(len * sizeof(*primes));
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (isPrime(arr[i])) {
            primes[count] = arr[i];
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
 * @brief Find prime numbers within given array in parallel.
 * 
 * @param arr Integer array.
 * @param len Length of integer array.
 * @param num_prime Number of primes.
 * @return The list of primes.
 */
int* prime_numbers_within_array_paral(
    int *arr, int len, int *num_prime)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD , &size);

    // TODO: use MPI_Scatter and/or MPI_Scatterv to send the array to each rank
    int local_size = 0;
    int *local_array = NULL;
    // Define the number of elements to be sent to each process
    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));

    // Calculate the sendcounts and displs arrays
    int chunk_size = len / size;
    int remaining = len % size;

    if (rank == 0) {
        // find sendcounts
        for (int i = 0; i < size; i++) {
            sendcounts[i] = chunk_size;
            if (i < remaining) {
                sendcounts[i]++;
            }
        }
        // find displs
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = (displs[i - 1] + sendcounts[i - 1]);
        }
    }

    MPI_Scatter(sendcounts, 1, MPI_INT, &local_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local_array = malloc(local_size * sizeof(int));
    // Scatter the array from the root process to all processes
    MPI_Scatterv(arr, sendcounts, displs, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // find the primes in the local array
    int num_prime_local = 0;
    int *primes_local = prime_numbers_within_array_seq(
        local_array, local_size, &num_prime_local);

    // gather primes_local into primes in rank 0 using MPI_Gatherv
    int *primes = NULL;
    
    // use MPI_Gather to find the numbers of prime numbers in each rank
    int* recvcount = NULL;
    // int* displs = NULL;
    if (rank == 0) {
        recvcount = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
    }
    MPI_Gather(&num_prime_local, 1, MPI_INT, recvcount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // use MPI_Gatherv to gather the result
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i - 1] + recvcount[i - 1]; 
        }
        primes = (int*)malloc((displs[size - 1] + recvcount[size - 1]) * sizeof(int));
    }
    MPI_Gatherv(primes_local, num_prime_local, MPI_INT, primes,
        recvcount, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) *num_prime = displs[size-1] + recvcount[size-1];

    // clean up
    free(primes_local);
    free(local_array);
    if (rank == 0) {
        free(sendcounts);
        free(recvcount);
        free(displs);
    }

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



int main(int argc, char** argv) {
    MPI_Init (&argc , &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);

    char filename[128] = "input.txt";
    if (argc >= 2) {
        strcpy(filename, argv[1]);
    }

    int len;
    int *numbers = NULL;
    
    if (rank == 0) {
        numbers = readNumbersFromFile(filename, &len);
        assert(numbers != NULL);

        printf("Numbers read from the file: %d\n", len);
        // print_int_array(numbers, len);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    const double t1 = MPI_Wtime();

    int num_prime = 0;
    int *primes = prime_numbers_within_array_paral(numbers, len, &num_prime);
    // int *primes = prime_numbers_within_array_seq(numbers, len, &num_prime);

    MPI_Barrier(MPI_COMM_WORLD);
    const double t2 = MPI_Wtime();
    
    if (rank == 0) printf("rank = %d, number of primes found: %d\n", rank, num_prime); 

    // write result on screen and/or to file
    if (rank == 0)
        write_result(primes, num_prime);

    free(primes);
    if (rank == 0) free(numbers);
    if (rank == 0) printf(GRN"Finding primes took: %.3f ms\n"RESET, (t2-t1)*1e3);
    MPI_Finalize();
    return 0;
}
