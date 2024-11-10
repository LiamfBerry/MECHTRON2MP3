#include <stdio.h>
#include <stdlib.h>

void matrixMultiply(int m, int n, double A[m][n], double B[n][m], double **C) {
    for (int i=0; i<m; i++) {
        for(int j=0; j< m; j++) {
            C[i][j] = 0;
            for (int k=0; k<n; k++) {
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}

double **allocateMem(int m) {
    double **C = (double **)malloc(m*sizeof(double *));
    for (int i=0; i<m; i++) {
        C[i] = (double *)malloc(m*sizeof(double));
    }

    return C;
}

void printMatrix(int rows, int cols, double **C) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%8.4lf ", C[i][j]);
        }
        printf("\n");
    }
}


int main() {
    int m = 2, n = 3;
    double A[2][3] = {{1,2,3},{4,5,6}};
    double B[3][2] = {{7,8}, {9, 10}, {11, 12}};

    double **C = allocateMem(m);

    matrixMultiply(m,n,A,B,C);

    printMatrix(m, m, C);

    for (int i = 0; i < m; i++) {
        free(C[i]);
    }
    free(C);

    return 0;
}