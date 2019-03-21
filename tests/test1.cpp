#include <iostream>
#include "CUnit.h"
#include "CCircuit.h"
#include "Genetic_Algorithm.h"
//#include "Genetic_Algorithm.cpp"



int main(int argc, char * argv[]){

		int valid[21] = { 0,1,2,3,4,5,6,7,8,9,10,10,11,10,11,10,11,10,11,10,11 };
        int invalid[3] = {0, 2, 2};
        int OneValid[7] = {0, 1, 2, 3, 0, 0, 4};
	std::cout << "Check_Validity({0,1,2}):\n";

        if (Check_Validity(valid))
	    std::cout  << "pass\n";
	else
	    std::cout << "fail\n";

		int **all_parents;
		int **new_all_parents;
		int num_parents = 100;
		double gene_change_rate = 0.01;
		double crossover_rate = 0.9;

		double *performance_list;
		double *distribution;


		run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units);



/*      
	std::cout << "Check_Validity({0,2,2}):\n";
        if (Check_Validity(invalid))
            std::cout << "fail\n";
        else
            std::cout << "pass\n";
*/

}
