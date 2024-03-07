#include <stdio.h> // mostly for testing rn
#include <stdlib.h>

#include "columnSort.h"

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

void transposeAndReshape(int** matrix, int len, int width) {
    // allocate temporary copy of the matrix
    int** temp = malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++) {
        temp[i] = malloc(width * sizeof(int));
    } 

    // copy over elements
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < width; j++)
            temp[i][j] = matrix[i][j];
    }

    // this takes care of the transpose and reshape, because
    // we want to iterate columnwise and then put into a 
    // sequential (i.e.) rowwise order in the final result
    int* ptr = matrix[0]; 

    // iterate columwise 
    for (int j = 0; j < width; j++) {
        // for every element, we jump row while keeping the column fixed
        // then we increment our pointer by one, so we actually iterate 
        // rowwise 
        for (int i = 0; i < len; i++) {
            *ptr = temp[i][j]; 
            ptr++;
        }
    }

    // free up the extra memory we used
    for (int i = 0; i < len; i++) {
        free(temp[i]);
    }
    free(temp);
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


void sortColumns(int** matrix, int len, int width) {
    // performs bubble sort in a columnwise traversal
    for (int i = 0; i < width; i++) {
        // here we are just doing bubble sort
        for (int j = 0; j < len - 1; j++) {
            for (int k = 0; k < len - j - 1; k++) {
                if (matrix[k][i] > matrix[k + 1][i])
                    swap(&matrix[k][i], &matrix[k + 1][i]);
            }
        }
    }
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

    // step 2: transpose and reshape
    transposeAndReshape(temp, length, width);
    printf("Matrix after transposing and reshaping columns:\n");
    printMatrix(temp, length, width);

    // step 3: sort all columns
    sortColumns(temp, length, width);
    printf("Matrix after sorting columns:\n");
    printMatrix(temp, length, width);
}

