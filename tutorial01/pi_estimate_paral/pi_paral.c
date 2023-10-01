#include <stdio.h>
#include <mpi.h>

#define M_PI 3.14159265358979323846 

/**
 * @brief Given num_intervals intervals, find the start index
 * assigned to each process.
 * 
 * @param num_intervals Total number of intervals. 
 * @param nproc Number of processes.
 * @param rank Rank of the process.
 * @return long 
 */
long find_i_start(long num_intervals, int nproc, int rank) {
    long n = num_intervals;
    int p = nproc;
    return n / p * rank + ((rank < n % p) ? rank : (n % p));
}


long find_i_end(int num_intervals, int nproc, int rank) {
    return find_i_start(num_intervals, nproc, rank+1) - 1;
}


/**
 * @brief Find the following integral numerically in parallel:
 *          I = \integral_a^b { 4 / (1 + x^2) } dx. 
 * 
 * @param a Lower bound.
 * @param b Upper bound.
 * @param num_intervals Number of intervals within [a,b].
 * @return double 
 */
double find_integral_paral(double a, double b, long num_intervals)
{
    int rank, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    double local_estimate = 0.0;
    double dx = (b - a) / (double) num_intervals;

    long i_start = find_i_start(num_intervals, nproc, rank);
    long i_end = find_i_end(num_intervals, nproc, rank);
    // printf("rank/nproc = %d/%d, i_start = %ld, i_end = %ld\n", rank, nproc, i_start, i_end);

    for (long i = i_start; i <= i_end; i++)
    {
        // double x = (i + 0.5) * dx; // more accurate
        double x = i * dx; // less accurate
        local_estimate += 4.0 / (1.0 + x*x) * dx;
    }

    double global_estimate = 0.0;
    // sum over all the processes to get the global sum
    MPI_Allreduce(&local_estimate, &global_estimate, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    return global_estimate;
}



int main(int argc, char **argv)
{
    printf("start\n");
    MPI_Init(&argc, &argv);

    long num_intervals = 1000000000;
    if (argc > 1) {
        sscanf(argv[1], "%ld", &num_intervals);
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double t1 = MPI_Wtime();
    double pi_estimate = find_integral_paral(0.0, 1.0, num_intervals);
    double t2 = MPI_Wtime();

    if (rank == 0) {
        printf("Reference value of pi: %.15f\n", M_PI);
        printf("Estimated value of pi: %.15f\n", pi_estimate);
        printf("N = %ld, Error = %.2e\n", num_intervals, pi_estimate - M_PI);
        printf("The program took: %.3f ms\n", (t2-t1)*1e3);
    }

    MPI_Finalize();
    return 0;
}

