/**
 * @author Kevin Cascais Nisterenko
 * @date March 8th, 2024
 * @class CSc 422, Spring 2024
 * 
 * Interface file for the helper functions for columnSort.
 */

#include <stdio.h> // mostly for testing and print function
#include <stdlib.h>

// ---- sorting


// ---- matrix transformations

/**
 * Tranformation function to transpose the matrix.  
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return shifted, 2d array of integers
 */
int** transpose(int** matrix, int len, int width);

/**
 * Tranformation function to reshape the given matrix to whatever dimensions
 * was passed to it. 
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 * @return res, 2d array of integers
 */
int** reshape(int** matrix, int len, int width);

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
int** shiftDown(int** matrix, int len, int width);

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
int** shiftUp(int** matrix, int len, int width);

// ---- helpers

/**
 * Helper function to print the matrix.
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void printMatrix(int** matrix, int length, int width);

/**
 * Helper function to free the matrix in memory.
 * 
 * @param matrix, 2d array of integers
 * @param len, integer representing number of rows 
 * @param width, integer representing number of cols 
 */
void freeMatrix(int** matrix, int len, int width);

/**
 * Helper function to swap two integer variables. 
 * 
 * @param a, pointer to integer a
 * @param b, pointer to integer b
 */
void swap(int* a, int* b);