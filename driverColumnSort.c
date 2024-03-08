#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "columnSort.h"

static void initData(int *A, int n) {
  int i;

  for (i = 0; i < n; i++) {
    A[i] = rand() % 1000;
  }
}

static void swap(int *x, int *y)
{ 
  int temp = *x;
  *x = *y;
  *y = temp;
}

static void bubbleSort(int *A, int n)
{
  int i, j;
  for (i = 0; i < n-1; i++)  {
    for (j = 0; j < n-i-1; j++) {
      if (A[j] > A[j+1])
          swap(&A[j], &A[j+1]);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, n, r, s, numWorkers;
  int *inputArray, *sortedArray;
  double elapsedTime;

  // read command line; note for sequential version numWorkers will be 1
  n = atoi(argv[1]);
  numWorkers = atoi(argv[2]);

  /* figure out r and s such that r and s are as close as possible satisfying columnsort constraints */
  i = 1;
  s = 0;
  while (i <= (int) sqrt(n/2)) {
    if (n % i == 0)
      s = i;
    i++;
  }

  r = n/s;

  while (r < 2 * pow(s - 1, 2)) {
    r = r * 2;
    s = s / 2;
  }

  inputArray = (int *) malloc (n * sizeof(int));
  sortedArray = (int *) malloc (n * sizeof(int));

  initData(inputArray, n);
  for (i = 0; i < n; i++) {
    sortedArray[i] = inputArray[i];
  }
  bubbleSort(sortedArray, n);

  // this is the function you must implement
  columnSort(inputArray, numWorkers, r, s, &elapsedTime);

  // just error checking here
  for (i = 0; i < n; i++) {
    if (sortedArray[i] != inputArray[i]) {
      printf("error at position %d; correct value is %d; your value is %d\n", i, sortedArray[i], inputArray[i]);
      exit(1);
    }
  }

  printf("correct\n");
  printf("elapsedTime is %.3f\n", elapsedTime);
}
