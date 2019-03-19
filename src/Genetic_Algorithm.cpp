#include <stdio.h>
#include <cmath>
#include <math.h>
#include <iostream>

#include "../includes/CCircuit.h"
#include "../includes/CUnit.h"
#include "../includes/Genetic_Algorithm.h"

#include <iostream>
#include <math.h>


double relative_change(double a, double b, double c, double d)
{
	double fin = b + a;
	double start = d + c;
	double change = abs(fin - start) / abs(start);
	return change;
}

int Evaluate_Circuit(int *circuit_vector, double tolerance, int max_iterations, int num_units, double conc, double waste, double profit, double cost)
{
	CUnit *circuit = new CUnit[num_units + 2];

	// set up connection numbers
	int cnt = 0;
	for (int i = 1; i < 2 * num_units + 1; i += 2)
	{
		circuit[cnt].conc_num = circuit_vector[i];
		circuit[cnt].tails_num = circuit_vector[i + 1];
		circuit[circuit[cnt].conc_num].conc_count++;
		circuit[circuit[cnt].tails_num].tail_count++;
		circuit[circuit[cnt].conc_num].conc_inlets.push_back(cnt);
		circuit[circuit[cnt].tails_num].tails_inlets.push_back(cnt);
		cnt++;
	}


	// set up the recycle inlets
	for (int i = 0; i < num_units + 2; i++)
	{
		circuit[i].conc_inlets.resize(circuit[i].conc_count);
		circuit[i].tails_inlets.resize(circuit[i].tail_count);
	}

	// set the initial feed to all units and calculate first output
	for (int i = 0; i < num_units; i++)
	{
		circuit[i].feed[0] = conc;
		circuit[i].feed[1] = waste;
		circuit[i].calc_stream();
		circuit[i].store();
		circuit[i].wipe_feed();
	}

	int it = 0;
	bool has_converged = false;

	int check_cnt = 0;
	do
	{
		check_cnt = 0;
		circuit[num_units].wipe_feed();
		circuit[num_units + 1].wipe_feed();

		// set up the next iteration feed
		for (int i = 0; i < num_units + 2; i++)
		{
			if (i == circuit_vector[0]) // identify as circuit feed
			{
				circuit[i].feed[0] += conc;
				circuit[i].feed[1] += waste;
			}

			for (int j = 0; j < circuit[i].conc_inlets.size(); j++)
			{
				circuit[i].feed[0] += circuit[circuit[i].conc_inlets[j]].top[0];
				circuit[i].feed[1] += circuit[circuit[i].conc_inlets[j]].top[1];
			}
			for (int j = 0; j < circuit[i].tails_inlets.size(); j++)
			{
				circuit[i].feed[0] += circuit[circuit[i].tails_inlets[j]].bottom[0];
				circuit[i].feed[1] += circuit[circuit[i].tails_inlets[j]].bottom[1];
			}
		}

		int check_cnt = 0;
		for (int i = 0; i < num_units; i++)
		{
			double change = relative_change(circuit[i].feed[0], circuit[i].feed[1], circuit[i].old_feed[0], circuit[i].old_feed[1]);
			std::cout << change << endl;
			if (change < tolerance)
				check_cnt++;
		}

		it++;

		// calculate the output streams
		for (int i = 0; i < num_units; i++)
		{
			circuit[i].calc_stream();
			circuit[i].store();
			circuit[i].wipe_feed();
		}

	} while (it < max_iterations && check_cnt < num_units);

	//calculating the performance
	double absolute_worst = waste * (- cost);
	double revenue = circuit[num_units].feed[0] * profit - circuit[num_units].feed[1] * cost;
	int performance = (int)((revenue - absolute_worst)/profit);

	delete[] circuit;
	return performance;
}