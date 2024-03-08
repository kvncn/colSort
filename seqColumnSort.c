#include <stdio.h> // mostly for testing rn
#include <stdlib.h>

#include "columnSort.h"

// ---- matrix transformations
void transposeAndReshape(int** matrix, int len, int width);
void reshapeAndTranspose(int** matrix, int len, int width);
void shiftDown(int** matrix, int len, int width);
void shiftUp(int** matrix, int len, int width);

// ---- sorting
void sortColumns(int** matrix, int len, int width);
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime);


// ---- helpers
int** copyMatrix(int** matrix, int len, int width);
void printMatrix(int** matrix, int length, int width);
void swap(int* a, int* b);

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
int** copyMatrix(int** matrix, int len, int width) {
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

    return temp;
}

/**
 * 
 */
void shiftDown(int** matrix, int len, int width) {

    int* temp = malloc(len * (width+1) * sizeof(int));

    int curr = 0;
    
    while (curr < len / 2){
        temp[curr] = -1;
        curr++;
    }

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < width; j++) {
            temp[curr] = matrix[i][j];
            curr++;
        }
    }

    int i = 0;
    while (i < len / 2) {
        temp[curr] = 1000;
        curr++;
        i++;
    }

    for (int i = 0; i < len; i++)
        matrix[i] = realloc(matrix[i], (width+1) * sizeof(int));

    for (int j = 0; j < width+1; j++) {
        for (int i = 0; i < len; i++) {
            matrix[j][i] = temp[i * (width+1) + j];
        }
    }


}

/**
 * 
 */
void transposeAndReshape(int** matrix, int len, int width) {
    int** temp = copyMatrix(matrix, len, width);
    
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

/**
 * 
 */
void reshapeAndTranspose(int** matrix, int len, int width) {
    int** temp = copyMatrix(matrix, len, width);

    // similar operation here, but now the pointer that is going rowwise 
    // will be the one copied in the matrix that is being iterated columwise
    int* ptr = matrix[0];

    for (int j = 0; j < width; j++) {
        for (int i = 0; i < len; i++) {
            temp[i][j] = *ptr;
            ptr++;
        }
    }

    // for some reason it is only working with temp and not matrix, 
    // so just copy over
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            matrix[i][j] = temp[i][j];
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

/**
 * 
 */
void columnSort(int *A, int numThreads, int length, int width, double *elapsedTime) {
    // allocate matrix
    int** temp = malloc (length * sizeof(int*));

    for (int i = 0; i < length; i++)
        temp[i] = malloc(width * sizeof(int));
     
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            temp[i][j] = A[i * width + j];
        }
    }

    printf("Original Matrix:\n");
    printMatrix(temp, length, width);

    // step 1: sort all columns
    sortColumns(temp, length, width);
    printf("Step 1: Matrix after sorting columns:\n");
    printMatrix(temp, length, width);

    // step 2: transpose and reshape
    transposeAndReshape(temp, length, width);
    printf("Step 2: Matrix after transposing and reshaping columns:\n");
    printMatrix(temp, length, width);

    // step 3: sort all columns
    sortColumns(temp, length, width);
    printf("Step 3: Matrix after sorting columns:\n");
    printMatrix(temp, length, width);

    // step 4: reshape and transpose
    reshapeAndTranspose(temp, length, width);
    printf("Step 4: Matrix after reshaping and transposing columns:\n");
    printMatrix(temp, length, width);

    // step 5: sort all columns
    sortColumns(temp, length, width);
    printf("Step 5: Matrix after sorting columns:\n");
    printMatrix(temp, length, width);

    // step 6: shift down
    shiftDown(temp, length, width);
    printf("Step 6: Matrix after shifting down:\n");
    printMatrix(temp, length, width+1);

    // use this to copy over everything back into A 
    transposeAndReshape(temp, length, width);
    int* ptr = &A[0];
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            *ptr = temp[i][j];
            ptr++;
        }
    }   

    // free up memory
    free(temp);

}

