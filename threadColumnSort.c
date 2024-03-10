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
#include "columnSortHelper.h"

// ---- global variables
int** matrix; 
int rows;
int cols;
int threadCount;
volatile int* arrive;
int active; 

// ---- sorting functions

void sortColumn(int i);
void sortColumns(int width);

// ---- thread organization

void* sorter(void *arg);
void barrier(int i);

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
        while (arrive[i] != 0) {
            printf("waiting for my arrive to be 0\n");
        }
        arrive[i] = j;
        waitFor = (int) (i + pow(2, j-1)) % threadCount;
        // wait for our budy
        while (arrive[waitFor] != j) {
            printf("waiting for my budy to be ready\n");
        }
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

    int numCols = cols / threadCount;
    int startCol = id * numCols;
    int endCol;

    if (id == threadCount - 1) {
        endCol = cols - 1;
    }
    else {
        endCol = startCol + numCols - 1;
    } 

    for (int i = startCol; i <= endCol; i++) 
        sortColumn(i);

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

    // allocate global variables for the threads
    matrix = mat; 
    rows = length;
    cols = width; 
    threadCount = numThreads;
    active = 1;

    arrive = (int*) calloc(threadCount, sizeof(int));

    // thread vars
    int *params;
    pthread_t *threads;

    // allocate thread handles
    threads = (pthread_t *) malloc(numThreads * sizeof(pthread_t));
    params = (int *) malloc(numThreads * sizeof(int));

    //printf("Original Matrix:\n");
    //printMatrix(matrix, length, width);

    // step 1: sort all columns
    for (int i = 0; i < numThreads; i++) {
        params[i] = i;
        pthread_create(&threads[i], NULL, sorter, (void *)(&params[i]));
    }

    //sortColumns(width);

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);


    // stop threads from trying to work

   //printf("Step 1: Matrix after sorting columns:\n");
    //printMatrix(matrix, length, width);

    // step 2: transpose and reshape
    matrix = transpose(matrix, length, width);
    matrix = reshape(matrix, length, width);
    //printf("Step 2: Matrix after transposing and reshaping columns:\n");
    //printMatrix(matrix, length, width);

    // step 3: sort all columns
    for (int i = 0; i < numThreads; i++) {
        params[i] = i;
        pthread_create(&threads[i], NULL, sorter, (void *)(&params[i]));
    }

    //sortColumns(width);

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);
    
    //printf("Step 3: Matrix after sorting columns:\n");
    //printMatrix(matrix, length, width);

    // step 4: reshape and transpose
    matrix = reshape(matrix, width, length);
    matrix = transpose(matrix, width, length);
    //printf("Step 4: Matrix after reshaping and transposing columns:\n");
    //printMatrix(matrix, length, width);


    // step 5: sort all columns
    for (int i = 0; i < numThreads; i++) {
        params[i] = i;
        pthread_create(&threads[i], NULL, sorter, (void *)(&params[i]));
    }

    //sortColumns(width);

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);
    
    // printf("Step 5: Matrix after sorting columns:\n");
    // printMatrix(matrix, length, width);

    // step 6: shift down the matrix
    matrix = shiftDown(matrix, length, width);
    printf("Step 6: Matrix after shifting down:\n");
    printMatrix(matrix, length, width+1);

    // step 7: sort all columns
    cols = width + 1;
    for (int i = 0; i < numThreads; i++) {
        params[i] = i;
        pthread_create(&threads[i], NULL, sorter, (void *)(&params[i]));
    }

    //sortColumns(width);

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);

    // fix up last col
    cols = width;
    sorter((void*) &cols);
    printf("Step 7: Matrix after sorting columns:\n");
    printMatrix(matrix, length, width+1);

    // step 8: shift up the matrix
    matrix = shiftUp(matrix, length, width);
    printf("Step 8: Matrix after shifting down:\n");
    printMatrix(matrix, length, width);

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
