#include <stdio.h>
#include <math.h>
#include <time.h>

int main() {

    clock_t start, end;

    start = clock();
    double x = sqrt(0.5);
    end = clock();
    printf("Square root is: %lf\n", x);
    printf("Time to run: %lf\n", (double)(end-start)/ CLOCKS_PER_SEC);

}