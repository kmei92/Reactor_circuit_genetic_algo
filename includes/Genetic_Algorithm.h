
#include "CCircuit.h"

#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

/*
 * Find the best and worst performance from the list
 */
void find_max_min(double *performance_list, int num_parents, int &max_index, int &min_index);

/*
 * Use a Roulette method to select parents based on the probability distribution
 */
void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2, double *distribution, int num_parents);

/*
 * Use a Roulette method to decide whether to do crossover
 */
bool RollingDice(double crossover_rate = 0.9);

/*
 * Check each value in the vector to find whether it should do mutation
 */
void mutation(int *circuit_vector, double gene_change_rate = 0.01);


/*
 * Swapp the parents and children
 */
void swapping_parent(int **all_parents, int **new_all_parents, int num_parents, int length);


/*
 * The main function to run genetic algorithm
 */
void run_genetic_algorithm(int **all_parents, int **new_all_parents, double *performance_list, double *distribution, int num_parents, int num_units, int max_steps,
        double gene_change_rate = 0.01, double crossover_rate = 0.9);


#endif //GENETIC_ALGORITHM_H

