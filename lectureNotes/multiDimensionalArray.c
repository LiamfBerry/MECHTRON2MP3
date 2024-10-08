#include <stdio.h>

void printArrayElement(int rows, int cols, int arr[rows][cols]) {
    for (int i=0; i < rows; i++) {
        for (int j=0; j<cols;j++) {
            printf("Element [%d][%d]: Value = %d, Address = %p\n",i,j,arr[i][j], &arr[i][j]);
        }
    }
}

int main() {
    int array[][3] = {{10,20,30}, {40,50,60}, {70,80,90}};
    int rows = sizeof(array) / sizeof(array[0]);
    int cols = sizeof(array[0]) / sizeof(array[0][0]);
    printArrayElement(rows,cols,array);
}