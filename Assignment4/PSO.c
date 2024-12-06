// CODE: include library(s)
#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <primesieve.h>
#include <omp.h>

#include <stdint.h>

// Helper function to generate random numbers in a range
//Using seed and rand_r such that function is thread safe
double random_double(double min, double max, unsigned int *seed) {
    return min + (max - min) * ((double)rand_r(seed) / RAND_MAX);
}

uint64_t *n_primes(int dimension) {
    if (dimension == 0) {
        printf("Can't compute 0th dimension\n");
        exit(1);
    }
    uint64_t n = (uint64_t)dimension;
    uint64_t upper_bound = 1000;

    while (primesieve_count_primes(0, upper_bound) < n) {
        upper_bound *= 2;
    }
    
    uint64_t *primes = primesieve_generate_primes(0, upper_bound, &n, UINT64_PRIMES);
    if (primes == NULL) {
        printf("Allocation failed\n");
        exit(1);
    }

    return primes;
}

//Helper function to generate seemingly randomly distirubted points for more coverage of space
//Precomputed primes to avoid bottle necking in parallelized initialization
double halton_sequence(double min, double max, int index, uint64_t prime, unsigned int *seed) {
    double h = 1.0;
    double halton_value = 0.0;

    while (index > 0) {
        h /= prime;
        halton_value += h * (double)(index % prime);
        index /= prime;
    }
    double position =  min + halton_value * (max - min);

    //Add randomness to initilization
    double purturbtion = (2 * (double)rand_r(seed) / RAND_MAX - 1) * 0.01;
    position += purturbtion;

    //Clamp if needed
    if (position < min) {
        position = min;
    }
    else if (position > max) {
        position = max;
    }

    return position;
}


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

        if (neighbour < NUM_PARTICLES && particle[neighbour].fp_best < f_von_neumann_best) {
            f_von_neumann_best = particle[neighbour].fp_best;
            memcpy(von_neumann_best, particle[neighbour].p, sizeof(double) * NUM_VARIABLES);
        }
    }


}

double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double *best_position, char *objective_function_name) {


    //-------------------------------------------------------------------------------------Might change termination condition later
    //Define termination condition for each case 
    double termination_condition = -INFINITY;

     if (strcmp(objective_function_name, "griewank") == 0
     ||strcmp(objective_function_name, "levy") == 0
     ||strcmp(objective_function_name, "rastrigin") == 0
     ||strcmp(objective_function_name, "rosenbrock") == 0
     ||strcmp(objective_function_name, "schwefel") == 0
     ||strcmp(objective_function_name, "dixon_price") == 0) {
        termination_condition = 0.000001;
    } else if (strcmp(objective_function_name, "styblinski_tang") == 0) {
       termination_condition = -39.1665*NUM_VARIABLES;
    }

   
    //Seed for the random number generator to ensure the sequence of random numbers generated is different each time
    srand((unsigned int)time(NULL));

    
    //Timing variables for initilize time calculation
    clock_t start_cpu, end_cpu;
    double serial_cpu_time;
    time_t start_time, end_time;
    double parallel_cpu_time;

    
    //Inertia max and min weights (not max and min values)
    double w_max = 5, w_min = 0.2, w; 
    //0.7298 constant value if prefered


    //Initialize cognitive and social constants
    double c1, c2, c1_max = 2, c2_max = 2, c1_min = 1.5, c2_min = 1.5;


    //Transition function constants for inertia and particle attractor weights based on neighbourhood influence
    double alpha = 10; //Steepness coefficient (i.e. assymptotic behaviour of transition)
    double beta = 0.6; //weight of transition (i.e. more global influence than local after 60% of iterations)
    double g_weight = 0; //particle attractor weight 


    //Stagnation and break constants
    double epsilon = 1e-6; //Threshold for stagnation to be considered for termination conditions
    int stagnated = 0;
    int break_count = 0;
    int max_stagnation = 1500;
    int break_threshold = 1500;
    double prev_fg_best = INFINITY;


    //Start recording init time once memory begins to be allocated 
    start_cpu = clock();
    time(&start_time);


    //Particle array for each particle structure
    Particle *particle = (Particle *)malloc(NUM_PARTICLES * sizeof(Particle));
    if (particle == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }


    //Allocate memory for particles and dimensions
    //We do this as a single block to avoid memory allocation locks for parallelization synchronization
    //We can then use pointer arithmatic to correctly navigate these
    double *x_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *v_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    double *p_data = calloc(NUM_PARTICLES * NUM_VARIABLES, sizeof(double));
    

    //Global variables
    double *g = calloc(NUM_VARIABLES, sizeof(double)); //Best position
    double fg_best = INFINITY; //Best fitness at position


    //Check for proper allocation
    if (x_data == NULL || v_data == NULL || p_data == NULL || g == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }


    //Sets a pointer to the memory address of the first coordinate in each particles position velocity and best position
    //This ensures that each particles coordinates are stored properly in the 1d arrays
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particle[i].x = &x_data[i * NUM_VARIABLES];
        particle[i].v = &p_data[i * NUM_VARIABLES];
        particle[i].p = &v_data[i * NUM_VARIABLES];
        particle[i].fp_best = 0.0;
    }


    //Precompute n primes based on dimension (i.e. if NUM_VARIABLES is 10 then we create an array with the first 10 prime numbers)
    //This is used for our particle distribution which is seqenced according to the halton sequence
    uint64_t *primes = n_primes(NUM_VARIABLES);
    

    //Use 1/5 the avaialable cores so CPU isn't completely used up by process and communication isn't obscured between threads
    //This was done on a 20 core CPU so if yours has say 8 Cores then you should also use 4 but if you only have 4 then you should use 2 or 3
    //Also using more cores leads to less communication between swarms since they are subsetted so this ensures there are enough particles per thread
    int num_cores = omp_get_num_procs() / 5;
    omp_set_num_threads(num_cores);

    //Initialize parallelization by creating an array of local best values based on number of threads which is also just the num_cores value
    double fg_best_local[omp_get_max_threads()];

    //Same for position but 2d since each thread will have a local best position with n variables
    double **g_local = malloc(omp_get_max_threads() * sizeof(double *));
    if (g_local == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < num_cores; i++) {
        g_local[i] = malloc(NUM_VARIABLES * sizeof(double));
    }
    printf("Number of cores used: %d\n", num_cores);
    


    // -------------------------------------------------------------Initialization loop using parallel computing-----------------------------------------------------------------
    #pragma omp parallel
    {
        //Local/Core specific variables
        int thread_id = omp_get_thread_num();
        fg_best_local[thread_id] = INFINITY; //INFINITY is used as it will be the worst possible inital position
        unsigned int seed = time(NULL) + thread_id; //Different seed for each thread so there is more randomization variety

        //schedule static is used for initalization as all particles are computationally the same intially making the work load distribution even for each core
        //reduction ensures the best fg value is found safely between threads
        #pragma omp for schedule(static) reduction(min:fg_best)

        for (int i = 0; i < NUM_PARTICLES; i++) {

            for (int j = 0; j < NUM_VARIABLES; j++) {

                //Values of pointers set to particle initial positions using the halton sequence distribution
                particle[i].x[j] = halton_sequence(bounds[j].lowerBound, bounds[j].upperBound, i * NUM_VARIABLES + j, primes[j], &seed);

                particle[i].p[j] = particle[i].x[j]; //Particles best known position is initial position

                particle[i].v[j] = random_double(-1, 1, &seed); //Small initial velocity based off difference of bounds
            }

            //Intialize best personal fitness based on initial position
            particle[i].fp_best = objective_function(NUM_VARIABLES, particle[i].x);

            //Finds threads local best position
            if (particle[i].fp_best < fg_best_local[thread_id]) {
                //update global thread best fitness for set 
                fg_best_local[thread_id] = particle[i].fp_best;
                for (int k = 0; k < NUM_VARIABLES; k++) {
                    //Update global best positions for thread
                    g_local[thread_id][k] = particle[i].p[k];
                }
            }           
        }    

    }
    //Compare local best positions from each thread to find global best fitness
    //This accomplishes two things
    //  1. Acts as a filter which avoids bottlenecking and race conditions from each thread needing to be synchronized 
    //  2. Having local best values for each thread prevents premature convergence making finding local minima less likely
    for ( int thread = 0; thread < omp_get_max_threads(); thread++) {
        if (fg_best_local[thread] < fg_best) {
            fg_best = fg_best_local[thread];
            memcpy(g, g_local[thread], NUM_VARIABLES * sizeof(double));
        }
    }

    //Initlization is done now se we see how long it took in CPU time verses real time. 
    //Since we are using parallel computing the CPU time is actually higher than if we accomplished this serially but the actual time is much faster
    end_cpu = clock();
    time(&end_time);
    serial_cpu_time = ((double)(end_cpu - start_cpu))/CLOCKS_PER_SEC;
    parallel_cpu_time = difftime(end_time, start_time);
    printf("Initialized in %lf seconds serially and %lf seconds in real time from parallelization\n", serial_cpu_time, parallel_cpu_time);



    //--------------------------------------------------------------------------------PSO loop using parallel computing ----------------------------------------------------------------
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {

        /*
        g weight:
        Calculate transition weight for adaptive topology using a sigmoid function
        This weighs how attracted the particles are to each other verses the global best value

        Inertia:
        The inertia is sinousoidal making it start low to allow particles to search there initial locations 
        Since our particles have proper distribution we want to stay local initially to see if there is any potential solution
        Then it gets very agressive to search the remaining domain
        Finally is subsidizes again to get low once the particles are confident in the global best position

        social coefficients:
        c1 (cognitive coefficient) represents how confident a particle is in its own performance
        c2 (social coefficient) represents how confident a particle is in its neighbours performace
        As the algorithim is optimized c1 should start higher while c2 should start lower and they should converge
        We will accomplish this with the sigmoid easing function as well
        */
        
        g_weight = 1/(1+exp(-alpha*((double)iter/MAX_ITERATIONS - beta)));
        
        w = sin(w_max*2.3*w_min/(1+exp(-alpha*((double)iter/MAX_ITERATIONS - beta)))+w_min);
        
        c1 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((c1_max - c1_min) * pow(iter, 3)) + 3 * (c1_min - c1_max) * pow(iter, 2)) * g_weight + c1_max;
        c2 = 1/pow(MAX_ITERATIONS, 2) * (2 / MAX_ITERATIONS * ((-c2_max + c2_min) * pow(iter, 3)) + 3 * (-c2_min + c2_max) * pow(iter, 2)) * g_weight + c2_min;

        //Start parallelization
        #pragma omp parallel
        {
            // Thread specific variables initialized these are private to each thread
            int thread_id = omp_get_thread_num();
            unsigned int seed = time(NULL) + thread_id; //Different seed for each thread
            double f = INFINITY;

            //Dynamic scheduling is used here since particle calculations differ based on their respective positions so work load distribution changes after compile time
            #pragma omp for schedule(dynamic) reduction(min:fg_best)

            for (int i = 0; i < NUM_PARTICLES; i++) {

                //Von neuman topolgy initalizes the best position between locally communicating particles
                //Weighted best is the attractor coordinate based on the g_weight, von_neuman best and global best positon

                //Having these be local to each threa is also helpfull since each thread can just access its own cache instead of having to synchronize
                double von_neumann_best[NUM_VARIABLES];
                double weighted_best[NUM_VARIABLES];
             

                //Find the best position between neighbouring particles
                von_neumann_topology(particle, i, von_neumann_best, NUM_PARTICLES, NUM_VARIABLES);


                for (int j = 0; j < NUM_VARIABLES; j++) {

                    //Weighted best values how attracted a particle is to its best neighbour verses the best particle overall
                    //Having this start local then become more global makes it so the particles can favour exploration then move to exploitation later
                    //We also take the average between the best local thread position and the global best position so the particles can communicate between threads as well
                    weighted_best[j] = g_weight * (g_local[thread_id][j] + g[j]) / 2 + (1 - g_weight) * von_neumann_best[j];

                    //Create random doubles
                    double r1 = random_double(0, 1, &seed), r2 = random_double(0, 1, &seed);

                    //Update velocity
                    
                    //Updates particle velocity based on inertia weights, weighted best positions, and social/cognitive coefficients with some random doubles
                    particle[i].v[j] = w * particle[i].v[j] + c1 * r1 * (particle[i].p[j] - particle[i].x[j]) + c2 * r2 * (weighted_best[j] - particle[i].x[j]); //Same as provided velocity equation with weighted best instead of global best

                    //Update position
                    particle[i].x[j] += particle[i].v[j];
                
                    //Reflect particle position back into bounds instead of clamping
                    if (particle[i].x[j] < bounds[j].lowerBound) {
                        particle[i].x[j] = bounds[j].lowerBound + fabs(particle[i].x[j]) * 0.005;
                        particle[i].v[j] *= -0.75; //Reverse velocity so particle is discouraged from reentring the boundry
                    }
                    else if (particle[i].x[j] > bounds[j].upperBound) {
                        particle[i].x[j] = bounds[j].upperBound - fabs(particle[i].x[j]) * 0.005;
                        particle[i].v[j] *= -0.75;
                    }

                }

                //Find fitness of new values
                f = objective_function(NUM_VARIABLES, particle[i].x);
        
                //Update personal best for each particle
                if (f < particle[i].fp_best) {
                    particle[i].fp_best = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        particle[i].p[k] = particle[i].x[k];
                    }
                }

                //Update best thread position
                if (f < fg_best_local[thread_id]) {
                    fg_best_local[thread_id] = f;
                    for (int k = 0; k < NUM_VARIABLES; k++) {
                        g_local[thread_id][k] = particle[i].x[k];
                    }
                }
            }

        }
        //Update best global position through filter system
        for (int thread = 0; thread < omp_get_max_threads(); thread++) {
            if (fg_best_local[thread] < fg_best) {
                fg_best = fg_best_local[thread];
                memcpy(g, g_local[thread], NUM_VARIABLES * sizeof(double));
            }
        }

        //If there is no significant improvement for 1000 sequential iterations then create a break counter
        //If there are 1000 sequential breaks then we can conclude the function will not improve further
        //This can also only occur after the transition weighted point to allow the weight to shift to global improvment verses exploration
        if(fabs(fg_best - prev_fg_best) < epsilon) {
            stagnated++;
            if (stagnated > max_stagnation) {
                break_count++;
                if ((break_count > break_threshold && iter > MAX_ITERATIONS * beta)|| fg_best <= termination_condition) {
                    printf("Early break on iteration %d\n", iter);
                    break;
                }
                stagnated = 0;
            }
        }
        else {
            //Debugging purposes tells me the current value and where it stagnates which I can backtrace to the current weights of my algorithm
            //printf("iteration at stagnation: %d\n", iter);
            //printf("fg_best: %lf\n", fg_best); 
            break_count = 0;
            stagnated = 0;
        }

        //Set previous iteration fitness to current 
        prev_fg_best = fg_best;
        
    }
    memcpy(best_position, g, NUM_VARIABLES * sizeof(double));

    //Free allocated memory
    free(particle);
    free(x_data);
    free(v_data);
    free(p_data);
    
    free(g);
    for (int i = 0; i < num_cores; i++) {
        free(g_local[i]);
    }
    free(g_local);

    primesieve_free(primes);
    
    return fg_best;
}