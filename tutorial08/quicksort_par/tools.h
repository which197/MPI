#ifndef TOOLS_H
#define TOOLS_H

/**
 * @brief Print an array of type int.
 * 
 * @param arr Array to be printed.
 * @param len Length of the int array.
 */
void print_int_array(int *arr, int len);

// if array is too long, only print first 5 and last 5 components
void show_int_array(int *array, int n);


void printArrayToFile(const char *filename, int *array, int size);


int* readNumbersFromFile(const char* filename, int* count);


void write_result(int *primes, int num_prime, char* filename);

#endif // TOOLS_H
