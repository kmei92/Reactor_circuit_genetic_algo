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
#include "Genetic_Algorithm.cpp"
#include "CCircuit.h"
#include "CUnit.h"
#include "CUnit.cpp"
#include "CCircuit.cpp"


using namespace std;

int **all_parents;
int **new_all_parents;
int num_parents = 50;
int length = num_units * 2 + 1;
double gene_change_rate = 0.01;
double mutation_rate = 0.9;

double *performance_list;
int *performance_index;
double *distribution;

void sort_track_index(int *performance_index, double *performance_list){
    sort(performance_index, performance_index + num_parents,
         [performance_list](size_t i1, size_t i2) {return performance_list[i1] < performance_list[i2];});
}

void find_max_min(double *performance_list, int &max_index, int &min_index){

    double min_value = performance_list[0];
    double max_value = performance_list[0];
    max_index = 0;
    min_index = 0;

    for (int i = 1; i < num_parents; i++){
        if (performance_list[i] > max_value){
            max_value = performance_list[i];
            max_index = i;
        }

        if (performance_list[i] < min_value){
            min_value = performance_list[i];
            min_index = i;
        }
    }

}


void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2){
    int pt1, pt2;
    pt1 = rand() % int(distribution[num_parents]);
    pt2 = rand() % int(distribution[num_parents]);

    while(pt1 == pt2) {
        pt1 = rand() % int(distribution[num_parents]);
        pt2 = rand() % int(distribution[num_parents]);
    }

    for (int j = 0; j < num_parents; j++) {
        if (pt1 < distribution[j]) {
            tgt_parent_index_1 = j - 1;
            break;
        }
    }

    for (int j = 0; j < num_parents; j++) {
        if (pt2 < distribution[j]) {
            tgt_parent_index_2 = j - 1;
            break;
        }
    }

}


// whether to mutation
bool RollingDice(){
    int i = 1 + rand() % 100;
    if (i < mutation_rate * 100)
        return true;
    else
        return false;
}

void mutation(int count, int i){
    int needle;
    needle = 1 + rand() % 100;
    if (needle > 100 - 100 * gene_change_rate) {
        if (i == 0 ){
            new_all_parents[count][i] = rand() % num_units;
        }else {
            new_all_parents[count][i] = rand() % (num_units + 2);
        }
    }
}

// swap the the old and new parents
void swapping_parent(){
    for (int i = 0; i < num_parents; i++){
        for (int j = 0; j < length; j++){
            int tmp;
            tmp = all_parents[i][j];
            all_parents[i][j] = new_all_parents[i][j];
            new_all_parents[i][j] = tmp;
        }
    }
}
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


    srand(time(NULL));
    // Initialize the parent vector
    all_parents = new int*[num_parents];
    new_all_parents = new int*[num_parents];

    // Initialize values
    distribution= new double[num_parents + 1];
    performance_list = new double[num_parents];
    performance_index = new int[num_parents];

    int ttest[21] = {0,1,2,3,4,5,6,7,8,9,10,10,11,10,11,10,11,10,11,10,11};

    cout << "check value: "<< Check_Validity(ttest)<<  endl;

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
        find_max_min(performance_list, max_index, min_index);
        //sort_track_index(test, performance_list);
        //sort(performance_list, performance_list + num_parents);

        // Choose the best one and the worst one
        best_performance = performance_list[max_index];

        //----------------------------
        performance_output << best_performance << endl;
        //----------------------------

        cout << "best performance: " << best_performance << endl;
        best_index = max_index;
        worst_performance = performance_list[min_index];
        cout << "worst performance: " << worst_performance << endl;

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
        while (count < num_parents ){

            int tgt_parent_index_1;
            int tgt_parent_index_2;
            select_parent(tgt_parent_index_1, tgt_parent_index_2);
            pair[0] = all_parents[tgt_parent_index_1];
            pair[1] = all_parents[tgt_parent_index_2];

            // Crossover to generate children
			int pt = rand() % length;
            while (pt == 0 || pt == length - 1) {
                pt = rand() % length;
            }

            // Do cross over and Mutation
            for (int i = 0; i < length;i++) {
                if (i < pt) {
                    new_all_parents[count][i] = pair[0][i];
                    // Decide whether put to mutation
                    if(RollingDice()) {
                        // Do mutation
                        mutation(count, i);
                    }
                }else{
                    new_all_parents[count][i] = pair[1][i];
                    if(RollingDice()) {
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
            if (Check_Validity(new_all_parents[count])){
                count++;
            }


        }
        swapping_parent();
        steps++;
        cout << "step " << steps<< endl;

    }

    cout << endl;
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
    delete [] performance_index;
}





