/**
 * @author Kevin Cascais Nisterenko
 * @date March 8th, 2024
 * @class CSc 422, Spring 2024
 * 
 * Sequential program that performs columnSort on a given matrix. The function 
 * for columnSort and its helpers are present, and it is to be used/called by 
 * another program, which will pass a vector of integers and dimensions for the
 * matrix that will have the algorithm performed on. 
 */
#include <stdio.h> // mostly for testing and print function
#include <stdlib.h>
#include <sys/time.h> 

#include "columnSort.h"
#include "columnSortHelper.h"

/**
 * Sorts every column in a matrix, uses bubble sort to do so  
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
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

/**
 * Column Sort algorithm function. It takes in a matrix's dimensions and the vector
 * with the integers in it, builds the matrix, performs all the steps of columnSort 
 * and then repopulates the vector with the numbers in sorted order. It also 
 * times the algorithm. 
 * 
 * @param A, vector of integers to sort, and where the results are stored at the end
 * @param numThreads, integer representing the numbr of threads, ignored here
 * @param length, number of rows of the matrix
 * @param width, number of cols of the matrix
 * @param elapsedTime, double pointer where the time is stored
 */
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime) {
    // allocate matrix
    int** mat = (int**) malloc (length * sizeof(int*));

    for (int i = 0; i < length; i++)
        mat[i] = (int*) malloc(width * sizeof(int));
     
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            mat[i][j] = A[i * width + j];
        }
    }

    //printf("Original Matrix:\n");
    //printMatrix(mat, length, width);

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // step 1: sort all columns
    sortColumns(mat, length, width);
    //printf("Step 1: Matrix after sorting columns:\n");
    //printMatrix(mat, length, width);

    // step 2: transpose and reshape
    mat = transpose(mat, length, width);
    mat = reshape(mat, length, width);
    //printf("Step 2: Matrix after transposing and reshaping columns:\n");
    //printMatrix(mat, length, width);

    // step 3: sort all columns
    sortColumns(mat, length, width);
    //printf("Step 3: Matrix after sorting columns:\n");
    //printMatrix(mat, length, width);

    // step 4: reshape and transpose
    mat = reshape(mat, width, length);
    mat = transpose(mat, width, length);
    //printf("Step 4: Matrix after reshaping and transposing columns:\n");
    //printMatrix(mat, length, width);

    // step 5: sort all columns
    sortColumns(mat, length, width);
    //printf("Step 5: Matrix after sorting columns:\n");
    //printMatrix(mat, length, width);

    // step 6: shift down the matrix
    mat = shiftDown(mat, length, width);
    //printf("Step 6: Matrix after shifting down:\n");
    //printMatrix(mat, length, width+1);

    // step 7: sort all columns
    sortColumns(mat, length, width+1);
    //printf("Step 7: Matrix after sorting columns:\n");
    //printMatrix(mat, length, width+1);

    // step 8: shift up the matrix
    mat = shiftUp(mat, length, width);
    //printf("Step 8: Matrix after shifting down:\n");
    //printMatrix(mat, length, width);

    gettimeofday(&stop, NULL);
    *elapsedTime = ((stop.tv_sec - start.tv_sec) * 1000000+(stop.tv_usec-start.tv_usec))/1000000.0;

    // use this to copy over everything back into A 
    int* ptr = &A[0];
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < length; i++) {
            *ptr = mat[i][j];
            ptr++;
        }
    }   

    free(mat);
}