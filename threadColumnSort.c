/**
 * @author Kevin Cascais Nisterenko
 * @date March 8th, 2024
 * @class CSc 422, Spring 2024
 * 
 * Parallelized program that performs columnSort on a given matrix. The function 
 * for columnSort and its helpers are present, and it is to be used/called by 
 * another program, which will pass a vector of integers and dimensions for the
 * matrix that will have the algorithm performed on. 
 */
#include <stdio.h> // mostly for testing and print function
#include <stdlib.h>
#include <sys/time.h> 
#include <pthread.h>
#include <math.h>

#include "columnSort.h"

// ---- global variables
int** matrix; 
int rows;
int cols;
int threadCount;
volatile int* arrive;
int active; 

// ---- matrix transformations

int** transpose(int** matrix, int len, int width);
int** reverseTranspose(int** matrix, int len, int width);
int** shiftDown(int** matrix, int len, int width);
int** shiftUp(int** matrix, int len, int width);

// ---- helpers

void printMatrix(int** matrix, int length, int width);
void freeMatrix(int** matrix, int len, int width);
void swap(int* a, int* b);

// ---- sorting functions

void sortColumn(int i);
void sortColumns(int width);

// ---- thread organization

void* sorter(void *arg);
void barrier(int i);


// ---- matrix transformations

/**
 * Tranformation function to transpose the matrix and reshape it to the 
 * original dimensions.
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return tr, 2d array of integers
 */
int** transpose(int** matrix, int len, int width) {
    int** tr = (int**) malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        tr[i] = (int*) malloc(width * sizeof(int)); 
    
    int row = 0;
    int col = 0;
    
    // transpose, what is a column of the original is a row of the 
    // result
    for (int j = 0; j < width; j++)
        for (int i = 0; i < len; i++) {
            tr[row][col] = matrix[i][j];
            col++;
            if (col == width) {
                col = 0;
                row++;
            } 
        }
    
    freeMatrix(matrix, len, width);

    return tr;
}

/**
 * Tranformation function to reverse the transpose of the matrix.  
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return res, 2d array of integers
 */
int** reverseTranspose(int** matrix, int len, int width) {
    int** res = (int**) malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        res[i] = (int*) malloc(width * sizeof(int)); 
    
    int row = 0;
    int col = 0;
    
    // untranspose, what is a column of the original is a row of the 
    // result
    for (int i = 0; i < len; i++)
        for (int j = 0; j < width; j++) {
            res[row][col] = matrix[i][j];
            row++;
            if (row == len) {
                row = 0;
                col++;
            } 
        }
    
    freeMatrix(matrix, len, width);

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
 * @param mat, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void printMatrix(int** mat, int len, int width) {
    for (int i = 0; i < len; i++) {
        printf("|");
        for (int j = 0; j < width; j++) {
            printf(" %4d ", mat[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

/**
 * Helper function to free the matrix in memory.
 * 
 * @param mat, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void freeMatrix(int** mat, int len, int width) { 
    for (int i = 0; i < len; i++)
        free(mat[i]);
    
    free(mat);
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

// ---- thread organization

/**
 * Barrier implementation for a single thread (represented by i). It is a
 * dissemination barrier inspired by the class example we covered in lecture
 * and in the slides.
 * 
 * @param i, integer representing current thread
 */
void barrier(int i) {
    int waitFor;
    for (int j = 1; j <= (int) ceil(log2(threadCount)); j++) {
        // spin if we are not supposed to be here yet
        while (arrive[i] != 0);
        arrive[i] = j;
        waitFor = (int) (i + pow(2, j-1)) % threadCount;
        // wait for our budy
        while (arrive[waitFor] != j);
        // reset
        arrive[waitFor] = 0;
    }
}

/**
 * Worker that sorts a column of the matrix, it calls the sortColumn on the
 * column it posesses and then it waits for the other threads.
 * 
 * @param arg, void pointer with the arguments for the function
 * @return NULL, nothing is returned
 */
void* sorter(void *arg) {
    int id = *((int *) arg);

    // thread 0 will also be the coordinator, i.e. it will take care of
    // the matrix transformations 

    int numCols = cols / threadCount;
    int startCol = id * numCols;
    int endCol;

    if (id == threadCount - 1) {
        endCol = cols - 1;
    } else {
        endCol = startCol + numCols - 1;
    } 

    // step 1: sort all columns
    for (int i = startCol; i <= endCol; i++) 
        sortColumn(i);
    barrier(id);

    // step 2: transpose and reshape
    if (id == 0)
        matrix = transpose(matrix, rows, cols);
    barrier(id);

    // step 3: sort all columns
    for (int i = startCol; i <= endCol; i++) 
        sortColumn(i);
    barrier(id);

    // step 4: reshape and transpose
    if (id == 0)
        matrix = reverseTranspose(matrix, rows, cols);
    barrier(id);

    // step 5: sort all columns
    for (int i = startCol; i <= endCol; i++) 
        sortColumn(i);
    barrier(id);

    // step 6: shift down the matrix
    if (id == 0)
        matrix = shiftDown(matrix, rows, cols);
    barrier(id);

    // step 7: sort all columns
    for (int i = startCol; i <= endCol; i++) 
        sortColumn(i);
    sortColumn(cols);
    barrier(id);

    // step 8: shift up the matrix
    if (id == 0)
        matrix = shiftUp(matrix, rows, cols);
    barrier(id);

    return NULL;
}

// ---- sorting functions

/**
 * Sorts a column in a matrix, uses bubble sort to do so. It uses the 
 * global matrix so that it can work with the threads
 * 
 * @param i, integer representing the col to sort 
 */
void sortColumn(int i) {
    // performs bubble sort in a single column
        // here we are just doing bubble sort
        for (int j = 0; j < rows - 1; j++) {
            for (int k = 0; k < rows - j - 1; k++) {
                if (matrix[k][i] > matrix[k + 1][i])
                    swap(&matrix[k][i], &matrix[k + 1][i]);
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
 * @param numThreads, integer representing the numbr of threads
 * @param length, number of rows of the matrix
 * @param width, number of cols of the matrix
 * @param elapsedTime, double pointer where the time is stored
 */
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime) {
    // allocate matrix
    matrix = (int**) malloc (length * sizeof(int*));

    for (int i = 0; i < length; i++)
        matrix[i] = (int*) malloc(width * sizeof(int));
     
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j] = A[i * width + j];
        }
    }

    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // allocate global variables for the threads
    rows = length;
    cols = width; 
    threadCount = numThreads;
    active = 1;
    arrive = (int*) calloc(threadCount, sizeof(int));

    // thread vars
    int* params;
    pthread_t* threads;

    // allocate thread handles
    threads = (pthread_t*) malloc(numThreads * sizeof(pthread_t));
    params = (int*) malloc(numThreads * sizeof(int));

    // start sorting
    for (int i = 0; i < numThreads; i++) {
        params[i] = i;
        pthread_create(&threads[i], NULL, sorter, (void *)(&params[i]));
    }

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);

    free(threads);
    free(params);

    gettimeofday(&stop, NULL);
    *elapsedTime = ((stop.tv_sec - start.tv_sec) * 1000000+(stop.tv_usec-start.tv_usec))/1000000.0;

    // use this to copy over everything back into A 
    int* ptr = &A[0];
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < length; i++) {
            *ptr = matrix[i][j];
            ptr++;
        }
    }   

    free(matrix);
}