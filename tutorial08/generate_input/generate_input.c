#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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




void write_result(int *nums, int len) {
    char filename[128];
    strcpy(filename, "input.txt");
    if (len < 20) {
        // print result on screen if it's short
        printf("Numbers generated = ");
        print_int_array(nums, len);
    } else {
        printf("result printed to: %s\n", filename);
    }
    // write result to file
    printArrayToFile(filename, nums, len);
}


void generateUnsortedArray(int arr[], int n, int MAXNUM, int shuffle) {
    if (shuffle) {
        srand(time(0));  // Seed the random number generator with the current time
    } else {
        srand(1);
    }
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % MAXNUM;  // Generate random numbers between 0 and 99
    }
}


void print_help() {
    printf("===========================================================\n");
    printf("USAGE: \n");
    printf("./generate_input <n> <maxnum> <shuffle>\n\n");
    printf("DESCRIPTION\n");
    printf("    <n>        Number of values to be generated.\n");
    printf("    <maxnum>   Upperbound of random number to be generated.\n");
    printf("    <shuffle>  Whether to shuffle the random seed.\n");
    printf("               0 - Do not shuffle.\n");
    printf("               1 - Shuffle.\n");
    printf("===========================================================\n");
}

int main(int argc , char* argv []) {
    print_help();
    
    // find prime numbers within the given interval
    int n = 1, maxnum = 1000, shuffle = 0;
    if (argc > 1) n = atoi(argv[1]);
    if (argc > 2) maxnum = atoi(argv[2]);
    if (argc > 3) shuffle = atoi(argv[3]);

    int *numbers = malloc(n * sizeof(int));
    generateUnsortedArray(numbers, n, maxnum, shuffle);
    
    printf("n = %d\n", n); 

    // write result on screen and/or to file
    write_result(numbers, n);

    free(numbers);
    return 0;
}
