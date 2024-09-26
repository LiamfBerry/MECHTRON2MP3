#include <stdio.h>
#include <math.h>
#include <time.h>

int main() {

    double number = 0;

    printf("Number to take square root of: ");
    scanf("%lf", &number);

    clock_t start, end;

    start = clock();
    double x = sqrt(number);
    end = clock();
    printf("Square root is: %lf\n", x);
    printf("Time to run: %lf\n", (double)(end-start)/ CLOCKS_PER_SEC);

}