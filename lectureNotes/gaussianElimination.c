#include <stdio.h>
#include <stdlib.h>

void gaussianElimination(double **A, double *b, double *x, int n) {

    double factor;

    //Forward elimination
    for (int i=0;i<n-1;i++) {
        if (A[i][i]==0) {
            printf("Division by zero");
            exit(1);
        }

        //Make zero for the coeffs for the same column but different rows
        for (int j=i+1;j<n;j++) {
            factor=-A[j][i]/A[i][i];

            //Update all columns in matrix A for the same row
            for (int k=0;k<n;k++) {
                A[j][k]+=factor*A[i][k];
            }
            //Update b as well
            b[j]+=factor*b[i];
        }

       
    }

    //Backward subsitution
    for (int i = n-1; i>=0; i--) {
        x[i]=b[i];

        for (int j=i+1; j<n;j++)  {
            x[i] -= A[i][j]*x[j];
        }

        x[i] = x[i]/A[i][i];
    } 

}

void printSystem(double **A, double *b, int n) {
    printf("System (A|b): \n");
    for (int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            printf("%8.4lf ", A[i][j]);
        }
        printf("| %8.4lf]\n", b[i]);
    }
}

int main() {

    int n=3;
    double inputA[3][3] = {{2,1,-1},{-3,-1,2}, {-2,1,2}};
    double inputb[3] = {8,-11,-3};

    double **A = (double**)malloc(n*sizeof(double*));
    for (int i = 0; i<n; i++) {
        A[i] = malloc(n*sizeof(double));
    }

    double *b = (double*)malloc(n*sizeof(double));
    double *x = (double*)malloc(n*sizeof(double));

    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            A[i][j] = inputA[i][j];
        }
    }

    for (int i=0; i<n; i++) {
        b[i] = inputb[i];
    }

    printSystem(A,b,n);

    gaussianElimination(A,b,x,n);

    printSystem(A,b,n);

    for (int i=0; i<n; i++) {
        printf("x[%d] = %8.4lf\n",i,x[i]);
    }

    for (int i=0; i<n; i++) {
        free(A[i]);
    }
    free(A);

    free(b);
    free(x);

}