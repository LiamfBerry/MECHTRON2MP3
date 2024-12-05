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

//Need to add logic to handle imperfect squares -------------------------------------------------------------------------------------------------------------------------------------------------
void von_neumann_topology(Particle *particle, int index, double *von_neumann_best, int NUM_PARTICLES, int NUM_VARIABLES) {
    int matrix_size = ceil(sqrt(NUM_PARTICLES));

    int i = index / matrix_size; //Since integers truncate to zero this provides the row index
    int j = index % matrix_size; //The modulus of the index with the matrix size return the column as it is the remainder

    double f_von_neumann_best = INFINITY;

    //Define coordinates for neighbours in matrix using torus topology
    int neighbour_indicies[4][2] = {
        {(i - 1 + matrix_size) % matrix_size, j}, //Neighbour above
        {(i + 1) % matrix_size, j}, //Neighbour below
        {i , (j - 1 + matrix_size) % matrix_size}, //Neighour to the left
        {i , (j + 1) % matrix_size} //Neighbour to the right
    };

    //Check neighbours
    for (int k = 0; k < 4; k++) {
        int unit_row = neighbour_indicies[k][0];
        int unit_column = neighbour_indicies[k][1];

        //Translates 2d von-neuman topology back to the 1d array of particles
        int neighbour = unit_row * matrix_size + unit_column;

        if (particle[neighbour].fp_best < f_von_neumann_best && neighbour < NUM_PARTICLES) {
            f_von_neumann_best = particle[neighbour].fp_best;
            memcpy(von_neumann_best, particle[neighbour].p, sizeof(double) * NUM_VARIABLES);
        }
    }


}

// CODE: implement other functions here if necessary

double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double *best_position) {
    // CODE: implement pso function here
    Particle *particle = (Particle *)malloc(NUM_PARTICLES * sizeof(Particle));
    if (particle == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }
    
    clock_t start_cpu, end_cpu;
    double serial_cpu_time;
    time_t start_time, end_time;
    double parallel_cpu_time;

    //Seed for the random number generator to ensure the sequence of random numbers generated is different each time
    srand((unsigned int)time(NULL));


    //Inertia max weights not max values
    double w_max = 3.75, w_min = 0.1; 
    

    double w;
    //0.7298
    //Initialize constants
    double c1, c2, c1_max = 1.6, c2_max = 1.6, c1_min = 1.4, c2_min = 1.4;

    double alpha = 10; //Steepness coefficient
    double beta = 0.6; //weight of transition
    double g_weight = 0;

    double epsilon = 1e-6;
    int stagnated = 0;
    int break_count = 0;

    int max_stagnation = 1000;
    int break_threshold = 100;

    start_cpu = clock();
    time(&start_time);
    //Allocate memory for particles and dimensions
    //We do this as a single block to avoid memory allocation locks
    //We can then use pointer arithmatic to correctly alter values
    double *x_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *v_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *p_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    

    //Global variables
    double *g = calloc(NUM_VARIABLES, sizeof(double));
    double fg_best = INFINITY;


    double prev_fg_best = INFINITY;

    int *primes = malloc(NUM_VARIABLES * sizeof(int));

    if (x_data == NULL || v_data == NULL || p_data == NULL || g == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Precompute n primes based on dimension
    for (int i = 0; i < NUM_VARIABLES; i++) {
        primes[i] = nth_prime_approx(i + 1);
    }

    //Sets a pointer to the memory address of the first coordinate in each particles position velocity and best position
    //This ensures that each particles coordinates are stored properly 
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particle[i].x = &x_data[i * NUM_VARIABLES];
        particle[i].v = &p_data[i * NUM_VARIABLES];
        particle[i].p = &v_data[i * NUM_VARIABLES];
        particle[i].fp_best = 0.0;
    }

    //Use half the avaialable cores so CPU isn't completely used up by process
    int num_cores = omp_get_num_procs() / 2;
    omp_set_num_threads(num_cores);

    //Initialize parallelization by creating an array of local best values based on number of threads
    double fg_best_local[omp_get_max_threads()];
    //Same for position but 2d array for each coordinate
    double **g_local = malloc(omp_get_max_threads() * sizeof(double *));
    if (g_local == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < num_cores; i++) {
        g_local[i] = malloc(NUM_VARIABLES * sizeof(double));
    }


   

    printf("Number of cores used %d\n", num_cores);
    
    //Initialization loop
    #pragma omp parallel
    {
        //Local/Core specific variables
        int thread_id = omp_get_thread_num();
        fg_best_local[thread_id] = INFINITY;
        unsigned int seed = time(NULL) + thread_id; //Different seed for each thread

        //schedule dynamic distributed work more evenly reduction ensures the best fg value is found safely between threads
        //Each thread is given 10 iterations of the loop at once
        #pragma omp for schedule(static) reduction(min:fg_best)

        for (int i = 0; i < NUM_PARTICLES; i++) {

            //Initialize particle positions with random positions
            for (int j = 0; j < NUM_VARIABLES; j++) {

                //Values of pointers set to particle initial positions 
                particle[i].x[j] = halton_sequence(bounds[j].lowerBound, bounds[j].upperBound, i * NUM_VARIABLES + j, primes[j]);
                //random_double(bounds[j].lowerBound, bounds[j].upperBound, &seed);//
                particle[i].p[j] = particle[i].x[j]; //Particles best known position is initial position

                particle[i].v[j] = random_double(-1, 1, &seed); //Small initial velocity based off difference of bounds
            }

            //Intialize best fitness evaluted from sets of decision variables 
            particle[i].fp_best = objective_function(NUM_VARIABLES, particle[i].x);

            if (particle[i].fp_best < fg_best_local[thread_id]) {
                //update global best fitness for set 
                fg_best_local[thread_id] = particle[i].fp_best;
                for (int k = 0; k < NUM_VARIABLES; k++) {
                    //Update global best positions
                    g_local[thread_id][k] = particle[i].p[k];
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

        //Calculate transition weight for adaptive topology using a sigmoid function
        g_weight = 1/(1+exp(-alpha*((double)iter/MAX_ITERATIONS - beta)));

        //Make inertia dynamic, starts small and gets larger on a modified sigmoid function to encourage looking in starting locations more before venturing
        //During exploitation phase it gets very aggressive
        w = w_max*2.3*w_min/(1+exp(-alpha*((double)iter/MAX_ITERATIONS - beta)))+w_min;
        //c1 (cognitive coefficient) represents how confident a particle is in its own performance
        //c2 (social coefficient) represents how confident a particle is in its neighbours performace
        //As the algorithim is optimized c1 should start higher while c2 should start lower and they should converge
        //We will accomplish this with the sigmoid easing function
        c1 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((c1_max - c1_min) * pow(iter, 3)) + 3 * (c1_min - c1_max) * pow(iter, 2)) * g_weight + c1_max;
        c2 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((-c2_max + c2_min) * pow(iter, 3)) + 3 * (-c2_min + c2_max) * pow(iter, 2)) * g_weight + c2_min;
        #pragma omp parallel
        {
            // Private/thread specific variables initialized 
            int thread_id = omp_get_thread_num();
            unsigned int seed = time(NULL) + thread_id; //Different seed for each thread
            double f = INFINITY;

            #pragma omp for schedule(static) reduction(min:fg_best)

            for (int i = 0; i < NUM_PARTICLES; i++) {

                double von_neumann_best[NUM_VARIABLES];
                double weighted_best[NUM_VARIABLES];
             
                von_neumann_topology(particle, i, von_neumann_best, NUM_PARTICLES, NUM_VARIABLES);


                for (int j = 0; j < NUM_VARIABLES; j++) {

                    weighted_best[j] = g_weight * g_local[thread_id][j] + (1 - g_weight) * von_neumann_best[j];

                    //Create random doubles
                    double r1 = random_double(0, 1, &seed), r2 = random_double(0, 1, &seed);

                    //Update velocity
                    
                    //If velocity stagnates then reinitialize
                    if(fabs(particle[i].v[j]) < epsilon) {
                        particle[i].v[j] = random_double(-1, 1, &seed);
                    } 
                    else {
                        particle[i].v[j] = w * particle[i].v[j] + c1 * r1 * (particle[i].p[j] - particle[i].x[j]) + c2 * r2 * (weighted_best[j] - particle[i].x[j]);
                    }

                    //Update position
                    particle[i].x[j] += particle[i].v[j];
                
                    //Reflect particle position back into bounds instead of clamping
                    if (particle[i].x[j] < bounds[j].lowerBound) {
                        particle[i].x[j] = bounds[j].lowerBound - particle[i].x[j] / 100;
                        particle[i].v[j] *= -1; //Reverse velocity to encourage more exploration
                    }
                    else if (particle[i].x[j] > bounds[j].upperBound) {
                        particle[i].x[j] = bounds[j].upperBound - particle[i].x[j] / 100;
                        particle[i].v[j] *= -1;
                    }

                }

                //Find fitness of new values
                f = objective_function(NUM_VARIABLES, particle[i].x);
        
                if (f < particle[i].fp_best) {
                    particle[i].fp_best = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        particle[i].p[k] = particle[i].x[k];
                    }
                }

                if (f < fg_best_local[thread_id]) {
                    fg_best_local[thread_id] = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        g_local[thread_id][k] = particle[i].x[k];
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

        //If there is no significant improvement for 1000 sequential iterations then create a break counter
        //If there are 50 sequential breaks then we can conclude the function will not improve further
        //This can also only occur after the halfway point to allow the weight to shift to global improvment verses exploration
        if(fabs(fg_best - prev_fg_best) < epsilon) {
            stagnated++;
            if (stagnated > max_stagnation) {
                break_count++;
                if (break_count > break_threshold && MAX_ITERATIONS/iter < 2) {
                    //printf("Early break on iteration %d\n", iter);
                    break;
                }
                stagnated = 0;
            }
        }
        else {
            //printf("stagnations at change: %d\n", stagnated);
            printf("fg_best: %lf\n", fg_best);
            break_count = 0;
            stagnated = 0;
        }

        //Set previous iteration fitness to current 
        prev_fg_best = fg_best;
        
        
        
    }
    memcpy(best_position, g, NUM_VARIABLES * sizeof(double));


    free(particle);
    free(x_data);
    free(v_data);
    free(p_data);
    
    free(g);
    for (int i = 0; i < num_cores; i++) {
        free(g_local[i]);
    }
    free(g_local);

    free(primes);
    
    return fg_best;
}