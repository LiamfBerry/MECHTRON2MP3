// CODE: include library(s)
#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Helper function to generate random numbers in a range
double random_double(double min, double max) {
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

// CODE: implement other functions here if necessary

double pso(ObjectiveFunction objective_function, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double *best_position) {

    // CODE: implement pso function here

    //Initialize constants
    double w = 0.7, c1 = 1.5, c2 = 1.5;

    //Allocate memory for vector rows
    double **x = calloc(NUM_PARTICLES, sizeof(double *));
    double **v = calloc(NUM_PARTICLES, sizeof(double *));
    double **p = calloc(NUM_PARTICLES, sizeof(double *));

    double *fp_best = calloc(NUM_PARTICLES, sizeof(double));
    double *g = calloc(NUM_VARIABLES, sizeof(double));

    //Initialize 
    double fg_best = INFINITY;
    

    //Initialization loop
    for (int i = 0; i < NUM_PARTICLES; i++) {
        
        //Initialize memory for vector columns
        x[i] = calloc(NUM_VARIABLES, sizeof(double));
        v[i] = calloc(NUM_VARIABLES, sizeof(double));
        p[i] = calloc(NUM_VARIABLES, sizeof(double));


        //Initialize particle positions with uniformly distributed random vector
        for (int j = 0; j < NUM_VARIABLES; j++) {

            x[i][j] = random_double(bounds[j].lowerBound, bounds[j].upperBound);
       
            p[i][j] = x[i][j]; //Particles best known position is initial position

            v[i][j] = random_double(-fabs(bounds[j].upperBound - bounds[j].lowerBound), fabs(bounds[j].upperBound - bounds[j].lowerBound)); //Small initial velocity based off difference of bounds
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
        for (int i = 0; i < NUM_PARTICLES; i++) {
            for (int j = 0; j < NUM_VARIABLES; j++) {

                //Create random doubles
                double r1 = random_double(0,1), r2 = random_double(0,1);

                //Update velocity
                v[i][j] = w * v[i][j] + c1 * r1 * (p[i][j] - x[i][j]) + c2 * r2 * (g[j] - x[i][j]);

                //Update position
                x[i][j] = x[i][j] + v[i][j];

                //Clamp particle position within bounds
                if (x[i][j] < bounds[j].lowerBound) {
                    x[i][j] = bounds[j].lowerBound;
                }
                else if (x[i][j] > bounds[j].upperBound) {
                    x[i][j] = bounds[j].upperBound;
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
    memcpy(best_position, g, NUM_VARIABLES * sizeof(double));

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
