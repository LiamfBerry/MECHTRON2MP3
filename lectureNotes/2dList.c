#include <stdio.h>

void printArray(int arr[][5], int colSize, int rowSize) {
    for (int i = 0; i< rowSize; i++) {
        for (int j=0; j<colSize; j++) {
            printf("arr[%d][%d] = %d with address %p\n", i, j, arr[i][j], &arr[i][j]);
        }
    }
}

int main() {
    int arr[][5] = {{10,20,30,40,50}, {10,20,30,40,50}};

    printArray(arr, 5, 2);
}