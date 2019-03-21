//#include <stdio.h>
//#include <iostream>
//
//#include "CUnit.h"
//#include "CCircuit.h"
//
//#include "genetic_algorithm.h"
//
//int main(int argc, char * argv[])
//{
//				int num_parents = 50;
//
//				double best_performance = 0.0;
//				int step = 0;
//
//				int *best_vector = new int[2 * num_units + 1];
//
//				//------------------------------------------
//				// Setup an initial race with all vectors valid
//				//------------------------------------------
//				// Initialize the parent vector
//				int **all_parents = new int*[num_parents];
//				int **new_all_parents = new int*[num_parents];
//
//				// Initialize values
//				double *distribution = new double[num_parents + 1];
//				double *performance_list = new double[num_parents];
//
//				//--------------------------------------------------------
//				int length = num_units * 2 + 1;
//				bool flag_right = true;
//				int index, top, bot;
//				// Initialize values for parents
//				for (int i = 0; i < num_parents; i++) {
//					if (flag_right) {
//						all_parents[i] = new int[length];
//						new_all_parents[i] = new int[length];
//					}
//					all_parents[i][0] = rand() % num_units;
//
//					for (int j = 1; j < length; j += 2) {
//						index = j / 2;
//						top = rand() % (num_units + 2);
//						bot = rand() % (num_units + 2);
//						while (top == index || bot == index || top == bot) {
//							top = rand() % (num_units + 2);
//							bot = rand() % (num_units + 2);
//						}
//						all_parents[i][j] = top;
//						all_parents[i][j + 1] = bot;
//					}
//
//					// Check if the randomly generated vector is valid
//					if (!Check_Validity(all_parents[i])) {
//						i--;
//						flag_right = false;
//					}
//					else {
//						flag_right = true;
//					}
//
//				}
//				//--------------------------------------------------------
//
//				
//				//--------------------------------------------------------
//
//				double tolerance = 1e-5;
//				int conc = 10;
//				int waste = 100;
//				double profit = 100.0;
//				double cost = 500.0;
//				int max_steps = 10000;
//				int max_steps_since_no_change = 2000;
//				double gene_change_rate = 0.01;
//				double crossover_rate = 0.9;
//
//
//				//run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units, gene_change_rate, crossover_rate);
//				run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units,
//					best_performance, best_vector, step, tolerance, conc, waste, profit, cost, max_steps, max_steps_since_no_change,
//					gene_change_rate, crossover_rate);
//
//				std::cout << "initial step since no change: " << step;
//				system("pause");
//				//--------------------------------------------------------
//				for (int i = 0; i < num_parents; i++) {
//					delete[] all_parents[i];
//					delete[] new_all_parents[i];
//				}
//				delete[] all_parents;
//				delete[] new_all_parents;
//				delete[] distribution;
//				delete[] performance_list;
//				//--------------------------------------------------------
//
//
//    return 0;
//}
