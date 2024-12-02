// CODE: include library(s)
#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Helper function to generate random numbers in a range
double random_double(double min, double max) {
    return min + (max - min) * ((double)rand() / RAND_MAX);
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
double halton_sequence(double min, double max, int index, int dimension) {
    double h = 1.0;
    double halton_value = 0.0;

    int prime = nth_prime_approx(dimension);

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


    long double epsilon = 1e-6;

    //Allocate memory for vector rows
    double **x = calloc(NUM_PARTICLES, sizeof(double *));
    double **v = calloc(NUM_PARTICLES, sizeof(double *));
    double **p = calloc(NUM_PARTICLES, sizeof(double *));

    double *fp_best = calloc(NUM_PARTICLES, sizeof(double));
    double *g = calloc(NUM_VARIABLES, sizeof(double));

    if (x == NULL || v == NULL || p == NULL || fp_best == NULL || g == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Initialize 
    double fg_best = INFINITY;
    
    //Initialization loop
    for (int i = 0; i < NUM_PARTICLES; i++) {
        
        //Initialize memory for vector columns
        x[i] = calloc(NUM_VARIABLES, sizeof(double));
        v[i] = calloc(NUM_VARIABLES, sizeof(double));
        p[i] = calloc(NUM_VARIABLES, sizeof(double));

        if (x[i] == NULL || v[i] == NULL || p[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        //Initialize particle positions with random positions
        for (int j = 0; j < NUM_VARIABLES; j++) {

            x[i][j] = halton_sequence(bounds[j].lowerBound, bounds[j].upperBound, i * NUM_VARIABLES + j, NUM_VARIABLES);
       
            p[i][j] = x[i][j]; //Particles best known position is initial position

            v[i][j] = random_double(-1, 1); //Small initial velocity based off difference of bounds
        }

        //Intialize best fitness evaluted from sets of decision variables 
        fp_best[i] = objective_function(NUM_VARIABLES, x[i]);

        if (fp_best[i] < fg_best) {
            //update global best fitness for set 
            fg_best = fp_best[i];
            for (int k = 0; k < NUM_VARIABLES; k++) {
                //Update global best positions
                g[k] = p[i][k];
            }
        }
    }

    //PSO loop
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {

        //Make inertia dynamic, starts large and gets progressively smaller on an easing function
        w = 1/pow(MAX_ITERATIONS,2) * (2/MAX_ITERATIONS * ((w_max - w_min) * pow(iter, 3)) + 3 * (w_min - w_max) * pow(iter,2)) + w_max;

        //c1 (cognitive coefficient) represents how confident a particle is in its own performance
        //c2 (social coefficient) represents how confident a particle is in its neighbours performace
        //As the algorithim is optimized c1 should start higher while c2 should start lower and they should converge
        //We will accomplish this with the sigmoid easing function
        sigmoid = 1/(1+exp(alpha*(MAX_ITERATIONS - iter)));
        c1 = 1/pow(MAX_ITERATIONS,2) * (2/MAX_ITERATIONS * ((c1_max - c1_min) * pow(iter, 3)) + 3 * (c1_min - c1_max) * pow(iter,2))*sigmoid + c1_max;
        c2 = 1/pow(MAX_ITERATIONS,2) * (2/MAX_ITERATIONS * ((-c2_max + c2_min) * pow(iter, 3)) + 3 * (-c2_min + c2_max) * pow(iter,2))*sigmoid + c2_min;

        for (int i = 0; i < NUM_PARTICLES; i++) {
            for (int j = 0; j < NUM_VARIABLES; j++) {

                 //Update velocity constraints based on bounds
                v_max = 0.1 * (bounds[j].upperBound - bounds[j].lowerBound);

                //Create random doubles
                double r1 = random_double(0,1), r2 = random_double(0,1);

                //Update velocity
                
                //If velocity stagnates then reinitialize
                if(fabs(v[i][j]) < epsilon) {
                    v[i][j] = random_double(-1, 1);
                } 
                else {
                    v[i][j] = w * v[i][j] + c1 * r1 * (p[i][j] - x[i][j]) + c2 * r2 * (g[j] - x[i][j]);// - w * (g[j] - p[i][j]);
                }

                //Update position
                x[i][j] = x[i][j] + v[i][j];

                //Reflect particle position back into bounds instead of clamping
                if (x[i][j] < bounds[j].lowerBound) {
                    x[i][j] = 2 * bounds[j].lowerBound - x[i][j];
                    v[i][j] *= -1; //Reverse velocity to encourage more exploration
                }
                else if (x[i][j] > bounds[j].upperBound) {
                    x[i][j] = 2 * bounds[j].upperBound - x[i][j];
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
            double f = objective_function(NUM_VARIABLES, x[i]);
      
            if (f < fp_best[i]) {
                fp_best[i] = f;
                for (int k = 0; k < NUM_VARIABLES; k++) {
                    p[i][k] = x[i][k];
                }
            }
            if (f < fg_best) {
                fg_best = f;
                for (int k = 0; k < NUM_VARIABLES; k++) {
                    g[k] = x[i][k];
                }
                
            }
        }
    }
    //Create list of local best values (n+1 nearest points of x)
   
    double **simplex = malloc((NUM_VARIABLES + 1) * sizeof(double *));
    if (simplex == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < NUM_VARIABLES + 1; i++) {
        simplex[i] = calloc(NUM_VARIABLES, sizeof(double));
        if (simplex[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_VARIABLES; i++) {
        simplex[0][i] = g[i];
    }
    for (int i = 1; i < NUM_VARIABLES + 1; i++) {
        for (int j = 0; j < NUM_VARIABLES; j++) {
            double purturb = (random_double(-NUM_VARIABLES, NUM_VARIABLES) - 0.5) * 0.2 * (bounds[j].upperBound - bounds[j].lowerBound);
            printf("purturb %lf\n", purturb);
            simplex[i][j] = g[j] + purturb;

            //clamp 
            if (simplex[i][j] > bounds[j].upperBound) {
                simplex[i][j] = bounds[j].upperBound;
            }
            if (simplex[i][j] < bounds[j].lowerBound) {
                simplex[i][j] = bounds[j].lowerBound;
            }
        }
    }

    printf("simplex before\n ");
    for (int i = 0; i < NUM_VARIABLES; i++) {
        printf(" %lf ", simplex[1][i]);
    }
    printf("simplex before\n ");

    //Best local value
    
    printf("\n[");
    for (int i = 0; i < NUM_VARIABLES + 1; i++) {
        printf(" %lf ", objective_function(NUM_VARIABLES, simplex[i]));
    }
    printf("]\n");
    
    
    double fl_best = nelder_mead(objective_function, simplex, NUM_VARIABLES, MAX_ITERATIONS, bounds);
    printf("best before nelders %lf\n", fg_best);
    printf("best after nelders %lf\n", fl_best);

    printf("simplex after\n ");
    for (int i = 0; i < NUM_VARIABLES; i++) {
        printf(" %lf ", simplex[1][i]);
    }
    printf("simplex after\n ");
    
    if (fl_best < fg_best) {
        fg_best = fl_best;
        memcpy(best_position, simplex[0], NUM_VARIABLES * sizeof(double));
    }
    else {
        memcpy(best_position, g, NUM_VARIABLES * sizeof(double));
    }
    
    for(int i = 0; i < NUM_VARIABLES + 1; i++) {
        free(simplex[i]);
    }

    free (simplex);



    for (int i = 0; i < NUM_PARTICLES; i++) {
        free(x[i]);
        free(v[i]);
        free(p[i]);
    }
    free(x);
    free(v);
    free(p);
    free(fp_best);
    free(g);

    return fg_best;
}

//PSO approximates area of minima and nelder mead gets more precise answer
double nelder_mead(ObjectiveFunction objective_function, double **simplex, int NUM_VARIABLES, int MAX_ITERATIONS, Bound *bounds) {
    double alpha = 1.0; //Reflection coefficient
    double beta = 0.5; //Contraction coefficient
    double gamma = 2.0; //Expansion coefficient
    double sigma = 0.5; //Shrink coefficient

    double *x_centroid = malloc (NUM_VARIABLES * sizeof(double));
    double *x_best = malloc (NUM_VARIABLES * sizeof(double));

    double *x_reflect = malloc(NUM_VARIABLES * sizeof(double));
    double *x_expand = malloc(NUM_VARIABLES * sizeof(double));
    double *x_contract = malloc(NUM_VARIABLES * sizeof(double));

    //Function array that will determine best and worst points
    double *f = calloc((NUM_VARIABLES + 1), sizeof(double));
    if (f == NULL || x_centroid == NULL || x_best == NULL || x_reflect == NULL || x_expand == NULL|| x_contract == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {

        beta = beta - 0.01*iter/MAX_ITERATIONS; //Contraction coefficient
        gamma = gamma - iter/MAX_ITERATIONS; //Expansion coefficient
        sigma = sigma - 0.01*iter/MAX_ITERATIONS; //Shrink coefficient

         for (int i = 1; i <= NUM_VARIABLES; i++) {
            for (int j = 0; j < NUM_VARIABLES; j++) {
                if (simplex[i][j] > bounds[j].upperBound) {
                    simplex[i][j] = bounds[j].upperBound;
                }
                if (simplex[i][j] < bounds[j].lowerBound) {
                    simplex[i][j] = bounds[j].lowerBound;
                }
            }
        }

        for (int i = 0; i < NUM_VARIABLES + 1; i++) {
            //Find objective function determinates
            f[i] = objective_function(NUM_VARIABLES, simplex[i]);
        }

        //Sort objective values (and simplex values) <-- still need to do second part
        heapSort(f, simplex, NUM_VARIABLES + 1, NUM_VARIABLES);

        /*printf("simplex sorted\n"); 
        for (int i =0; i<NUM_VARIABLES+1;i++) {
            for(int j=0;j<NUM_VARIABLES;j++) {
                printf(" %lf ", simplex[i][j]);
            }
            printf("\n");
        }
         printf("\n\n"); 

        printf("value sorted\n"); 
        for (int i =0; i<NUM_VARIABLES+1;i++) {
            printf(" %lf ", f[i]);
        }
         printf("\n"); */

        //Calculate centroid of all points except x n+1 i.e. worst point
        for (int i = 0; i < NUM_VARIABLES; i++) {
            //Initialize centroid values for each coordniate in dimension
            x_centroid[i] = 0.0;
            for (int j = 0; j < NUM_VARIABLES; j++) {
                x_centroid[i] += simplex[j][i]; //Adds coresponding axis (i.e. vector addition)
            }
            x_centroid[i] /= (double)NUM_VARIABLES;
        }

        //simplex transformation logic
        for (int i = 0; i < NUM_VARIABLES; i++) {
            x_reflect[i] = x_centroid[i] + alpha * (x_centroid[i] - simplex[NUM_VARIABLES][i]);
        }
        double f_reflect = objective_function(NUM_VARIABLES, x_reflect);

        if (f_reflect < f[0]) {
            for (int i = 0; i < NUM_VARIABLES; i++) {
                x_expand[i] = x_centroid[i] + gamma * (x_reflect[i] - x_centroid[i]);
            }
            double f_expand = objective_function(NUM_VARIABLES, x_expand);

            if (f_expand < f_reflect) {
                for (int i = 0; i < NUM_VARIABLES; i++) {
                    simplex[NUM_VARIABLES][i] = x_expand[i];
                }
            }
           else {
                for (int i = 0; i < NUM_VARIABLES; i++) {
                    simplex[NUM_VARIABLES][i] = x_reflect[i];
                }
            }
        }

        else if (f_reflect < f[NUM_VARIABLES - 1]) {
            for (int i = 0; i < NUM_VARIABLES; i++) {
                    simplex[NUM_VARIABLES][i] = x_reflect[i];
                }
        }
        //If triggers then we know f_reflect >= second worst point
        else { 

            if (f_reflect < f[NUM_VARIABLES]) {
                for (int i = 0; i < NUM_VARIABLES; i++) {
                    x_contract[i] = x_centroid[i] + beta * (x_reflect[i] - x_centroid[i]);
                }
            }
            else {
                for (int i = 0; i < NUM_VARIABLES; i++) {
                    x_contract[i] = x_centroid[i] + beta * (simplex[NUM_VARIABLES][i] - x_centroid[i]);
                }
            }

            double f_contract = objective_function(NUM_VARIABLES, x_contract);

            if (f_contract < f[NUM_VARIABLES]) {
               for (int i = 0; i < NUM_VARIABLES; i++) {
                    simplex[NUM_VARIABLES][i] = x_contract[i];
                }
            }
            else {
                //Shrink 
                for (int i = 1; i <= NUM_VARIABLES; i++) {
                    for (int j = 0; j < NUM_VARIABLES; j++) {
                        simplex[i][j] = simplex[0][j] + sigma * (simplex[i][j] - simplex[0][j]);
                    }
                }
            }
            
        }
    }
    double f_best = objective_function(NUM_VARIABLES, simplex[0]);
    
 
    free(x_centroid);
    free(f);
    free(x_best);
    free(x_reflect);
    free(x_expand);
    free(x_contract);
    

    return f_best;
}


//Functions taken from assignment 2 to sort objective values (specifically heap sort)
void swap(double *x, double *y) {

    //dereferences 
    double temp = *x; //Assign temporary int to dereferenced array pointer
    *x = *y; //Dereference pointer and change value to other dereferenced pointer
    *y = temp; //Assign other dereferenced pointer to temporary value
}

//Swaps each verticie in the simplex which switches each coordniate for n dimensions
void swap_simplex_rows(double *row1, double *row2, int NUM_VARIABLES) {
    for (int i = 0; i < NUM_VARIABLES; i++) {
        swap(&row1[i], &row2[i]);
    }
}

void maxHeap(double *f, double **simplex, int size, int i, int NUM_VARIABLES) {

    int max = i; //max is current node

    //defines children of node in heap
    int leftChild = 2*i + 1; 
    int rightChild = 2*i + 2;

    //Checks if children are greater than root then updates max
    if (leftChild < size && f[leftChild] > f[max]) {
        max = leftChild;
    }
    if (rightChild < size && f[rightChild] > f[max]) {
        max = rightChild;
    }

    //If max is not root index make it root
    if (max != i) {
        swap(&f[max], &f[i]);
        swap_simplex_rows(simplex[i], simplex[max], NUM_VARIABLES);

        //Repeat for each node and its children until max heap is created
        maxHeap(f, simplex, size, max, NUM_VARIABLES);
    }
}

void heapSort(double *f, double **simplex, int size, int NUM_VARIABLES) {
    //Size is number of verticies which is n + 1

    //Builds max heap
    for (int i = size/2-1; i >= 0; i--) { //i=n/2-1 ensures we start at the deepest parent layer and move upwards until we are at the root
        maxHeap(f, simplex, size, i, NUM_VARIABLES);
    }

    //smallest element to root
    for (int i = size-1; i > 0; i--) { //start from end and go to start 
        swap(&f[0], &f[i]);
        if (i != 0) {
            swap_simplex_rows(simplex[0], simplex[i], NUM_VARIABLES);
        }

        //Reheapifies reduced (unsorted heap)
        maxHeap(f, simplex, i, 0, NUM_VARIABLES);
    }

}

//Euclidean distance finds the distance between two n-dimensional points
//For this case we are finding the n + 1 nearest values to the global best position
double euclidean_distance(double *x, double *g, int NUM_VARIABLES) {

    double distance = 0.0;
    for (int i = 0; i < NUM_VARIABLES; i++) {
        distance += pow(g[i]-x[i], 2);
    }
    return sqrt(distance);
}


//Nelders is same values now but we want to improve it
//Also when nelders-mead returns as best position all x values are 0?