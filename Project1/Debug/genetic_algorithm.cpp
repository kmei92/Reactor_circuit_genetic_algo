#include <stdio.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

#include "Genetic_Algorithm.h"
#include "Circuit_Simulator.h"

using namespace std;

bool Check_Convergence(double old_best_performance, double best_performance, double tol, int &steps_since_no_change, int steps, int &initial_step_since_no_change, int max_steps_since_no_change)
{
	bool has_converged = false;
	if (abs(best_performance - old_best_performance) < tol)
	{
		steps_since_no_change++;
		if (steps_since_no_change == max_steps_since_no_change)
		{
			has_converged = true;
		}
	}
	else
	{
		initial_step_since_no_change = steps;
		steps_since_no_change = 0;
	}
	return has_converged;
}

// Find the max and min value as well as the index of performance
void find_max_min(double *performance_list, int num_parents, int &max_index, int &min_index) {

	double min_value = performance_list[0];
	double max_value = performance_list[0];
	max_index = 0;
	min_index = 0;

	for (int i = 1; i < num_parents; i++) {
		if (performance_list[i] > max_value) {
			max_value = performance_list[i];
			max_index = i;
		}

		if (performance_list[i] < min_value) {
			min_value = performance_list[i];
			min_index = i;
		}
	}

}

void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2, double *distribution, int num_parents) {
	double pt1, pt2;

	do {
		// Generate 2 random double value between 0 and max value in distribution
		pt1 = ((double)rand() / RAND_MAX) * distribution[num_parents];
		pt2 = ((double)rand() / RAND_MAX) * distribution[num_parents];
		//pt1 = rand() % int(distribution[num_parents]);
		//pt2 = rand() % int(distribution[num_parents]);

	} while (pt1 == pt2);

	for (int j = 1; j < num_parents + 1; j++) {
		if (pt1 < distribution[j]) {
			tgt_parent_index_1 = j - 1;
			break;
		}
	}

	for (int j = 1; j < num_parents + 1; j++) {
		if (pt2 < distribution[j]) {
			tgt_parent_index_2 = j - 1;
			break;
		}
	}

}

// Randomly decide whether to do crossover
bool RollingDice(double crossover_rate) {
	int i = 1 + rand() % 100;
	if (i < crossover_rate * 100)
		return true;
	else
		return false;
}

// Do mutation
void mutation(int *circuit_vector, double gene_change_rate) {
	int needle;
	for (int i = 0; i < num_units * 2 + 1; i++) {
		needle = 1 + rand() % 100;
		if (needle > 100 - 100 * gene_change_rate) {
			if (i == 0) {
				circuit_vector[i] = rand() % num_units;
			}
			else {
				circuit_vector[i] = rand() % (num_units + 2);
			}
		}
	}
}


// swap the the old and new parents
void swapping_parent(int **all_parents, int **new_all_parents, int num_parents, int length) {
	for (int i = 0; i < num_parents; i++) {
		for (int j = 0; j < length; j++) {
			int tmp;
			tmp = all_parents[i][j];
			all_parents[i][j] = new_all_parents[i][j];
			new_all_parents[i][j] = tmp;
		}
	}
}


void run_genetic_algorithm(int **all_parents, int **new_all_parents, double *performance_list, double *distribution, int num_parents, int num_units, 
	double best_performance, int *best_vector, int initial_step_since_no_change, double tolerance, int conc, int waste, double profit, double cost, int max_steps, int max_steps_since_no_change, double gene_change_rate, double crossover_rate) 
{

	srand(time(0));
	int length = num_units * 2 + 1;

	//// print all the vectors
	//for (int i = 0; i < num_parents; i++) {
	//	for (int j = 0; j < length; j++) {
	//		cout << all_parents[i][j] << " ";
	//		if (j == length - 1) {
	//			cout << endl;
	//		}
	//	}
	//}

	int max_index, min_index; // the index of the best performance one
	double exp_performance = 10000.0; // expected performance
	// max iteration steps
	//double best_performance = 0.0;      // best performance in one generation
	double worst_performance;
	double tol = 0.1;             // a small value to make sure even the worst one has the right to reproduce
	int steps = 0;

	int *circuit_child1 = new int[length]; // potential child 1
	int *circuit_child2 = new int[length]; // potential child 2
	double t1, t2;
	double t_sum = 0;


	ofstream performance_output;
	performance_output.open("../performance_list.txt");

	double old_best_performance = exp_performance;
	int steps_since_no_change = 0;
	bool has_converged = false;

	//while (!has_converged) {
	while (!has_converged && steps < max_steps) {

	// Out loop
	//while (best_performance < exp_performance && steps < max_steps) {

		//---------------------------
		// run the circuit simulator
		// Update function
		//---------------------------
		t1 = clock();
		for (int i = 0; i < num_parents; i++) {
			//performance_list[i] = UpdateFunction(all_parents[i], length);
			performance_list[i] = Evaluate_Circuit(all_parents[i], tolerance, max_steps, num_units, conc, waste, profit, cost);
			//cout << "i  " << i << endl;
		}
		t2 = clock();

		t_sum += (t2 - t1) / CLOCKS_PER_SEC;
		/*
		cout << "Performance: ";
		for (int i =0 ;i < num_parents; i++){
			cout << performance_list[i] << " ";
		}
		cout << endl;
		 */

		 // Evaluate the performance
		 // find the max and min value
		find_max_min(performance_list, num_parents, max_index, min_index);

		// Choose the best one and the worst one
		best_performance = performance_list[max_index];
		worst_performance = performance_list[min_index];
		//cout << "best performance: " << best_performance << endl;
		//cout << "worst performance: " << worst_performance << endl;


		double avg = 0.0;
		for (int i = 0; i < num_parents; i++) {
			avg += performance_list[i];
		}
		avg = avg / num_parents;
		//cout << "average: "<< avg;

		//----------------------------
		performance_output << best_performance << " " << worst_performance << " " << avg << endl;
		//----------------------------

		// If the performance is negative
		if (worst_performance < 0) {
			for (int i = 0; i < num_parents; i++) {
				performance_list[i] = (performance_list[i] - worst_performance + tol);
			}
		}

		// Cast a needle to find parents
		int *pair[2];
		double sum = 0.0;
		distribution[0] = 0.0;
		for (int i = 0; i < num_parents; i++) {
			sum += performance_list[i];
			distribution[i + 1] = sum;
		}

		// push the best one to next generation
		for (int i = 0; i < length; i++) {
			new_all_parents[0][i] = all_parents[max_index][i];
		}

		int count = 1;
		int tgt_parent_index_1;
		int tgt_parent_index_2;
		int pt;
		while (count < num_parents) {
			select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);
			pair[0] = all_parents[tgt_parent_index_1];
			pair[1] = all_parents[tgt_parent_index_2];

			/*
				//-------------------------------------------
				// Old and more compact method
				//-------------------------------------------

				// Do cross over and Mutation
				for (int i = 0; i < length; i++) {
					if (i < pt) {
						new_all_parents[count][i] = pair[0][i];
						// Decide whether put to mutation
						if (RollingDice()) {
							// Do mutation
							mutation(count, i);
						}
					}
					else {
						new_all_parents[count][i] = pair[1][i];
						if (RollingDice()) {
							// Do mutation
							mutation(count, i);
						}
					}
				}

				//Check
				//-----------------------
				// check validity here
				// if true count ++
				// if false continue without count ++
				//-----------------------
				if (Check_Validity(new_all_parents[count])) {
					count++;
				}

				*/
				//----------------------------------------------
				// Check whether to do the crossover
			if (RollingDice()) {

				// Crossover to generate children
				pt = rand() % length;
				while (pt == 0 || pt == length - 1) {
					pt = rand() % length;
				}
				// the part before the break point
				for (int i = 0; i < pt; i++) {
					circuit_child1[i] = pair[0][i];
					circuit_child2[i] = pair[1][i];

				}
				// the part after the break point
				for (int i = pt; i < length; i++) {
					circuit_child1[i] = pair[1][i];
					circuit_child2[i] = pair[0][i];

				}
			}
			else {
				// keep parents unchanged
				for (int i = 0; i < length; i++) {
					circuit_child1[i] = pair[0][i];
					circuit_child2[i] = pair[1][i];
				}
			}
			// Do mutation
			mutation(circuit_child1);
			mutation(circuit_child2);

			// Check the first potential child
			if (Check_Validity(circuit_child1)) {
				for (int i = 0; i < length; i++) {
					new_all_parents[count][i] = circuit_child1[i];
				}
				count++;
			}

			// Check whether filling is finished
			if (count >= num_parents) {
				break;
			}

			// Check the second potential child
			if (Check_Validity(circuit_child2)) {
				for (int i = 0; i < length; i++) {
					new_all_parents[count][i] = circuit_child2[i];
				}
				count++;
			}

		
		}
		swapping_parent(all_parents, new_all_parents, num_parents, length);
		
		has_converged = Check_Convergence(old_best_performance, best_performance, tol, steps_since_no_change, steps, initial_step_since_no_change, max_steps_since_no_change);
		old_best_performance = best_performance;

		for (int i = 0; i < 2 * num_units + 1; i++)
		{
			best_vector[i] = all_parents[0][i];
		}

		steps++;
		//cout << "step " << steps << " best performance " << best_performance << endl;

	}

	performance_output << endl;
	performance_output.close();
	ofstream final_vec;
	final_vec.open("../data.txt");

	cout << "simulation function time used: " << t_sum << endl;
	cout << "best performance: " << best_performance << endl;
	cout << "final vector: " << endl;
	for (int i = 0; i < length; i++) {
		cout << all_parents[max_index][i] << " ";
		final_vec << all_parents[max_index][i] << " ";
	}
	cout << endl;
	final_vec.close();


	delete[] circuit_child1;
	delete[] circuit_child2;

}