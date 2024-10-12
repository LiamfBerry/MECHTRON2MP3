// CODE: include necessary library(s)
#include <stdlib.h>
#include <stdio.h>
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
    for (int i = 1; i < n; i++) { //checks element
        int j = i-1; //previous element 
        int check = arr[i];
        while (j>=0 && arr[j] > check){ 
            //Move arr[j] up an index then check again 
            arr[j+1]=arr[j];
            j--; //Ensures j checks each element previous to it and no elements are lost
        }
        arr[j+1] = check; //Update checked element to next
    }
}

void merge(int arr[], int l, int m, int r) {

    int sizeLeft = m-l+1;
    int sizeRight = r-m;
    int arrayLeft[sizeLeft], arrayRight[sizeRight]; //split arrays

    //fills arrays which respective haves of each list 
    for (int i = 0; i < sizeLeft; i++) {
        arrayLeft[i] = arr[l + i];
    }
    for (int j = 0; j < sizeRight; j++) {
        arrayRight[j] = arr[m+1+j];
    }
    
    //Merge them back into the main array as being sorted
    int i = 0;
    int j = 0;
    int k = l; //sorted iterative should start at left endpoint index so its position is known
    while (i < sizeLeft && j < sizeRight) {
        if(arrayLeft[i] <= arrayRight[j]) {
            arr[k] = arrayLeft[i]; //If left value is smaller populate list with the smaller value first
            i++;
        }
        else {
            arr[k] = arrayRight[j]; //Right array has smaller value then populate 
            j++;
        }
        k++;
    }

    //on last iteration of previous while loop one element will be left behind so we assume that is the greatest value and populate it to the final index
    while ( i < sizeLeft) {
        arr[k] = arrayLeft[i];
        i++;
        k++;
    }
    while (j < sizeRight) {
        arr[k] = arrayRight[j];
        j++;
        k++;
    }

}

void mergeSort(int arr[], int l, int r) {

    //If lists are larger than size 1 we split the index again and sort
    if (l<r) { 

        int m = (l+r)/2; //middle index

        mergeSort(arr, l, m);
        mergeSort(arr, m+1,r);

        merge(arr,l,m,r);
    }

}

void maxHeap(int arr[], int n, int i) {

    int max = i; //max is current node

    //defines children of node in heap
    int leftChild = 2*i + 1; 
    int rightChild = 2*i + 2;

    //Checks if children are greater than root then updates max
    if (leftChild < n && arr[leftChild] > arr[max]) {
        max = leftChild;
    }
    if (rightChild < n && arr[rightChild] > arr[max]) {
        max = rightChild;
    }

    //If max is not root index make it root
    if (max != i) {
        swap(&arr[max], &arr[i]);

        //Repeat for each node and its children until max heap is created
        maxHeap(arr,n,max);
    }
}

void heapSort(int arr[], int n) {

    //Builds max heap
    for (int i = n/2-1; i>=0; i--) {
        maxHeap(arr, n, i);
    }

    //smallest element to root
    for (int i = n-1; i>0; i--) {
        swap(&arr[0],&arr[i]);

        //Reheapifies reduced (unsorted heap)
        maxHeap(arr,i,0);
    }

}


void countingSort(int arr[], int n) {
    int max = arr[0];
    int min = arr[0];

    //Find max value and min value
    for (int i = 1; i<n; i++) {
        if (arr[i]>max) {
            max = arr[i];
        }
        if (arr[i]<min) {
            min = arr[i];
        }
    }

    //Create array of size max-min+1 (shifts negative numbers to 0 to handle negative indecies)
    int size = max-min+1;
    int* countArray = (int*)malloc(size*sizeof(int));
    if (countArray == NULL) {
        printf("Error");
        return;
    }
    for (int i=0;i<size;i++) {
        countArray[i] = 0;
    }

    //Assign each index in new array to instances of value in old array
    for (int i=0;i<n;i++) {
        countArray[arr[i]-min]++; //ensure no negative indexes are returned
    }

    //Take cumulative sum of all previous values in array and assign to index
    //Can start at one becuase index 0 is its own cumulative sum
    for (int i=1; i<size; i++) {
        countArray[i]+=countArray[i-1];
    }

    //Initialize an array to sort into
    int* sortedArray = (int*)malloc(n*sizeof(int));
    if (sortedArray == NULL) {
        printf("Error");
        return;
    }
    for (int i=0;i<n;i++) {
        sortedArray[i] = 0;
    }

    //Traverse input array from last element to preserve order then track that index to the 
    //cumulative sum and subtract 1 to find index of where value should be assigned
    for (int i=n-1; i>=0; i--) {
        sortedArray[countArray[arr[i]-min]-1]=arr[i];
        countArray[arr[i]-min]--;
    }

    //Replace origional array with sorted array
    for (int i=0; i<n; i++) {
        arr[i]=sortedArray[i];
    }

    //Free up memory 
    free(countArray);
    free(sortedArray);
}

// CODE: implement the algorithms for Insertion Sort, Merge Sort, Heap Sort, Counting Sort