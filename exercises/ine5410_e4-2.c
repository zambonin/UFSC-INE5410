#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NRA 800 /* number of rows in matrix A */
#define NCA 800 /* number of columns in matrix A */
#define NCB 800 /* number of columns in matrix B */

void matrix_mult(void);

int main() {
  matrix_mult();

  return 0;
}

void matrix_mult() {
  int i, j, k;

  /* matrix A to be multiplied */
  double **a = (double **)malloc(sizeof(double *) * NRA);
  for (i = 0; i < NRA; i++) {
    a[i] = (double *)malloc(sizeof(double) * NCA);
  }

  /* matrix B to be multiplied */
  double **b = (double **)malloc(sizeof(double *) * NCA);
  for (i = 0; i < NCA; i++) {
    b[i] = (double *)malloc(sizeof(double) * NCB);
  }

  /* result matrix C */
  double **c = (double **)malloc(sizeof(double *) * NRA);
  for (i = 0; i < NRA; i++) {
    c[i] = (double *)malloc(sizeof(double) * NCB);
  }

  /* initialize matrices */
  for (i = 0; i < NRA; i++) {
    for (j = 0; j < NCA; j++) {
      a[i][j] = i + j;
    }
  }

  for (i = 0; i < NCA; i++) {
    for (j = 0; j < NCB; j++) {
      b[i][j] = i * j;
    }
  }

  for (i = 0; i < NRA; i++) {
    for (j = 0; j < NCB; j++) {
      c[i][j] = 0;
    }
  }

/* matrix-matrix multiplication */
#pragma omp parallel for private(i, j, k)
  for (i = 0; i < NRA; i++) {
    for (j = 0; j < NCB; j++) {
      for (k = 0; k < NCA; k++) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }

  /* print results */
  for (i = 0; i < NRA; i++) {
    for (j = 0; j < NCB; j++) {
      printf("%10.2f  ", c[i][j]);
    }
    printf("\n");
  }

  /* free memory */
  for (i = 0; i < NRA; i++) {
    free(a[i]);
  }
  free(a);

  for (i = 0; i < NCA; i++) {
    free(b[i]);
  }
  free(b);

  for (i = 0; i < NRA; i++) {
    free(c[i]);
  }
  free(c);
}
