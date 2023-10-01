#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


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


void show_int_array(int *array, int n) {
    if (n <= 20) {
        printf("([");
        for (int i = 0; i < n; i++) {
            if (i > 0) printf(" ");
            printf("%d", array[i]);
        }
        printf("], len = %d)\n",n);
        return;
    }

    printf("([");
    for (int i = 0; i < 10; i++) {
        if (i > 0) printf(" ");
        printf("%d", array[i]);
    }
    printf(" ...");
    for (int i = n-10; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", array[i]);
    }
    printf("], len = %d)\n",n);
}


void write_result(int *arr, int len, char* filename) {
    show_int_array(arr, len);
    printf("result printed to: %s\n", filename);
    // write result to file
    printArrayToFile(filename, arr, len);
}
