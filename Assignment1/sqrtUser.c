#include <stdio.h>
#include <stdbool.h>
#include <time.h>

double sqrtUser(double number, int n) {

    double root = number/2;
    double precision = 0.9;

    if (n>=16) {
        printf("Precision is too high, choose something between 1-15\n");
        return 0;
    }

    for (int i=0; i<n; i++) {
        precision /= 10;
    }

    while (true) {
        
       if (root*root-number < precision && root*root-number >=0 || number==0) {
            break;
        }
        else if (number<0) {
            printf("enter a valid number\n");
            return 0;
        }
        root = root-(root*root-number)/(2*root);
    }  
    return root;
}

int main() {
    double number = 0;
    int decimals = 1;

    printf("Take the square root of: ");
    scanf("%lf", &number);
    printf("Accurate to how many decimals: ");
    scanf("%d", &decimals);

    clock_t start, end;

    start = clock();
    double result = sqrtUser(number,decimals);
    end = clock();
    printf("The square root is %0.*f\n", decimals, result);
    printf("Time to run %lf\n", (double)(end-start)/CLOCKS_PER_SEC);
}