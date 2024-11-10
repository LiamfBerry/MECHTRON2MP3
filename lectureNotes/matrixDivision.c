#include <stdio.h>
#include <stdlib.h>

void gaussianElimination(int n, double A[n][n], double **inv_A) {
    int i, j, k;

    //Initialize with identity matrix 
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i==j) {
                    inv_A[i][j] = 1;
                }
            else {
                inv_A[i][j] = 0;
            }
        }
    }
                
    //Forward elimination
    for (i = 0; i < n-1; i++) {
        if (A[i][i]==0) {
            printf("Division by zero error\n");
            exit(1);
        }

        for (j = i+1; j < n; j++) {
            double factor = -A[j][i]/A[i][i];

            for (k = 0; k < n; k++) {

                A[j][k] += factor*A[i][k];
                inv_A[j][k] += factor*inv_A[i][k];
            }
        }
    }

    //Backward elimination
    for (i = n-1; i > 0; i--) {
        if (A[i][i]==0) {
            printf("Division by zero error\n");
            exit(1);
        }
        for (j = i-1; j >= 0; j--) {
            double factor = -A[j][i]/A[i][i];
            
            A[j][i] += factor*A[i][i];

            for (k = n-1; k >= 0; k--) {
                inv_A[j][k] += factor*inv_A[i][k];
            }
            
        }
    }
    
    //Division by magnitude
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            inv_A[i][j] = inv_A[i][j]/A[i][i];
        }
    }
    
}

double *output(int n, double **inv_A, double b[n]) {
    double *x = (double *)malloc(n*sizeof(double));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            x[i] += inv_A[i][j]*b[j];
        }
    }
    return x;
}

double **allocateMem(int n) {
    double **inv_A = (double **)malloc(n*sizeof(double *));
    for (int i = 0; i < n; i++) {
        inv_A[i] = (double *)malloc(n*sizeof(double));
    }
    return inv_A;
}

void freeArray(double **inv_A, int n) {
    for (int i = 0; i < n; i++) {
        free(inv_A[i]);
    }
    free(inv_A);
}

void printSystem(int n, double A[n][n], double **inv_A) {
    printf("System (A|inv_A): \n");
    for (int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            printf("%8.4lf ", A[i][j]);
        }
        printf("|");
        for(int k=0;k<n;k++) {
            printf(" %8.4lf", inv_A[i][k]);
        }
        printf("\n");
        
    }
}

int main() {

    int n = 3;
    
    double A[3][3] = {
        {2,1,-1},
        {-3,-1,2},
        {-2,1,2}
    };

    double b[3] = {8, -11, -3};

    double **inv_A = allocateMem(n);

    printSystem(n,A,inv_A);

    gaussianElimination(n,A,inv_A);

    printSystem(n,A,inv_A);

    double *x = output(n, inv_A, b);

    for (int i = 0; i < n; i++) {
        printf("%lf\n", x[i]);
    }

    freeArray(inv_A, n);

}