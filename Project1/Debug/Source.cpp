#include <stdio.h>
#include <cmath>
#include <math.h>
#include <iostream>
#include <time.h>
#include <fstream>


//#include "../run_genetic_algorithm.h"
#include "CCircuit.h"
#include "Genetic_Algorithm.h"

int **all_parents;
int **new_all_parents;

double gene_change_rate;
double crossover_rate;
int num_parents;
int length = 2 * num_units + 1;

double *performance_list;
double *distribution;

//int initial_step_since_no_change;
//double best_performance;


using namespace std;
//void sort_track_index(int *performance_index, double *performance_list) {
//	sort(performance_index, performance_index + num_parents,
//		[performance_list](size_t i1, size_t i2) {return performance_list[i1] < performance_list[i2]; });
//}

double get_avg(double *values_array, int counts)
{
	double sum = 0;
	for (int i = 0; i < counts; i++)
	{
		sum += values_array[i];
	}

	return sum / (double)counts;
}

int get_num_parents(int counter, int counts, bool num_parents_const, std::ofstream &convergence)
{
	int num_parents;
	if (!num_parents_const)
	{
		switch (counter)
		{
		case(0): {num_parents = 25; break; };
		case(1): {num_parents = 50; break; };
		case(2): {num_parents = 75; break; };
		case(3): {num_parents = 100; break; };
		case(4): {num_parents = 125; break; };
		case(5): {num_parents = 150; break; };
		case(6): {num_parents = 175; break; };
		case(7): {num_parents = 200; break; };
		case(8): {num_parents = 300; break; };
		case(9): {num_parents = 400; break; };
		}
	}
	else
	{
		num_parents = 100;
	}
	return num_parents;
}

double get_gene_change_rate(int counter, int counts, bool gene_change_rate_const, std::ofstream &convergence)
{
	double gene_change_rate;
	if (!gene_change_rate_const)
	{
		switch (counter)
		{
		case(0): {gene_change_rate = 0.01; break; }
		case(1): {gene_change_rate = 0.05; break; }
		case(2): {gene_change_rate = 0.10; break; }
		case(3): {gene_change_rate = 0.15; break; }
		case(4): {gene_change_rate = 0.20; break; }
		case(5): {gene_change_rate = 0.25; break; }
		case(6): {gene_change_rate = 0.30; break; }
		case(7): {gene_change_rate = 0.35; break; }
		case(8): {gene_change_rate = 0.40; break; }
		case(9): {gene_change_rate = 0.50; break; }
		}
	}
	else
	{
		gene_change_rate = 0.01;
	}
	return gene_change_rate;
}

double get_crossover_rate(int counter, int counts, bool crossover_rate_const, std::ofstream &convergence)
{
	double crossover_rate;
	if (!crossover_rate_const)
	{
		switch (counter)
		{
		case(0): {crossover_rate = 0.82; break; }
		case(1): {crossover_rate = 0.84; break; }
		case(2): {crossover_rate = 0.86; break; }
		case(3): {crossover_rate = 0.88; break; }
		case(4): {crossover_rate = 0.90; break; }
		case(5): {crossover_rate = 0.92; break; }
		case(6): {crossover_rate = 0.94; break; }
		case(7): {crossover_rate = 0.96; break; }
		case(8): {crossover_rate = 0.98; break; }
		case(9): {crossover_rate = 1.00; break; }
		}
	}
	else
	{
		crossover_rate = 0.9;
	}
	return crossover_rate;
}

int get_conc(int counter, int counts, bool conc_const, std::ofstream &convergence)
{
	int conc;
	if (!conc_const)
	{
		conc = 10 * ((double)counter / (double)counts);
	}
	else
	{
		conc = 10;
	}
	return conc;
}

int get_waste(int counter, int counts, bool waste_const, std::ofstream &convergence)
{
	int waste;
	if (!waste_const)
	{
		waste = 100 * ((double)counter / (double)counts);
	}
	else
	{
		waste = 100;
	}
	return waste;
}

double get_profit(int counter, int counts, bool profit_const, std::ofstream &convergence)
{
	double profit;
	if (!profit_const)
	{
		switch (counter)
		{
		case(0): {profit = 0.0;   break; }
		case(1): {profit = 10.0;  break; }
		case(2): {profit = 20.0;  break; }
		case(3): {profit = 50.0;  break; }
		case(4): {profit = 100.0; break; }
		case(5): {profit = 200.0; break; }
		case(6): {profit = 250.0; break; }
		case(7): {profit = 300.0; break; }
		case(8): {profit = 400.0; break; }
		case(9): {profit = 500.0; break; }
		}
	}
	else
	{
		profit = 100.0;
	}
	return profit;
}

double get_cost(int counter, int counts, bool cost_const, std::ofstream &convergence)
{
	double cost;
	if (!cost_const)
	{
		switch (counter)
		{
		case(0): {cost = 0.0;   break; };
		case(1): {cost = 10.0;  break; }
		case(2): {cost = 20.0;  break; }
		case(3): {cost = 50.0;  break; }
		case(4): {cost = 100.0; break; }
		case(5): {cost = 200.0; break; }
		case(6): {cost = 250.0; break; }
		case(7): {cost = 300.0; break; }
		case(8): {cost = 400.0; break; }
		case(9): {cost = 500.0; break; }
		}
	}
	else
	{
		cost = 500.0;
	}
	return cost;
}


int main(int argc, char *argv[])
{
	//input
	///////////////////////////////////////////////////////
	int max_steps_since_no_change = 200;	//how many maximum steps since no change before break?
	int max_steps = 1000;					// max iteration steps before break?
	int its = 3;							// run each variation how many times?
	int max_variation_step_number = 10;		// how many variations?

	for (int j = 1; j < 2; j++)
	{
		//leave which parameters constant?
		bool num_parents_const = true;
		bool gene_change_rate_const = true;
		bool crossover_rate_const = true;
		bool cost_const = true;
		bool profit_const = true;
		bool conc_const = true;
		bool waste_const = true;
		///////////////////////////////////////////////////////

		switch (j)
		{
		case(0): {break; };
		case(1): {num_parents_const = false; break; };
		case(2): {gene_change_rate_const = false; break; };
		case(3): {crossover_rate_const = false; break; };
		case(4): {cost_const = false; break; };
		case(5): {profit_const = false; break; };
		case(6): {conc_const = false; break; };
		case(7): {waste_const = false; break; };
		}

		int const_parameters = (int)num_parents_const
			+ (int)gene_change_rate_const
			+ (int)crossover_rate_const
			+ (int)cost_const
			+ (int)profit_const
			+ (int)conc_const
			+ (int)waste_const;

		std::cout << "number of constant parameters: " << const_parameters << " / 7 parameters" << std::endl;

		ofstream convergence;

		if (const_parameters == 7)
		{
			convergence.open("../original_convergence.txt", std::ios_base::app);
			convergence << "No parameter is varied." << std::endl;
		}
		else if (!num_parents_const && const_parameters == 6)
		{
			convergence.open("../num_parents_convergence.txt", std::ios_base::app);
			convergence << "The number of children is varied." << std::endl;
		}
		else if (!gene_change_rate_const && const_parameters == 6)
		{
			convergence.open("../gene_change_rate_convergence.txt", std::ios_base::app);
			convergence << "The gene change rate is varied." << std::endl;
		}
		else if (!crossover_rate_const && const_parameters == 6)
		{
			convergence.open("../crossover_rate_convergence.txt", std::ios_base::app);
			convergence << "The crossover rate is varied." << std::endl;
		}
		else if (!cost_const && const_parameters == 6)
		{
			convergence.open("../cost_convergence.txt", std::ios_base::app);
			convergence << "The cost is varied." << std::endl;
		}
		else if (!profit_const && const_parameters == 6)
		{
			convergence.open("../profit_convergence.txt", std::ios_base::app);
			convergence << "The profit is varied." << std::endl;
		}
		else if (!conc_const && const_parameters == 6)
		{
			convergence.open("../conc_convergence.txt", std::ios_base::app);
			convergence << "The cost is varied." << std::endl;
		}
		else if (!conc_const && const_parameters == 6)
		{
			convergence.open("../waste_convergence.txt", std::ios_base::app);
		}

		int max_variation_steps = -1;

		if (const_parameters == 6)
		{
			max_variation_steps = max_variation_step_number;
		}
		else if (const_parameters == 7)
		{
			max_variation_steps = 1;
		}
		else
		{
			std::cerr << "Error: Number of const parameters must be 6 or 7." << std::endl;
		}

		double *all_steps = new double[its];
		double *avg = new double[max_variation_steps];

		double cost;
		double profit;
		int conc;
		int waste;
		double tolerance = 10e-5;

		convergence << "Number of runs (per variation): " << its << std::endl;
		convergence << "Max steps: " << max_steps << std::endl;
		convergence << "Max steps since no change: " << max_steps_since_no_change << std::endl;
		convergence << "Tolerance: " << tolerance << std::endl;

		for (int variation_step = 0; variation_step < max_variation_steps; variation_step++)
		{
			for (int it = 0; it != its; it++)
			{
				int initial_step_since_no_change = 0;

				cost = get_cost(variation_step, max_variation_steps, cost_const, convergence);
				profit = get_profit(variation_step, max_variation_steps, profit_const, convergence);

				conc = get_conc(variation_step, max_variation_steps, conc_const, convergence);
				waste = get_waste(variation_step, max_variation_steps, waste_const, convergence);

				num_parents = get_num_parents(variation_step, max_variation_steps, num_parents_const, convergence);
				gene_change_rate = get_gene_change_rate(variation_step, max_variation_steps, gene_change_rate_const, convergence);
				crossover_rate = get_crossover_rate(variation_step, max_variation_steps, crossover_rate_const, convergence);

				std::cout << "num_parents: " << num_parents;
				std::cout << ", cost: " << cost;
				std::cout << ", profit: " << profit;
				std::cout << ", waste: " << waste;
				std::cout << ", conc: " << conc;
				std::cout << ", gene_change_rate: " << gene_change_rate;
				std::cout << ", crossover_rate: " << crossover_rate << std::endl;

				double best_performance = 0.0;
				int step = 0;

				int *best_vector = new int[2 * num_units + 1];

				//------------------------------------------
				// Setup an initial race with all vectors valid
				//------------------------------------------
				// Initialize the parent vector
				all_parents = new int*[num_parents];
				new_all_parents = new int*[num_parents];

				// Initialize values
				distribution = new double[num_parents + 1];
				performance_list = new double[num_parents];

				//--------------------------------------------------------
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
				//--------------------------------------------------------




				//run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units, gene_change_rate, crossover_rate);
				run_genetic_algorithm(all_parents, new_all_parents, performance_list, distribution, num_parents, num_units,
					best_performance, best_vector, step, tolerance, conc, waste, profit, cost, max_steps, max_steps_since_no_change,
					gene_change_rate, crossover_rate);


				//--------------------------------------------------------
				for (int i = 0; i < num_parents; i++) {
					delete[] all_parents[i];
					delete[] new_all_parents[i];
				}
				delete[] all_parents;
				delete[] new_all_parents;
				delete[] distribution;
				delete[] performance_list;
				//--------------------------------------------------------

				cout << "steps until convergence: " << step << ", best performance: " << best_performance << std::endl;
				cout << "best vector: " << std::endl;
				for (int i = 0; i < 2 * num_units + 1; i++)
				{
					cout << best_vector[i] << " ";
				}
				cout << std::endl;

				convergence << "steps until convergence: " << step << ", best performance: " << best_performance << std::endl;

				convergence << "best vector: " << std::endl;
				for (int i = 0; i < 2 * num_units + 1; i++)
				{
					convergence << best_vector[i] << " ";
				}
				convergence << std::endl;

				delete[] best_vector;

				//std::cout << "Needed " << initial_step_since_no_change << " iterations, ";
				//std::cout << "performance value: " << best_performance << std::endl;
				//convergence << initial_step_since_no_change << " " << best_performance << std::endl;
				//convergence << "vector: " << std::endl;

				all_steps[it] = step;
				//all_steps[it] = initial_step_since_no_change;

				//system("pause");
				std::cout << std::endl;
			}

			avg[variation_step] = get_avg(&all_steps[0], its);
			std::cout << "Average number of steps: " << avg[variation_step] << std::endl;
			convergence << "Average number of steps: " << avg[variation_step] << std::endl;
		}

		if (const_parameters == 6)
		{
			convergence << "Total avg: " << get_avg(&avg[0], max_variation_steps) << std::endl;
		}
		convergence.close();
		delete[] avg;
		delete[] all_steps;
	}
	//system("pause");
}