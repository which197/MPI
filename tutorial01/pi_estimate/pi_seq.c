#include <stdio.h>
#define M_PI 3.14159265358979323846 

/**
 * @brief Find the following integral numerically:
 *          I = \integral_a^b { 4 / (1 + x^2) } dx. 
 * 
 * @param a Lower bound.
 * @param b Upper bound.
 * @param num_intervals Number of intervals within [a,b].
 * @return double 
 */
double find_integral(double a, double b, long num_intervals)
{
    double estimate = 0.0;
    double dx = (b - a) / (double) num_intervals;

    for (long i = 0; i < num_intervals; i++)
    {
        // double x = (i + 0.5) * dx; // more accurate
        double x = i * dx; // less accurate
        estimate += 4.0 / (1.0 + x*x) * dx;
    }

    return estimate;
}


int main(int argc, char **argv)
{
    long num_intervals = 1000000000;
    if (argc > 1) {
        sscanf(argv[1], "%ld", &num_intervals);
    }
    double pi_estimate = find_integral(0.0, 1.0, num_intervals);

    printf("Reference value of pi: %.15f\n", M_PI);
    printf("Estimated value of pi: %.15f\n", pi_estimate);
    printf("N = %ld, Error = %.2e\n", num_intervals, pi_estimate - M_PI);
    return 0;
}

