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

#include "Circuit_Simulator.h"
#include "Genetic_Algorithm.h"
#include "CCircuit.h"
#include "CUnit.h"

using namespace std;


int **all_parents;
int **new_all_parents;
int num_parents = 50;
double gene_change_rate = 0.01;
double crossover_rate = 0.9;

double *performance_list;
double *distribution;

/*

double UpdateFunction(int *vec, int size){
    int sum = 0;
    double std, avg;
    for (int i = 0; i < size; i++){
        sum += vec[i];
    }
    avg = sum / size;
    for (int i = 0; i < size; i++){
        std += pow(abs(vec[i] - avg), 2.0);
    }
    std = pow(std, 0.5);
    return std;
}
 */


int main(int argc, char *argv[]) {

/*
    srand(time(NULL));
    // Initialize the parent vector
    all_parents = new int*[num_parents];
    new_all_parents = new int*[num_parents];

    // Initialize values
    distribution= new double[num_parents + 1];
    performance_list = new double[num_parents];


    bool flag_right = true;
    // Initialize values for parents
    for (int i = 0; i < num_parents; i++) {
        if (flag_right) {
            all_parents[i] = new int[2 * num_units + 1];
            new_all_parents[i] = new int[2 * num_units + 1];
        }
        all_parents[i][0] = rand() % num_units;

        for (int j = 1; j < 2 * num_units + 1; j += 2) {
            int index = j / 2;
            int top = rand() % (num_units + 2);
            int bot = rand() % (num_units + 2);
            while (top == index || bot == index || top == bot) {
                top = rand() % (num_units + 2);
                bot = rand() % (num_units + 2);
            }
            all_parents[i][j] = top;
            all_parents[i][j + 1] = bot;
            //cout << all_parents[i][j]  << " "<< all_parents[i][j + 1]<< " ";
        }

        // Check if the randomly generated vector is valid
        if(!Check_Validity(all_parents[i])){
            i--;
            flag_right = false;
            cout << "i "<< i << endl;
        }else{
            cout << "valid!!! " <<endl;
            flag_right = true;
        }

    }

    // print all the vectors
    for (int i = 0; i < num_parents; i++) {
        for (int j = 0; j < length; j++) {
            cout << all_parents[i][j] << " ";
            if ( j == length - 1){
                cout << endl;
            }
        }
    }


    double exp_performance = 10000.0; // expected performance
    int max_steps = 1000;          // max iteration steps
    double best_performance = 10.0;      // best performance in one generation
    int best_index;               // the index of the best performance one
    double worst_performance;
    double tol = 0.1;             // a small value to make sure even the worst one has the right to reproduce
    int steps = 0;


    ofstream performance_output;
    performance_output.open("../performance_list.txt");

    // Out loop
    while(best_performance < exp_performance && steps < max_steps) {

        //---------------------------
        // run the circuit simulator
        // Update function
        //---------------------------
        for (int i = 0; i < num_parents; i++){
            //performance_list[i] = UpdateFunction(all_parents[i], length);
            performance_list[i] = Evaluate_Circuit(all_parents[i], 0.00001, 30, num_units, 10, 100, 100.0, 500.0);
            //cout << "i  " << i << endl;
        }
        cout << "Performance: ";
        for (int i =0 ;i < num_parents; i++){
            cout << performance_list[i] << " ";
        }
        cout << endl;

        // Evaluate the performance
        // find the max and min value
        int max_index, min_index;
        find_max_min(performance_list, num_parents, max_index, min_index);
        //sort_track_index(test, performance_list);
        //sort(performance_list, performance_list + num_parents);

        // Choose the best one and the worst one
        best_performance = performance_list[max_index];
        best_index = max_index;
        worst_performance = performance_list[min_index];
        cout << "best performance: " << best_performance << endl;
        cout << "worst performance: " << worst_performance << endl;


        double avg = 0.0;
        for (int i = 0; i < num_parents; i++){
            avg += performance_list[i];
        }
        avg = avg / num_parents;
        cout << "average: "<< avg;

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
            new_all_parents[0][i] = all_parents[best_index][i];
        }

        int count = 1;
        int tgt_parent_index_1;
        int tgt_parent_index_2;
        int pt;
        while (count < num_parents ){
            select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);
            pair[0] = all_parents[tgt_parent_index_1];
            pair[1] = all_parents[tgt_parent_index_2];

            // Crossover to generate children
            pt = rand() % length;
            while (pt == 0 || pt == length - 1) {
                pt = rand() % length;
            }

            // Do cross over and Mutation
            for (int i = 0; i < length; i++) {
                // whether to do crossover
                if(RollingDice()) {
                    // do crossover
                    if (i < pt) {
                        new_all_parents[count][i] = pair[0][i];
                        // Do mutation
                        mutation(i, new_all_parents[count][i]);

                        if (count + 1 < num_parents) {
                            new_all_parents[count + 1][i] = pair[1][i];
                            // Do mutation
                            mutation(i, new_all_parents[count + 1][i]);
                        }

                    } else {
                        new_all_parents[count][i] = pair[1][i];
                        // Do mutation
                        mutation(i, new_all_parents[count][i]);

                        if (count + 1 < num_parents) {
                            new_all_parents[count + 1][i] = pair[0][i];
                            // Do mutation
                            mutation(i, new_all_parents[count + 1][i]);
                        }
                    }
                }else{
                    // keep parents unchanged
                    new_all_parents[count][i] = pair[0][i];
                    mutation(i, new_all_parents[count][i]);

                    if (count + 1 < num_parents) {
                        new_all_parents[count + 1][i] = pair[1][i];
                        mutation(i, new_all_parents[count + 1][i]);
                    }
                }
            }
            //---------------------------------------------------
            // Check the two new children
            // if 1st and 2nd one are both valid, count = count + 2
            // if 1st valid and 2nd invalid, count++
            // if 1st invalid and 2nd valid, give the values of 2nd to the 1st, then count++
            // if both invalid, continue
            //---------------------------------------------------
            if (Check_Validity(new_all_parents[count])){
                count++;
                if (count + 1 >= num_parents){
                    continue;
                }
            }else{
                if (count + 1 >= num_parents){
                    continue;
                }
                // check the second one if the first one invalid
                if (Check_Validity(new_all_parents[count+1])){
                    for (int i = 0; i < length; i++){
                        new_all_parents[count][i] = new_all_parents[count+1][i];
                    }
                    count++;
                    continue;
                }else{
                    continue;
                }
            }

            // Check the second one if the first one valid
            if (Check_Validity(new_all_parents[count+1])){
                count++;
            }

        }
        swapping_parent(all_parents, new_all_parents, num_parents, length);
        steps++;
        cout << "step " << steps<< endl;

    }

    performance_output << endl;
    performance_output.close();
    ofstream final_vec;
    final_vec.open("../data.txt");

    cout << "final vector: " << endl;
    for (int i = 0; i < length; i++){
        cout << all_parents[best_index][i] << " ";
        final_vec << all_parents[best_index][i] << " ";
    }
    cout << endl;
    final_vec.close();

    for (int i = 0; i < num_parents; i++) {
        delete[] all_parents[i];
        delete[] new_all_parents[i];
    }

    delete[] all_parents;
    delete[] new_all_parents;
    delete [] distribution;
    delete [] performance_list;
    */
double t1 = clock();
    run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units);
	double t2 = clock();
	cout << "Time used: " << (t2 - t1) / CLOCKS_PER_SEC << " s " << endl;
	system("pause");
}





