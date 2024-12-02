#ifndef UTILITY_H
#define UTILITY_H

// Function pointer type for objective functions
typedef double (*ObjectiveFunction)(int, double *);

typedef struct Bound{
    double lowerBound;
    double upperBound;
}Bound;


// Function prototypes
double random_double(double min, double max);
int nth_prime_approx(int dimension);
double halton_sequence(double min, double max, int index, int dimension);
double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double best_position[]);

// CODE: declare other functions and structures if necessary
double nelder_mead(ObjectiveFunction objective_function, double **x_local_best, int NUM_VARIABLES, int MAX_ITERATIONS, Bound *bounds);
void swap(double *x, double *y);
void swap_simplex_rows(double *row1, double *row2, int NUM_VARIABLES);
void maxHeap(double *f, double **simplex, int size, int i, int NUM_VARIABLES);
void heapSort(double *f, double **simplex, int size, int NUM_VARIABLES);
double euclidean_distance(double *x, double *g, int NUM_VARIABLES);
#endif // UTILITY_H
