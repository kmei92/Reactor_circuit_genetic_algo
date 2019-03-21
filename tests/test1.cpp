//#include <iostream>
//#include "CUnit.h"
//#include "CCircuit.h"
//
//
///*
//int main(int argc, char * argv[]){
///*
//        int valid[3] = {0, 1, 2};
//        int invalid[3] = {0, 2, 2};
//        int OneValid[7] = {0, 1, 2, 3, 0, 0, 4};
//	std::cout << "Check_Validity({0,1,2}):\n";*/
//
//        if (Check_Validity(invalid))
//	    std::cout  << "pass\n";
//	else
//	    std::cout << "fail\n";
//
//
//
///     
//	std::cout << "Check_Validity({0,2,2}):\n";
//        if (Check_Validity(invalid))
//            std::cout << "fail\n";
//        else
//            std::cout << "pass\n";
//
//
//}
//*/

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
int max_steps = 1000;
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

	//------------------------------------------
	// Setup an initial race with all vectors valid
	//------------------------------------------
	// Initialize the parent vector
	all_parents = new int*[num_parents];
	new_all_parents = new int*[num_parents];

	// Initialize values
	distribution = new double[num_parents + 1];
	performance_list = new double[num_parents];


	int length = num_units * 2 + 1;
	bool flag_right = true;
	int index, top, bot;
	// Initialize values for parents
	for (int i = 0; i < num_parents; i++) {
		if (flag_right) {
			all_parents[i] = new int[length];
			new_all_parents[i] = new int[length];
		}
		all_parents[i][0] = rand() % num_units;

		for (int j = 1; j < length; j += 2) {
			index = j / 2;
			top = rand() % (num_units + 2);
			bot = rand() % (num_units + 2);
			while (top == index || bot == index || top == bot) {
				top = rand() % (num_units + 2);
				bot = rand() % (num_units + 2);
			}
			all_parents[i][j] = top;
			all_parents[i][j + 1] = bot;
		}

		// Check if the randomly generated vector is valid
		if (!Check_Validity(all_parents[i])) {
			i--;
			flag_right = false;
		}
		else {
			flag_right = true;
		}

	}
	//------------------------------------------


	//------------------------------------------
	// Run the GA with circuit simulation
	//------------------------------------------
	double start = clock();
	run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units, max_steps);
	double end = clock();
	cout << "Time used: " << (end - start) / CLOCKS_PER_SEC << endl;



	for (int i = 0; i < num_parents; i++) {
		delete[] all_parents[i];
		delete[] new_all_parents[i];
	}
	delete[] all_parents;
	delete[] new_all_parents;
	delete[] distribution;
	delete[] performance_list;




	int a[num_units * 2 + 1];
	fstream in("../data.txt");
	cin.rdbuf(in.rdbuf());
	for (int i = 0; i < (num_units * 2 + 1); i++)
		cin >> a[i];
	if (Check_Validity(a))
		std::cout << "pass\n";
	else
		std::cout << "fail\n";









}


