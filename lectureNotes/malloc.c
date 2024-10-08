#include <stdio.h>
#include <stdlib.h>

int main() {
    int n=10;
    
    int *arr=malloc(n*sizeof(int)); //Assign pointer array to dynamically alocate memory n elements

    if (arr == NULL) {
        printf("Memory allocation failed. Exiting the program. \n");
        return 1; //Documentation say if it returns 1 then Error
    }


    printf("Number of elements: %d\n", n);
    printf("Size of each element: %zu\n", sizeof(*arr));

    free(arr); //free up memory after it's done being used

    return 0;
}