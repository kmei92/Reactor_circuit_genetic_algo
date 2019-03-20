#include "CCircuit.h"

#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H


void find_max_min(double *performance_list, int num_parents, int &max_index, int &min_index);

void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2, double *distribution, int num_parents);

bool RollingDice(double crossover_rate = 0.9);

void mutation(int i, int &current_value, double gene_change_rate = 0.01);

void swapping_parent(int **all_parents, int **new_all_parents, int num_parents, int length);

void run_genetic_algorithm(int **all_parents, int **new_all_parents, double *performance_list, double *distribution, int num_parents, int num_units, int max_steps,
        double gene_change_rate = 0.01, double crossover_rate = 0.9);



#endif //GENETIC_ALGORITHM_H
