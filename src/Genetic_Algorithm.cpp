#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include "../includes/Genetic_Algorithm.h" 
#include "../includes/Circuit_Simulator.h"

using namespace std;


bool Check_Convergence(double expected_performance, double performance, double tol, int steps, int max_steps)
{
	bool has_converged = false;
	if (std::abs(performance - expected_performance) < tol && steps > max_steps)
	{
		has_converged = true;
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

void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2, double *distribution, int num_parents){
    double pt1, pt2;

    do {
        // Generate 2 random double value between 0 and max value in distribution
        pt1 = ((double)rand()/RAND_MAX) * distribution[num_parents];
        pt2 = ((double)rand()/RAND_MAX) * distribution[num_parents];

    }while(pt1 == pt2);

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
bool RollingDice(double crossover_rate){
    int i = 1 + rand() % 100;
    if (i < crossover_rate * 100)
        return true;
    else
        return false;
}

// Do mutation
void mutation(int *circuit_vector, double gene_change_rate){
    int needle;
    for (int i = 0; i < num_units * 2 + 1; i++) {
        needle = 1 + rand() % 100;
        if (needle > 100 - 100 * gene_change_rate) {
            if (i == 0) {
                circuit_vector[i] = rand() % num_units;
            } else {
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

// Main function
void run_genetic_algorithm(int **all_parents, int **new_all_parents, double *performance_list, double *distribution, int num_parents, int num_units, int max_steps,
                           double gene_change_rate, double crossover_rate){

    srand(time(NULL));
    int length = num_units * 2 + 1;

    // print all the vectors
    for (int i = 0; i < num_parents; i++) {
        for (int j = 0; j < length; j++) {
            cout << all_parents[i][j] << " ";
            if ( j == length - 1){
                cout << endl;
            }
        }
    }

    int max_index, min_index; // the index of the best performance one
    double exp_performance = 10000.0; // expected performance
    double best_performance = 0.0;      // best performance in one generation
    double worst_performance;      // worst performance in one generation
    double tol = 0.1;             // a small value to make sure even the worst one has the right to reproduce
    int steps = 0;

    int *circuit_child1 = new int[length]; // potential child 1
    int *circuit_child2 = new int[length]; // potential child 2
    double t1, t2;
    double t_sum = 0;


    ofstream performance_output;

    stringstream fname;


    fname << "../performance_list"<< "_" << num_units << "_" << max_steps << "_gcr"<< gene_change_rate << "_cr" << crossover_rate << ".txt";

    performance_output.open(fname.str().c_str());

    // Out loop
    while(best_performance < exp_performance && steps < max_steps) {

        //---------------------------
        // run the circuit simulator
        // Update function
        //---------------------------
        t1 = clock();
        for (int i = 0; i < num_parents; i++){
            //the circuit simulator
            performance_list[i] = Evaluate_Circuit(all_parents[i], 1e-6, 500, num_units, 10, 100, 100.0, 500.0);
        }
        t2 = clock();

        t_sum += (t2 - t1)/CLOCKS_PER_SEC;

        // Evaluate the performance
        // find the max and min value
        find_max_min(performance_list, num_parents, max_index, min_index);

        // Choose the best one and the worst one
        best_performance = performance_list[max_index];
        worst_performance = performance_list[min_index];
        //cout << "best performance: " << best_performance << endl;
        //cout << "worst performance: " << worst_performance << endl;


        double avg = 0.0;
        for (int i = 0; i < num_parents; i++){
            avg += performance_list[i];
        }
        avg = avg / num_parents;
        //cout << "average: "<< avg;

        //----------------------------
        performance_output << best_performance << " "<< worst_performance << " " << avg <<  endl;
        //----------------------------

        // If the performance is negative
        if (worst_performance < 0){
            for (int i = 0; i < num_parents; i++){
                performance_list[i] = (performance_list[i] - worst_performance + tol);
            }
        }

        // Cast a needle to find parents
        int *pair[2];
        double sum = 0.0;
        distribution[0] = 0.0;
        for (int i = 0 ; i < num_parents; i++) {
            sum += performance_list[i];
            distribution[i+1] = sum;
        }

        // push the best one to next generation
        for (int i = 0; i < length; i++) {
            new_all_parents[0][i] = all_parents[max_index][i];
        }

        int count = 1;
        int tgt_parent_index_1;
        int tgt_parent_index_2;
        int pt;
        while (count < num_parents ){
            select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);
            pair[0] = all_parents[tgt_parent_index_1];
            pair[1] = all_parents[tgt_parent_index_2];

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
                } else {
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
                if (Check_Validity(circuit_child1)){
                    for (int i = 0; i < length; i++) {
                        new_all_parents[count][i] = circuit_child1[i];
                    }
                    count++;
                }

                // Check whether filling is finished
                if (count >= num_parents){
                    break;
                }

                // Check the second potential child
                if (Check_Validity(circuit_child2)){
                    for (int i = 0; i < length; i++) {
                        new_all_parents[count][i] = circuit_child2[i];
                    }
                    count++;
                }

        }
        swapping_parent(all_parents, new_all_parents, num_parents, length);
        steps++;
        cout << "step " << steps<< " best performance " << best_performance << endl;

    }

    performance_output << endl;
    performance_output.close();


    ofstream final_vec;

    stringstream fname_vec;

    fname_vec << "../final_vec"<< "_" << num_units << "_" << max_steps << "_gcr"<< gene_change_rate <<"_cr" << crossover_rate <<".txt";

    final_vec.open(fname.str().c_str());

    cout << "simulation function time used: " << t_sum << endl;
    cout << "best performance: "<< best_performance << endl;
    cout << "final vector: " << endl;
    for (int i = 0; i < length; i++){
        cout << all_parents[max_index][i]  << " ";
        final_vec << all_parents[max_index][i] << " ";
    }
    cout << endl;
    final_vec.close();


    //---------------
    // output for test
    ofstream testData;
    testData.open("../data.txt");
    for (int i = 0; i < length; i++){
        testData << all_parents[max_index][i] << " ";
    }
    testData.close();


    //-------------

    delete [] circuit_child1;
    delete [] circuit_child2;
}