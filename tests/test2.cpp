#include <cmath>
#include <iostream>

#include "CUnit.h"
#include "CCircuit.h"
#include "Genetic_Algorithm.h"
#include "Circuit_Simulator.h"

int main(int argc, char * argv[])
{
      int vec1[2*num_units+1] = {0, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 9,
				10, 11, 10, 11, 10, 11, 10, 11}  ;

      int vec2[2*num_units+1] = {0, 1, 11, 2, 11, 3, 11, 4, 11, 5, 11, 6, 11,
				7, 11, 8, 11, 9, 11, 10, 11}  ;

      std::cout << "Evaluate_Circuit(vec1, 0.0, 1) close to 42000.0:\n";
	  /*
      if (std::fabs(Evaluate_Circuit(vec1, 0.0, 1)-42000.0)<1.0e-8)
	        std::cout << "pass\n";
      else
	        std::cout << "fail\n";


      std::cout << "Evaluate_Circuit(vec2, 0.0, 1) close to 42000.0:\n";
      if (std::fabs(Evaluate_Circuit(vec2, 0.0, 1 - 37500.0)>1.0e-8))
	        std::cout << "pass\n";
      else
	        std::cout << "fail";
			*/
	  int **all_parents;
	  int **new_all_parents;
	  int num_parents = 100;
	  double gene_change_rate = 0.01;
	  double crossover_rate = 0.9;

	  double *performance_list;
	  double *distribution;

	  for (int i = 0; i < num_parents; i++) {
		  all_parents[i] = new int[2 * num_units + 1];
		  new_all_parents[i] = new int[2 * num_units + 1];
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
	  }


	  run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units);
	
}
