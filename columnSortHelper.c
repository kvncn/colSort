/**
 * @author Kevin Cascais Nisterenko
 * @date March 8th, 2024
 * @class CSc 422, Spring 2024
 * 
 * Helper functions for columnSort, implementation of the columSortHelper 
 * interface. 
 */
#include "columnSortHelper.h"

// ---- sorting

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

int** transpose(int** matrix, int len, int width) {
    int** tr = malloc(width * sizeof(int*));

    for (int i = 0; i < width; i++)
        tr[i] = malloc(len * sizeof(int)); 
    
    // transpose, what is a column of the original is a row of the 
    // result
    for (int i = 0; i < len; i++)
        for (int j = 0; j < width; j++)
            tr[j][i] = matrix[i][j];
    
    freeMatrix(matrix, len, width);

    return tr; 

}

int** reshape(int** matrix, int len, int width) {
    int** res = malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        res[i] = malloc(width * sizeof(int)); 

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

int** shiftDown(int** matrix, int len, int width) {
    int* temp = malloc(len * (width+1) * sizeof(int));

    int curr = 0;

    // set the first len/2 elements to -1 (negative infinity)
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
    int i = 0;
    while (i < len / 2) {
        temp[curr] = 1000;
        curr++;
        i++;
    }

    curr = 0;
    
    // allocate space for the shifted matrix
    int** shifted = malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        shifted[i] = malloc((width+1) * sizeof(int)); 
    
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

int** shiftUp(int** matrix, int len, int width) {
    int* temp = malloc(len * (width+1) * sizeof(int));

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
    int** shifted = malloc(len * sizeof(int*));

    for (int i = 0; i < len; i++)
        shifted[i] = malloc(width * sizeof(int)); 

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

void freeMatrix(int** matrix, int len, int width) { 
    for (int i = 0; i < len; i++)
        free(matrix[i]);
    
    free(matrix);
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

