#include <stdio.h>
#include <omp.h>
#include <time.h>


int parallel_sum(int partial_sum, int total_sum) {

    #pragma omp parallel private(partial_sum) shared(total_sum)
    {
        partial_sum = 0;
        total_sum = 0;

        #pragma omp for
        for (int i = 1; i <= 100000000; i++) {
            partial_sum += i;
        }
        printf("partial sum: %d\n", partial_sum);

        //Thread safe region
        #pragma omp critical 
        {
            total_sum += partial_sum;
        }
    }
    return total_sum;
}

int serial_sum(int total_sum) {
    for (int i = 1; i <= 100000000; i++) {
        total_sum += i;
    }
    return total_sum;
}

int main () {
    int partial_sum, total_sum;

    clock_t start, end;
    double cpu_time;

    start = clock();
    total_sum = serial_sum(0);
    end = clock();
    cpu_time = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("Total Sum: %d\n", total_sum);
    printf("CPU time serial: %lf\n", cpu_time);

    start = clock();
    total_sum = parallel_sum(0, 0);
    end = clock();
    cpu_time = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("Total Sum: %d\n", total_sum);
    printf("CPU time parallel: %lf\n", cpu_time);

    return 0;
}