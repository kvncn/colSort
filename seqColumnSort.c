#include <stdio.h> // mostly for testing rn
#include <stdlib.h>

#include "columnSort.h"

// ---- sorting
void sortColumns(int** matrix, int len, int width);
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime);

// ---- matrix transformations
int** transpose(int** matrix, int len, int width);
int** reshape(int** matrix, int len, int width);

// ---- helpers
void freeMatrix(int** matrix, int len, int width);
int** copyMatrix(int** matrix, int len, int width);
void printMatrix(int** matrix, int length, int width);
void swap(int* a, int* b);

// ---- sorting functions

/**
 * 
 */
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime) {
    // allocate matrix
    int** mat = malloc (length * sizeof(int*));

    for (int i = 0; i < length; i++)
        mat[i] = malloc(width * sizeof(int));
     
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            mat[i][j] = A[i * width + j];
        }
    }

    printf("Original Matrix:\n");
    printMatrix(mat, length, width);

    // step 1: sort all columns
    sortColumns(mat, length, width);
    printf("Step 1: Matrix after sorting columns:\n");
    printMatrix(mat, length, width);

    // step 2: transpose and reshape
    mat = transpose(mat, length, width);
    mat = reshape(mat, length, width);
    printf("Step 2: Matrix after transposing and reshaping columns:\n");
    printMatrix(mat, length, width);

    // step 3: sort all columns
    sortColumns(mat, length, width);
    printf("Step 3: Matrix after sorting columns:\n");
    printMatrix(mat, length, width);

    // step 4: reshape and transpose
    mat = reshape(mat, width, length);
    mat = transpose(mat, width, length);
    printf("Step 4: Matrix after reshaping and transposing columns:\n");
    printMatrix(mat, length, width);

    // step 5: sort all columns
    sortColumns(mat, length, width);
    printf("Step 5: Matrix after sorting columns:\n");
    printMatrix(mat, length, width);


    

    // use this to copy over everything back into A 
    //transpose(temp, length, width);
    int* ptr = &A[0];
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            *ptr = mat[i][j];
            ptr++;
        }
    }   

    // free up memory
    free(mat);

}

/**
 * 
 */
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

// ---- matrix transformations 

/**
 * 
 */
int** transpose(int** matrix, int len, int width) {
    int** tr = malloc(width * sizeof(int*));

    for (int i = 0; i < width; i++)
        tr[i] = malloc(len * sizeof(int)); 
    
    for (int i = 0; i < len; i++)
        for (int j = 0; j < width; j++)
            tr[j][i] = matrix[i][j];
    
    freeMatrix(matrix, len, width);

    return tr; 

}

/**
 * 
 */
int** reshape(int** matrix, int len, int width) {
    int** res = malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        res[i] = malloc(width * sizeof(int)); 

    for (int i = 0; i < len * width; i++) {
        int row = i / width; 
        int col = i % width; 
        res[row][col] = matrix[i / len][i % len];
    }

    freeMatrix(matrix, width, len);

    return res;

}


// ---- helpers 
void printMatrix(int** matrix, int length, int width) {
    for (int i = 0; i < length; i++) {
        printf("|");
        for (int j = 0; j < width; j++) {
            printf(" %4d ", matrix[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

/**
 * 
 */
void freeMatrix(int** matrix, int len, int width) { 
    for (int i = 0; i < len; i++)
        free(matrix[i]);
    
    free(matrix);
}

/**
 * 
 */
// int** copyMatrix(int** matrix, int len, int width) {
//     // allocate temporary copy of the matrix
//     int** temp = malloc(len * sizeof(int*));

//     for (int i = 0; i < len; i++) {
//         temp[i] = malloc(width * sizeof(int));
//     } 

//     // copy over elements
//     for (int i = 0; i < len; i++) {
//         for (int j = 0; j < width; j++)
//             temp[i][j] = matrix[i][j];
//     }

//     return temp;
// }

/**
 * 
 */
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

