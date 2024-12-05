#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

// Function pointer type for objective functions
typedef double (*ObjectiveFunction)(int, double *);

typedef struct Bound{
    double lowerBound;
    double upperBound;
}Bound;

typedef struct Particle {
    double *x; //position
    double *v; //velocity
    double *p; //best position
    double fp_best; //personal best fitness

} Particle;


// Function prototypes
double random_double(double min, double max, unsigned int *seed);
uint64_t *n_primes(int dimension);
double halton_sequence(double min, double max, int index, uint64_t prime, unsigned int *seed);
double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double best_position[]);

// CODE: declare other functions and structures if necessary

#endif // UTILITY_H
