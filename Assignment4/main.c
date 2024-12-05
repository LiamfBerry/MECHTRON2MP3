// CODE: inlcude library(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "OF_lib.h"
#include <time.h>

int main(int argc, char **argv) {

    // Validate arguments
    if (argc < 7) {
        fprintf(stderr, "Usage: %s <ObjectiveFunctionName> <NUM_VARIABLES> <LowerBound> <UpperBound> <NUM_PARTICLES> <MAX_ITERATIONS>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    char *objective_function_name = argv[1];
    int NUM_VARIABLES = atoi(argv[2]);   // Number of dimensions
    double lower_bound = atof(argv[3]);  // Search space lower bound
    double upper_bound = atof(argv[4]); // Search space upper bound

    const int NUM_PARTICLES = atoi(argv[5]);  // Number of particles, e.g. 1000
    const int MAX_ITERATIONS = atoi(argv[6]); // Maximum iterations, e.g. 1000

    double start, end;
    time_t start_time, end_time;
    double cpu_time, real_time;

   // Assign the appropriate function pointer based on user input
    ObjectiveFunction objective_function = NULL;

    // Map function name to pointer
    if (strcmp(objective_function_name, "griewank") == 0) {
        objective_function = griewank;
    } else if (strcmp(objective_function_name, "levy") == 0) {
        objective_function = levy;
    } else if (strcmp(objective_function_name, "rastrigin") == 0) {
        objective_function = rastrigin;
    } else if (strcmp(objective_function_name, "rosenbrock") == 0) {
        objective_function = rosenbrock;
    } else if (strcmp(objective_function_name, "schwefel") == 0) {
        objective_function = schwefel;
    } else if (strcmp(objective_function_name, "dixon_price") == 0) {
        objective_function = dixon_price;
    } else if (strcmp(objective_function_name, "michalewicz") == 0) {
        objective_function = michalewicz;
    } else if (strcmp(objective_function_name, "styblinski_tang") == 0) {
        objective_function = styblinski_tang;
    } else {
        printf("Invalid objective function: %s\n", objective_function_name);
        return 1;
    }

    // Print inputs for verification
    printf("Objective Function: %s\n", objective_function_name);
    printf("Number of Variables: %d\n", NUM_VARIABLES);
    printf("Lower Bound for all variables: %lf\n", lower_bound);
    printf("Upper Bound for all variables: %lf\n", upper_bound);
    printf("-------------------------------------\n");
    printf("PSO initiated with:\n");
    printf("Number of particles: %d\n", NUM_PARTICLES);
    printf("Number of iterations: %d\n", MAX_ITERATIONS);

    
    // bounds[j] stores the lower and upper bound for variable j
    Bound *bounds = (Bound *)malloc(NUM_VARIABLES*sizeof(Bound));

    // The following loop means all the vairables have the same lower and upper bounds
    for (int i = 0; i < NUM_VARIABLES; i++)
    {
        bounds[i].lowerBound = lower_bound;
        bounds[i].upperBound = upper_bound;
    }
    

    // best_position is sent to pso and the final global best solution is save into this array
    double *best_position = (double *)malloc(NUM_VARIABLES*sizeof(double));
    
    // CODE: measure the CPU time only for the following pso function

    time(&start_time);
    start = clock();
    double best_fitness = pso(objective_function, NUM_VARIABLES, bounds, NUM_PARTICLES, MAX_ITERATIONS, best_position, objective_function_name);
    end = clock();
    time(&end_time);

    real_time = difftime(end_time, start_time);
    cpu_time = ((double)(end - start))/CLOCKS_PER_SEC;
    // Print results
    // best_fitness is the objective function value (fitness) for the final global best solution, this is the lowest fitness achieved
    printf("\nResults using %s\n", objective_function_name);
    printf("-------------------------------------\n");
    printf("CPU time: %.3lf\t seconds\n", cpu_time);
    printf("real time: %.3lf\t seconds\n", real_time);
    printf("Optimal fitness: %lf\n", best_fitness);
    printf("Optimal position: ");
    for (int i = 0; i < NUM_VARIABLES; i++) {
        printf("%lf ", best_position[i]);
    }
    printf("\n");
    printf("-------------------------------------\n");

    free(bounds);
    free(best_position);


    return 0;
}
