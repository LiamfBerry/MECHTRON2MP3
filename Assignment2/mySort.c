// CODE: include necessary library(s)
// you have to write all the functions and algorithms from scratch,
// You will submit this file, mySort.c holds the actual implementation of sorting functions


void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// Bubble Sort
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
        }
    }
}

void insertionSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) { //checks element
        for (int j = i-1; j >= 0; j--) { 
            if (arr[i] < arr[j]) { //compares to sorted list 
                swap(&arr[i], &arr[j]);
            }
        }
    }
}

/* 
//Fix: Runaway code + needs to dereference pointer to array in each 
//sorted element and reassign. Do this when merging lists with swap
//need to redefine function to use l and r

void mergeSort(int arr[], int n) {

    int split = n/2;
    int arrayLeft[split], arrayRight[n-split]; //split arrays
    while (n > 1) {
        

        //fills arrays which respective haves of each list 
        for (int i = 0; i < split; i++) {
            arrayLeft[i] = arr[i];
        }
        for (int j = n-split-1; j < n-split; j++) {
            arrayRight[j] = arr[j];
        }
        mergeSort(arrayLeft, split);
        mergeSort(arrayRight, n-split);
    }
    int i = 0, j = 0, k = 0;
    while (i < split && j < n-split) {
        if (arrayLeft[i] <= arrayRight[j]) {
            arr[k] = arrayLeft[i];
            i++;
        }
        else {
            arr[k] = arrayRight[j];
            j++;
        }
        k++;
    }
}
*/

void heapSort(int arr[], int n) {

}

// CODE: implement the algorithms for Insertion Sort, Merge Sort, Heap Sort, Counting Sort