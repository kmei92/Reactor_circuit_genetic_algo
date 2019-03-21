#include <stdio.h>
#include <cmath>
#include <math.h>
#include <iostream>
#include <algorithm>
#include "../includes/CCircuit.h"
#include "../includes/CUnit.h"
#include "../includes/Circuit_Simulator.h"

vector<CUnit> circuit;


double relative_change(double a, double b, double c, double d)
{
	//calculates the relative change of (a+b) to (c+d)
	double fin = b + a;
	double start = d + c;
	double change = abs(fin - start) / abs(start);
	return change;
}
double Evaluate_Circuit(int *circuit_vector, double tolerance, int max_iterations, int num_units, double conc, double waste, double profit, double cost)
{
	circuit.resize(num_units + 2);
	double max_change;


	// set up connection numbers
	int cnt = 0;
	int conc_index, tail_index;
	for (int i = 0; i < num_units; i++)
	{
		conc_index = circuit_vector[1 + i * 2];
		tail_index = circuit_vector[2 + i * 2];
		
		circuit[i].conc_num = conc_index;
		circuit[i].tails_num = tail_index;
	}

	// giving the outlet streams unrealistic numbers
	for (int j = 0; j < 2; j++) {
		circuit[j + num_units].conc_num = -1;
		circuit[j + num_units].tails_num = -1;
	}

	// set the initial feed to all units
	for (int i = 0; i < num_units; i++)
	{
		circuit[i].feed[0] = conc;
		circuit[i].feed[1] = waste;
	}

	int it = 0;

	int check_cnt = 0;
	do
	{
		// calculate the output streams from the feed
		for (int i = 0; i < num_units; i++)
		{
			circuit[i].calc_stream();
			circuit[i].store();
		}
		//counts number of converged units
		check_cnt = 0;
		circuit[num_units].wipe_feed();
		circuit[num_units + 1].wipe_feed();

		circuit[circuit_vector[0]].feed[0] = conc;
		circuit[circuit_vector[0]].feed[1] = waste;

		// set up the next iteration feed
		for (int i = 0; i < num_units; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				circuit[circuit[i].conc_num].feed[j] += circuit[i].top[j];
				circuit[circuit[i].tails_num].feed[j] += circuit[i].bottom[j];
			}
		}

		max_change = relative_change(circuit[0].feed[0], circuit[0].feed[1], circuit[0].old_feed[0], circuit[0].old_feed[1]);
		for (int i = 1; i < num_units; i++)
		{
			max_change = max(max_change, relative_change(circuit[i].feed[0], circuit[i].feed[1], circuit[i].old_feed[0], circuit[i].old_feed[1]));
		}

		it++;

	} while (it < max_iterations && max_change > tolerance);

	//calculating the performance
	double absolute_worst = waste * (-cost);
	double revenue = circuit[num_units].feed[0] * profit - circuit[num_units].feed[1] * cost;
	// if divergent revenue
	if (it == max_iterations)	
		revenue = absolute_worst;
	return revenue;
}