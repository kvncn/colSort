#include <stdio.h> // mostly for testing rn
#include <stdlib.h>

#include "columnSort.h"

void transposeAndReshape(int** matrix, int len, int width) {

}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


void sortColumns(int** matrix, int len, int width) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < len - 1; j++) {
            for (int k = 0; k < len - j - 1; k++) {
                if (matrix[k][i] > matrix[k + 1][i])
                    swap(&matrix[k][i], &matrix[k + 1][i]);
            }
        }
    }
}

void printMatrix(int** matrix, int length, int width) {
    printf("Matrix:\n");
    for (int i = 0; i < length; i++) {
        printf("|");
        for (int j = 0; j < width; j++) {
            printf(" %3d ", matrix[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}


void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime) {
    // allocate matrix
    int** temp = (int **) malloc (length * sizeof(int*));
    
    // copy over the elements to the matrix
    for (int i = 0; i < length; i++)
        temp[i] = &(A[i * width]);

    printf("Original Matrix:\n");
    printMatrix(temp, length, width);

    // step 1: sort all columns
    sortColumns(temp, length, width);

    printf("Matrix after sorting columns:\n");
    printMatrix(temp, length, width);
}

