// CODE: include library(s)
#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <omp.h>

// Helper function to generate random numbers in a range
//Using seed and rand_r such that function is thread safe
double random_double(double min, double max, unsigned int *seed) {
    return min + (max - min) * ((double)rand_r(seed) / RAND_MAX);
}

int nth_prime_approx(int dimension) {
    if (dimension == 0) {
        printf("Can't compute 0th dimension\n");
        exit(1);
    }
    if (dimension < 6) {
        int small_primes[] = {2,3,5,7,11};
        return small_primes[dimension-1];
    }
    //Approximates nth prime number
    return dimension * log(dimension) + dimension * log(log(dimension));
}

//Helper function to generate seemingly randomly distirubted points for more coverage of space
//Precomputed primes to avoid bottle necking in parallelized initialization
double halton_sequence(double min, double max, int index, int prime) {
    double h = 1.0;
    double halton_value = 0.0;

    while (index > 0) {
        h /= prime;
        halton_value += h * (double)(index % prime);
        index /= prime;
    }
    return min + halton_value * (max - min);
}

// CODE: implement other functions here if necessary

double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double *best_position) {
    // CODE: implement pso function here
    clock_t start_cpu, end_cpu;
    double serial_cpu_time;
    time_t start_time, end_time;
    double parallel_cpu_time;

    //Seed for the random number generator to ensure the sequence of random numbers generated is different each time
    srand((unsigned int)time(NULL));


    //Inertia 
    double w_max = 0.9, w_min = 0.4, w;
    //Initialize constants
    double c1_max = 1.6, c2_max = 1.6, c1_min = 1.4, c2_min = 1.4, c1, c2;
    double v_max;

    //easing function parameters
    double sigmoid = 1;
    double alpha = -0.002;

    double epsilon = 1e-6;
    int stagnated = 0;
    int max_stagnation = 500;

    start_cpu = clock();
    time(&start_time);
    //Allocate memory for particles and dimensions
    //We do this as a single block to avoid memory allocation locks
    //We can then use pointer arithmatic to correctly alter values
    double *x_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *v_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *p_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));

    double *fp_best = calloc(NUM_PARTICLES, sizeof(double));

    //Global variables
    double *g = calloc(NUM_VARIABLES, sizeof(double));
    double fg_best = INFINITY;

    double prev_fg_best = INFINITY;

    int *primes = malloc(NUM_VARIABLES * sizeof(int));

    if (x_data == NULL || v_data == NULL || p_data == NULL || fp_best == NULL || g == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Precompute n primes based on dimension
    for (int i = 0; i < NUM_VARIABLES; i++) {
        primes[i] = nth_prime_approx(i + 1);
    }
    
    //Pointers for finding coordinates in 1d array set to the first coordinate of each particle
    double **x = malloc(NUM_PARTICLES * sizeof(double *));
    double **v = malloc(NUM_PARTICLES * sizeof(double *));
    double **p = malloc(NUM_PARTICLES * sizeof(double *));

    if (x == NULL || v == NULL || p == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Sets a pointer to the memory address of the first coordinate in each particles position velocity and best position
    //This ensures that each particles coordinates are stored properly 
    for (int i = 0; i < NUM_PARTICLES; i++) {
        x[i] = &x_data[i * NUM_VARIABLES];
        v[i] = &p_data[i * NUM_VARIABLES];
        p[i] = &v_data[i * NUM_VARIABLES];
    }

    //Initialize parallelization by creating an array of local best values based on number of threads
    double fg_best_local[omp_get_max_threads()];

    //Same for position but 2d array for each coordinate
    double g_local[omp_get_max_threads()][NUM_VARIABLES];
    
    //Initialization loop
    #pragma omp parallel
    {
        //Local/Core specific variables
        int thread_id = omp_get_thread_num();
        fg_best_local[thread_id] = INFINITY;
        unsigned int seed = time(NULL) + thread_id; //Different seed for each thread

        //schedule dynamic distributed work more evenly reduction ensures the best fg value is found safely between threads
        //Each thread is given 10 iterations of the loop at once
        #pragma omp for schedule(dynamic,10) reduction(min:fg_best)

        for (int i = 0; i < NUM_PARTICLES; i++) {

            //Initialize particle positions with random positions
            for (int j = 0; j < NUM_VARIABLES; j++) {

                //Values of pointers set to particle initial positions 
                x[i][j] = halton_sequence(bounds[j].lowerBound, bounds[j].upperBound, i * NUM_VARIABLES + j, primes[j]);
        
                p[i][j] = x[i][j]; //Particles best known position is initial position

                v[i][j] = random_double(-1, 1, &seed); //Small initial velocity based off difference of bounds
            }

            //Intialize best fitness evaluted from sets of decision variables 
            fp_best[i] = objective_function(NUM_VARIABLES, x[i]);

            if (fp_best[i] < fg_best_local[thread_id]) {
                //update global best fitness for set 
                fg_best_local[thread_id] = fp_best[i];
                for (int k = 0; k < NUM_VARIABLES; k++) {
                    //Update global best positions
                    g_local[thread_id][k] = p[i][k];
                }
            }           
        }    

      
    }
    for ( int thread = 0; thread < omp_get_max_threads(); thread++) {
        if (fg_best_local[thread] < fg_best) {
            fg_best = fg_best_local[thread];
            memcpy(g, g_local[thread], NUM_VARIABLES * sizeof(double));
        }
    }
    end_cpu = clock();
    time(&end_time);
    serial_cpu_time = ((double)(end_cpu - start_cpu))/CLOCKS_PER_SEC;
    parallel_cpu_time = difftime(end_time, start_time);
    printf("Initialized in %lf seconds serially and %lf seconds in real time from parallelization\n", serial_cpu_time, parallel_cpu_time);


    //PSO loop
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {

        //Make inertia dynamic, starts large and gets progressively smaller on an easing function
        w = 1/pow(MAX_ITERATIONS,2) * (2/MAX_ITERATIONS * ((w_max - w_min) * pow(iter, 3)) + 3 * (w_min - w_max) * pow(iter,2)) + w_max;

        //c1 (cognitive coefficient) represents how confident a particle is in its own performance
        //c2 (social coefficient) represents how confident a particle is in its neighbours performace
        //As the algorithim is optimized c1 should start higher while c2 should start lower and they should converge
        //We will accomplish this with the sigmoid easing function
        sigmoid = 1/(1+exp(alpha*(MAX_ITERATIONS - iter)));
        c1 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((c1_max - c1_min) * pow(iter, 3)) + 3 * (c1_min - c1_max) * pow(iter, 2)) * sigmoid + c1_max;
        c2 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((-c2_max + c2_min) * pow(iter, 3)) + 3 * (-c2_min + c2_max) * pow(iter, 2)) * sigmoid + c2_min;

        #pragma omp parallel
        {
            // Private/thread specific variables initialized 
            int thread_id = omp_get_thread_num();
            unsigned int seed = time(NULL) + thread_id; //Different seed for each thread
            double f = INFINITY;

            #pragma omp for
            for (int i = 0; i < NUM_PARTICLES; i++) {

                for (int j = 0; j < NUM_VARIABLES; j++) {

                    //Update velocity constraints based on bounds
                    v_max = 0.1 * (bounds[j].upperBound - bounds[j].lowerBound);

                    //Create random doubles
                    double r1 = random_double(0, 1, &seed), r2 = random_double(0, 1, &seed);

                    //Update velocity
                    
                    //If velocity stagnates then reinitialize
                    if(fabs(v[i][j]) < epsilon) {
                        v[i][j] = random_double(-1, 1, &seed);
                    } 
                    else {
                        v[i][j] = w * v[i][j] + c1 * r1 * (p[i][j] - x[i][j]) + c2 * r2 * (g_local[thread_id][j] - x[i][j]);
                    }

                    //Update position
                    x[i][j] = x[i][j] + v[i][j];
                
                    //Reflect particle position back into bounds instead of clamping
                    if (x[i][j] < bounds[j].lowerBound) {
                        x[i][j] = bounds[j].lowerBound - x[i][j] / 100;
                        v[i][j] *= -1; //Reverse velocity to encourage more exploration
                    }
                    else if (x[i][j] > bounds[j].upperBound) {
                        x[i][j] = bounds[j].upperBound - x[i][j] / 100;
                        v[i][j] *= -1;
                    }

                    //Clamp velocity to prevent particles from overshooting
                    if (fabs(v[i][j]) > v_max) {
                        if (v[i][j] >= 0) {
                            v[i][j] = v_max;
                        }
                        else {
                            v[i][j] = -v_max;
                        } 
                    }
                }

                //Find fitness of new values
                f = objective_function(NUM_VARIABLES, x[i]);
        
                if (f < fp_best[i]) {
                    fp_best[i] = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        p[i][k] = x[i][k];
                    }
                }

                if (f < fg_best_local[thread_id]) {
                    fg_best_local[thread_id] = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        g_local[thread_id][k] = x[i][k];
                    }
                }
            }

        }
        for (int thread = 0; thread < omp_get_max_threads(); thread++) {
            if (fg_best_local[thread] < fg_best) {
                fg_best = fg_best_local[thread];
                memcpy(g, g_local[thread], NUM_VARIABLES * sizeof(double));
            }
        }

        //If there is no significant improvement for 10 sequential iterations then break early
        if(fabs(fg_best - prev_fg_best) < epsilon) {
            stagnated++;
            if (stagnated > max_stagnation) {
                printf("Early break\n");
                break;
            }
        }
        else {
            printf("stagnations at change: %d\n", stagnated);
            printf("fg_best: %lf\n", fg_best);
            stagnated = 0;
        }

        //Set previous iteration fitness to current 
        prev_fg_best = fg_best;
        
        
    }
    memcpy(best_position, g, NUM_VARIABLES * sizeof(double));
    
    free(x_data);
    free(v_data);
    free(p_data);
    free(fp_best);
    free(g);
    free(primes);
    free(x);
    free(v);
    free(p);
    
    return fg_best;
}



//Implement adaptive topology that transitions from von-neuman topology to global topology
//Maintain parallelization while doing so
//Switch implimentation to structure format to control topology better