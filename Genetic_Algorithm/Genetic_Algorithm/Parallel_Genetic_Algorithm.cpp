#include <iostream>
#include <stdio.h>
#include <cmath>
#include <mpi.h>
#include <time.h>
#include "../../includes/CCircuit.h"
#include "../../includes/Genetic_Algorithm.h"
#include "../../includes/Cmatrix.h"
#include "../../includes/Circuit_Simulator.h"

using namespace std;

int id, p;
int *division;			// processor divisions
int num_parents = 100;
int length = num_units * 2 + 1;
Cmatrix all_parents;
Cmatrix new_all_parents;

double *performance_list;
double *distribution;
int *null_list;
bool write_to_file = true;

double simu_tol = 0.00001;
int sim_iter = 500;
int germanium = 10;
int waste = 100;
double profit = 100.0;
double cost = 500.0;

double cnvrg_tol = 1.0;


// Function to split processors into evenly divided tasks
void split_group(int *divisions) {

	int remaining = num_parents;
	for (int i = 0; i < p; i++)
	{
		int allocated = remaining / (p - i);
		divisions[i] = allocated;
		remaining -= allocated;
	}
}

// From serial: swap the old and new parents
void swapping_parent_parallel(int *division, int id) {
	for (int i = 0; i < division[id]; i++) {
		for (int j = 0; j < length; j++) {
			int tmp;
			tmp = all_parents.data[i][j];
			all_parents.data[i][j] = new_all_parents.data[i][j];
			new_all_parents.data[i][j] = tmp;
		}
	}
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	double start =  clock();
	// divide up the work among processors to create parent vectors
	division = new int[p];
	split_group(division);
	
	// initialize the parent vectors
	all_parents.Set_Size(num_parents, length);
	new_all_parents.Set_Size(division[id], length);

	// initialize tracking vectors
	distribution = new double[num_parents + 1];
	performance_list = new double[num_parents];
	

	// fill in only the necessary portions of the parent vectors in each processor
	// PARALLEL PORTION
	int count = 0;
	for (int i = 0; i < division[id]; i++)
	{
		all_parents.data[i][0] = rand() % num_units;
		for (int j = 1; j < 2 * num_units + 1; j += 2)
		{
			// generate random vector
			int index = j / 2;
			int top = rand() % (num_units + 2);
			int bot = rand() % (num_units + 2);
			while (top == index || bot == index || top == bot)
			{
				top = rand() % (num_units + 2);
				bot = rand() % (num_units + 2);
			}
			all_parents.data[i][j] = top;
			all_parents.data[i][j + 1] = bot;
		}
		// Check validity of the generated vector
		if (!Check_Validity(all_parents.data[i]))
		{ // if not valid, redo the current vector
			i--;
		}
		else {
			count++;
		}
	}
	// At this point, every processor has created its respective parent vectors
	// Begin receiving parent vectors from every other processor
	MPI_Request *recv_request = new MPI_Request[p - 1];
	MPI_Request *send_request = new MPI_Request[p - 1];
	all_parents.setup_MPI_row_send(0, division[id]-1); // setup the send data type on each processor
	int recv_flag = 0;
	int recv_cnt = 0;
	int send_cnt = 0;
	// receive parents from other processors and send own parents to other processors
	for (int i = 0; i < p; i++)
	{
		if (i != id)
		{
			all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[recv_cnt]);
			recv_cnt++;
			count += division[i];
			all_parents.Clear_recv_Type();
			all_parents.send_matrix_rows(i, id, &send_request[send_cnt]);
			send_cnt++;
		}
	}
	MPI_Waitall(p - 1, send_request, MPI_STATUSES_IGNORE);
	MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);

	// Every processor should now have all of the generated parent vectors
	// Begin Genetic Algorithm to select new generation of parents
	// From serial: variables
	int steps = 0;
	int initial_steps_since_no_change = 0;
	int max_steps = 500;
	double expected_performance = 165.5;	// expected performance used for convergence
	double best_performance = 10.0;      // best performance in one generation             
	double worst_performance;
	double tol = 0.1;             // a small value to make sure even the worst one has the right to reproduce
	bool has_converged = false;

	int *circuit_child1 = new int[length]; // potential child 1
	int *circuit_child2 = new int[length]; // potential child 2

	if (id == 0)
	{
		do
		{
			// EVERY PROCESSOR HAS TO UNDERSTAND THE PERFORMANCE OF THE CURRENT GENERATION
			//---------------------------
			// run the circuit simulator for each processor's own all_parents.data and then send to the other processors
			// Update function
			//---------------------------
			count = 0; // reset the counter
			for (int i = 0; i < division[id]; i++)
			{
				performance_list[i] = Evaluate_Circuit(all_parents.data[i], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
				count++;
			}

			for (int i = 1; i < p; i++)
			{
				MPI_Irecv(&performance_list[count], division[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD, &recv_request[i - 1]);
				count += division[i];
				MPI_Isend(&performance_list[0], division[id], MPI_DOUBLE, i, id, MPI_COMM_WORLD, &send_request[i - 1]);
			}
			MPI_Waitall(p - 1, send_request, MPI_STATUSES_IGNORE);
			MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);

			// Wait for all performance lists to be received before calculating indices
			// find the max and min value
			int max_index, min_index;
			find_max_min(performance_list, num_parents, max_index, min_index);
			// Choose the best and worst one
			best_performance = performance_list[max_index];
			worst_performance = performance_list[min_index];
			// if the performance is negative
			if (worst_performance < 0)
			{
				for (int i = 0; i < num_parents; i++)
				{
					performance_list[i] = (performance_list[i] - worst_performance + tol);
				}
			}

			double sum = 0.0;
			distribution[0] = 0.0;
			for (int i = 0; i < num_parents; i++)
			{
				sum += performance_list[i];
				distribution[i + 1] = sum;
			}

			// PARALLEL PORTION - EACH PROCESSOR GENERATES NEW GENERATION AND THEN COMMUNICATES WITH EACH OTHER
			// Only main processor will select the best parents to push to the next generation
			for (int i = 0; i < length; i++)
			{
				new_all_parents.data[0][i] = all_parents.data[max_index][i];
			}
			count = 1;
			int tgt_parent_index_1;
			int tgt_parent_index_2;
			int pt;
			while (count < division[id])
			{

				select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);
				// Crossover to generate children
				pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

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
						circuit_child1[i] = all_parents.data[tgt_parent_index_1][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_2][i];

					}
					// the part after the break point
					for (int i = pt; i < length; i++) {
						circuit_child1[i] = all_parents.data[tgt_parent_index_2][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_1][i];

					}
				}
				else {
					// keep parents unchanged
					for (int i = 0; i < length; i++) {
						circuit_child1[i] = all_parents.data[tgt_parent_index_1][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_2][i];
					}
				}
				// Do mutation
				mutation(circuit_child1);
				mutation(circuit_child2);

				// Check the first potential child
				if (Check_Validity(circuit_child1)) {
					for (int i = 0; i < length; i++) {
						new_all_parents.data[count][i] = circuit_child1[i];
					}
					count++;
				}

				// Check whether filling is finished
				if (count >= division[id]) {
					break;
				}

				// Check the second potential child
				if (Check_Validity(circuit_child2)) {
					for (int i = 0; i < length; i++) {
						new_all_parents.data[count][i] = circuit_child2[i];
					}
					count++;
				}
			}
			swapping_parent_parallel(division, id);
			// Now that the all_parents have been repopulated with new parents, check if the solution has converged 
			double new_performance = Evaluate_Circuit(all_parents.data[0], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
			double old_performance = Evaluate_Circuit(new_all_parents.data[0], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
			cout << new_performance << endl;
			// check if we have arrived at the expected performance value
			has_converged = Check_Convergence(expected_performance, new_performance, cnvrg_tol, steps, max_steps);
			cout << has_converged << endl;
			cout << steps << endl;
			if (has_converged) // tell other processors to stop
			{
				for (int i = 1; i < p; i++)
				{
					MPI_Isend(nullptr, 0, MPI_INT, i, id, MPI_COMM_WORLD, &send_request[i - 1]);
					send_cnt++;
				}
				steps++;
			}
			else // solution has not yet converged, send own parents to others and receive parents from others
			{
				for (int i = 1; i < p; i++)
				{
					all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[i - 1]);
					count += division[i];
					all_parents.Clear_recv_Type();
					all_parents.send_matrix_rows(i, id, &send_request[i - 1]);
				}
				MPI_Waitall(p - 1, send_request, MPI_STATUSES_IGNORE);
				MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
				steps++;
			}
		} while (!has_converged);
	}
	else
	{
		MPI_Status status;
		int num_recv;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &num_recv);
		do
		{
			// EVERY PROCESSOR HAS TO UNDERSTAND THE PERFORMANCE OF THE CURRENT GENERATION
			//---------------------------
			// run the circuit simulator for each processor's own all_parents.data and then send to the other processors
			// Update function
			//---------------------------
			count = 0; // reset the counter
			for (int i = 0; i < division[id]; i++)
			{
				performance_list[i] = Evaluate_Circuit(all_parents.data[i], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
				count++;
			}
			send_cnt = 0; // reset send request counter
			recv_cnt = 0; // reset recv request counter
			for (int i = 0; i < p; i++)
			{
				if (i != id)
				{
					MPI_Irecv(&performance_list[count], division[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD, &recv_request[recv_cnt]);
					recv_cnt++;
					count += division[i];
					MPI_Isend(&performance_list[0], division[id], MPI_DOUBLE, i, id, MPI_COMM_WORLD, &send_request[send_cnt]);
					send_cnt++;
				}
			}
			MPI_Waitall(p - 1, send_request, MPI_STATUSES_IGNORE);
			MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
			
			int max_index, min_index;
			find_max_min(performance_list, num_parents, max_index, min_index);
			// Choose the best and worst one
			best_performance = performance_list[max_index];
			worst_performance = performance_list[min_index];
			if (worst_performance < 0)
			{
				for (int i = 0; i < num_parents; i++)
				{
					performance_list[i] = (performance_list[i] - worst_performance + tol);
				}
			}

			// Wait for all performance lists to be received before calculating indices
			double sum = 0.0;
			distribution[0] = 0.0;
			for (int i = 0; i < num_parents; i++)
			{
				sum += performance_list[i];
				distribution[i + 1] = sum;
			}
			count = 0;
			while (count < division[id])
			{
				int tgt_parent_index_1;
				int tgt_parent_index_2;
				select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);

				// Crossover to generate children
				int pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

				if (RollingDice()) {

					// Crossover to generate children
					pt = rand() % length;
					while (pt == 0 || pt == length - 1) {
						pt = rand() % length;
					}
					// the part before the break point
					for (int i = 0; i < pt; i++) {
						circuit_child1[i] = all_parents.data[tgt_parent_index_1][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_2][i];

					}
					// the part after the break point
					for (int i = pt; i < length; i++) {
						circuit_child1[i] = all_parents.data[tgt_parent_index_2][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_1][i];

					}
				}
				else {
					// keep parents unchanged
					for (int i = 0; i < length; i++) {
						circuit_child1[i] = all_parents.data[tgt_parent_index_1][i];
						circuit_child2[i] = all_parents.data[tgt_parent_index_2][i];
					}
				}
				// Do mutation
				mutation(circuit_child1);
				mutation(circuit_child2);

				// Check the first potential child
				if (Check_Validity(circuit_child1)) {
					for (int i = 0; i < length; i++) {
						new_all_parents.data[count][i] = circuit_child1[i];
					}
					count++;
				}

				// Check whether filling is finished
				if (count >= division[id]) {
					break;
				}

				// Check the second potential child
				if (Check_Validity(circuit_child2)) {
					for (int i = 0; i < length; i++) {
						new_all_parents.data[count][i] = circuit_child2[i];
					}
					count++;
				}
			}
			swapping_parent_parallel(division, id);
			MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &num_recv);
			if (num_recv != 0) // if convergence has not been determined by processor 0, continue sending and receiving
			{
				// Now that the all_parents have been repopulated with new parents, send and receive from other processors
				send_cnt = 0;
				recv_cnt = 0;
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[recv_cnt]);
						recv_cnt++;
						count += division[i];
						all_parents.Clear_recv_Type();
						all_parents.send_matrix_rows(i, id, &send_request[send_cnt]);
						send_cnt++;
					}
				}
				MPI_Waitall(p - 1, send_request, MPI_STATUSES_IGNORE);
				MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
			}
			
		} while (num_recv != 0);
	}
	
	double end = clock();
	if (id == 0)
	{
		if (write_to_file) { all_parents.write_solution_vector(); }
		std::cout << steps << endl;
		std::cout << "performance: " << Evaluate_Circuit(all_parents.data[0], simu_tol, sim_iter, num_units, germanium, waste, profit, cost) << endl;
		std::cout << "Total time (ms): " << (end - start) / CLOCKS_PER_SEC * 1000;
	}
	// Finished, clearing all memory
	all_parents.Clear_recv_Type();
	all_parents.Clear_send_Type();
	delete[] circuit_child1;
	delete[] circuit_child2;
	delete[] distribution;
	delete[] performance_list;
	delete[] division;
	delete[] recv_request;
	delete[] send_request;
	MPI_Finalize();
}