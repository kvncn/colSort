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

// ---- matrix transformations

int** transpose(int** matrix, int len, int width);
int** reshape(int** matrix, int len, int width);
int** shiftDown(int** matrix, int len, int width);
int** shiftUp(int** matrix, int len, int width);

// ---- helpers

void printMatrix(int** matrix, int length, int width);
void freeMatrix(int** matrix, int len, int width);
void swap(int* a, int* b);

// ---- matrix transformations

/**
 * Tranformation function to transpose the matrix.  
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return shifted, 2d array of integers
 */
int** transpose(int** matrix, int len, int width) {
    int** tr = (int**) malloc(width * sizeof(int*));

    for (int i = 0; i < width; i++)
        tr[i] = (int*) malloc(len * sizeof(int)); 
    
    // transpose, what is a column of the original is a row of the 
    // result
    for (int i = 0; i < len; i++)
        for (int j = 0; j < width; j++)
            tr[j][i] = matrix[i][j];
    
    freeMatrix(matrix, len, width);

    return tr; 

}

/**
 * Tranformation function to reshape the given matrix to whatever dimensions
 * was passed to it. 
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return res, 2d array of integers
 */
int** reshape(int** matrix, int len, int width) {
    int** res = (int**) malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        res[i] = (int*) malloc(width * sizeof(int)); 

    // calculate the inidices by  circling around and wrapping so we can 
    // traverse the columns of the matrix, what is a column for the new 
    // one is used as a row base of the old and vice versa
    for (int i = 0; i < len * width; i++) {
        int row = i / width; 
        int col = i % width; 
        res[row][col] = matrix[i / len][i % len];
    }

    freeMatrix(matrix, width, len);

    return res;
}

/**
 * Tranformation function to shift down the matrix for column sort, 
 * it adds infinities to the edges of the matrix and adds one more col
 * to it, which is then returned
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return shifted, 2d array of integers
 */
int** shiftDown(int** matrix, int len, int width) {
    int* temp = (int*) malloc(len * (width+1) * sizeof(int));

    int curr = 0;

    // set the first len/2 elements to -1 (negative infinity)
    // it works due to the fact that rand only gives non-neg values
    while (curr < len / 2){
        temp[curr] = -1;
        curr++;
    }

    // copy over middle, which is just our matrix colum-wise
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < len; i++) {
            temp[curr] = matrix[i][j];
            curr++;
        }
    }

    // set last len/2 elements as 1000 (infinity)
    // it works due to the mod 1000 in the driver
    int i = 0;
    while (i < len / 2) {
        temp[curr] = 1000;
        curr++;
        i++;
    }

    curr = 0;
    
    // allocate space for the shifted matrix
    int** shifted = (int**) malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        shifted[i] = (int*) malloc((width+1) * sizeof(int)); 
    
    // copy over the elements to the new matrix
    for (int j = 0; j < width + 1; j++) {
        for (int i = 0; i < len; i++) {
            shifted[i][j] = temp[curr];
            curr++;
        }
    }

    free(matrix);
    free(temp);

    return shifted; 
}

/**
 * Tranformation function to shift up the matrix for column sort, 
 * it removes the infinities from the edges of the matrix and reshapes 
 * it into a matrix with one less col, which is returned. 
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return shifted, 2d array of integers
 */
int** shiftUp(int** matrix, int len, int width) {
    int* temp = (int*) malloc(len * (width+1) * sizeof(int));

    int curr = 0;

    // copy matrix over into temp
    for (int j = 0; j < width+1; j++) {
        for (int i = 0; i < len; i++) {
            temp[curr] = matrix[i][j];
            curr++;
        }
    }

    // now we skip the elements we do not care about while building our new 
    // matrix, so we shift back up as we ignore the infinities/edges of the matrix
    curr = len / 2;

    // allocate space for the new matrix
    int** shifted = (int**) malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        shifted[i] = (int*) malloc(width * sizeof(int)); 

    // copy over the elements to the matrix
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < len; i++) {
            shifted[i][j] = temp[curr];
            curr++;
        }
    }

    return shifted;
}

// ---- helpers 

/**
 * Helper function to print the matrix.
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void printMatrix(int** matrix, int len, int width) {
    for (int i = 0; i < len; i++) {
        printf("|");
        for (int j = 0; j < width; j++) {
            printf(" %4d ", matrix[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

/**
 * Helper function to free the matrix in memory.
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void freeMatrix(int** matrix, int len, int width) { 
    for (int i = 0; i < len; i++)
        free(matrix[i]);
    
    free(matrix);
}

/**
 * Helper function to swap two integer variables. 
 * 
 * @param a, pointer to integer a
 * @param b, pointer to integer b
 */
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

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

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // step 1: sort all columns
    sortColumns(mat, length, width);

    // step 2: transpose and reshape
    mat = transpose(mat, length, width);
    mat = reshape(mat, length, width);

    // step 3: sort all columns
    sortColumns(mat, length, width);

    // step 4: reshape and transpose
    mat = reshape(mat, width, length);
    mat = transpose(mat, width, length);

    // step 5: sort all columns
    sortColumns(mat, length, width);

    // step 6: shift down the matrix
    mat = shiftDown(mat, length, width);

    // step 7: sort all columns
    sortColumns(mat, length, width+1);

    // step 8: shift up the matrix
    mat = shiftUp(mat, length, width);

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