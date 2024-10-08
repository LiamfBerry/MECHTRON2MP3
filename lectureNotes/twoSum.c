#include <stdio.h>

void twoSum(int arr[], int elements, int target) {

    for (int i=0; i<elements;i++) {
        for (int j=i+1; j<elements;j++) {
            if (arr[i] + arr[j] == target) {
                printf("[%d,%d]\n", i,j);
                return;
            }
        }
    }

}

int main() {
    int array[] = {2,7,11,15};
    int target = 13;
    int elements = sizeof(array)/sizeof(array[0]);
    twoSum(array, elements, target);
    return 0;
}