#include <stdio.h>
#include <stdlib.h>

double **jaggedArray(int rows, double *columns) {

    double **jaggedArray = (double **)malloc(rows*sizeof(double *));
    if (jaggedArray == NULL) {
        printf("Memory allocation failed");
        return NULL;
    }
    for (int  i = 0; i < rows; i++) {
        jaggedArray[i] = (double *)malloc(columns[i]*sizeof(double));
        if (jaggedArray[i] == NULL) {
            printf("Memory allocation failed");

            //Free all successfully allocated memory
            for (int j = 0; j < i; j++) {
                free(jaggedArray[j]);
            }
            free(jaggedArray);
            return NULL;
        }
    }

    return jaggedArray;
}

void freeJaggedArray(int rows, double **jaggedArray) {
    for (int i = 0; i < rows; i++) {
        free(jaggedArray[i]);
    }
    free(jaggedArray);
}

int main() {

    int rows = 3;
    double columns[3] = {3, 5, 2}; //Num columns in each row

    double **array = jaggedArray(rows, columns);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns[i]; j++) {
            array[i][j] = i * 10 + j; // Example initialization
        }
    }

    // Print the jagged array
    printf("Jagged Array:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns[i]; j++) {
            printf("%.2f ", array[i][j]);
        }
        printf("\n");
    }

    // Free the jagged array
    freeJaggedArray(rows, array);


}