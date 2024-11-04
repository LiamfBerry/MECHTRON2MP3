#include <stdio.h>
#include <stdlib.h>

void gaussianElimination(double **A, double *b, double *X, int n) {

    int i,j,k;
    double factor;

    //Forward elimination
    for (i=0;i<n-1;i++) {
        if (A[i][i]==0) {
            printf("Division by zero");
            exit(1);
        }

        //Make zero for the coeffs for the same column but different rows
        for (j=i+1;j<n;j++) {
            factor=-A[j][i]/A[i][i];

            //Update all columns in matrix A for the same row
            for (k=0;k<n;k++) {
                A[j][k]+=factor*A[i][k];
            }
            //Update b as well
            b[j]+=factor*b[i];
        }
    }
}

void printSystem(double **A, double *b, int n) {
    printf("[");
    for (int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            printf("%d ", A[i][j]);
        }
        printf("| %d]\n", b[i]);
    }
}

int main() {

}