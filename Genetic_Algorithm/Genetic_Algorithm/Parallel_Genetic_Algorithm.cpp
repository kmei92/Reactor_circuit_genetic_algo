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
//int **all_parents;
//int **new_all_parents; // from serial
int num_parents = 1000;
int length = num_units * 2 + 1;
Cmatrix all_parents;
Cmatrix new_all_parents;

double *performance_list;
double *distribution;
bool write_to_file = false;

double simu_tol = 0.00001;
int sim_iter = 1000;
int germanium = 10;
int waste = 100;
double profit = 100.0;
double cost = 500.0;

double cnvrg_tol = 5e-1;


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

//// From serial
//void find_max_min(double *performance_list, int &max_index, int &min_index) {
//
//	double min_value = performance_list[0];
//	double max_value = performance_list[0];
//	max_index = 0;
//	min_index = 0;
//
//	for (int i = 1; i < num_parents; i++) {
//		if (performance_list[i] > max_value) {
//			max_value = performance_list[i];
//			max_index = i;
//		}
//
//		if (performance_list[i] < min_value) {
//			min_value = performance_list[i];
//			min_index = i;
//		}
//	}
//
//}
//
//// From serial
//void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2) {
//	int pt1, pt2;
//	pt1 = rand() % int(distribution[num_parents]);
//	pt2 = rand() % int(distribution[num_parents]);
//
//	while (pt1 == pt2) {
//		pt1 = rand() % int(distribution[num_parents]);
//		pt2 = rand() % int(distribution[num_parents]);
//	}
//
//	for (int j = 0; j < num_parents; j++) {
//		if (pt1 < distribution[j]) {
//			tgt_parent_index_1 = j - 1;
//			break;
//		}
//	}
//
//	for (int j = 0; j < num_parents; j++) {
//		if (pt2 < distribution[j]) {
//			tgt_parent_index_2 = j - 1;
//			break;
//		}
//	}
//
//}
//
//// From serial: whether to mutation
//bool RollingDice(double crossover_rate) {
//	int i = 1 + rand() % 100;
//	if (i < crossover_rate * 100)
//		return true;
//	else
//		return false;
//}
//
//// From serial
//
//void mutation(int i, int &current_value, double gene_change_rate) {
//	int needle;
//	needle = 1 + rand() % 100;
//	if (needle > 100 - 100 * gene_change_rate) {
//		if (i == 0) {
//			current_value = rand() % num_units;
//		}
//		else {
//			current_value = rand() % (num_units + 2);
//		}
//	}
//}
//
//// From serial: swap the old and new parents
//void swapping_parent() {
//	for (int i = 0; i < num_parents; i++) {
//		for (int j = 0; j < length; j++) {
//			int tmp;
//			tmp = all_parents.data[i][j];
//			all_parents.data[i][j] = new_all_parents.data[i][j];
//			new_all_parents.data[i][j] = tmp;
//		}
//	}
//}

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

//bool Check_Convergence(double old_best_performance, double best_performance, double tol)
//{
//	bool has_converged = false;
//	if (abs(best_performance - old_best_performance) < tol)
//	{
//		has_converged = true;
//	}
//	return has_converged;
//}

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
		if (!Check_Validity(all_parents.data[i]))
		{
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
	int send_cnt = 0;
	for (int i = 0; i < p; i++)
	{
		if (i != id)
		{
			all_parents.send_matrix_rows(i, id, &send_request[send_cnt]);
			send_cnt++;
		}
	}

	int recv_cnt = 0;
	for (int i = 0; i < p; i++)
	{

		if (i != id)
		{
			all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[recv_cnt]);
			recv_cnt++;
			count += division[i];
			all_parents.Clear_recv_Type();
		}
	}
	MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);

	// Every processor should now have all of the generated parent vectors
	// Begin Genetic Algorithm to select select new generation
	// From serial: variables
	double exp_performance = 10000.0; // expected performance
	int max_steps = 100;          // max iteration steps
	double best_performance = 10.0;      // best performance in one generation
	int best_index;               // the index of the best performance one
	double worst_performance;
	double tol = 0.1;             // a small value to make sure even the worst one has the right to reproduce
	int steps = 0;
	bool has_converged = false;

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
			for (int i = 0; i < num_parents; i++)
			{
				std::cout << performance_list[i];
			}
			send_cnt = 0; // reset send request counter
			for (int i = 0; i < p; i++)
			{
				if (i != id)
				{
					MPI_Isend(&performance_list[0], division[id], MPI_DOUBLE, i, id, MPI_COMM_WORLD, &send_request[send_cnt]);
					send_cnt++;
					std::cout << "sent to: " << i;
				}
			}
			recv_cnt = 0; // reset receive counter
			for (int i = 0; i < p; i++)
			{
				if (i != id)
				{
					MPI_Irecv(&performance_list[count], division[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD, &recv_request[recv_cnt]);
					recv_cnt++;
					count += division[i];
					std::cout << "recvd from: " << i;
				}
			}
			MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
			for (int i = 0; i < num_parents; i++)
			{
				std::cout << performance_list[i];
			}
			std::cout << "finished updating performance list" << endl;

			// Wait for all performance lists to be received before calculating indices
			// From serial: Evaluate the performance
			// find the max and min value
			int max_index, min_index;
			for (int i = 0; i < num_parents; i++)
			{
				std::cout << performance_list[i];
			}
			find_max_min(performance_list, num_parents, max_index, min_index);
			std::cout << "max index: " << max_index;
			// Choose the best and worst one
			best_performance = performance_list[max_index];
			best_index = max_index;
			worst_performance = performance_list[min_index];
			// if the performance is negative
			if (worst_performance < 0)
			{
				for (int i = 0; i < num_parents; i++)
				{
					performance_list[i] = (performance_list[i] - worst_performance + tol);
				}
			}
			std::cout << "max index: " << max_index;
			// From serial: cast a needle to find parents
			//int *pair[2];
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
				new_all_parents.data[0][i] = all_parents.data[best_index][i];
			}
			count = 1;
			while (count < division[id])
			{
				int tgt_parent_index_1;
				int tgt_parent_index_2;
				select_parent(tgt_parent_index_1, tgt_parent_index_2, distribution, num_parents);
				std::cout << "tgt_parents" << tgt_parent_index_1 << " " << tgt_parent_index_2 << std::endl;
				//pair[0] = all_parents.data[tgt_parent_index_1];
				//pair[1] = all_parents.data[tgt_parent_index_2];
				// Crossover to generate children
				int pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

				if (RollingDice()) {
					// Do cross over and Mutation
					for (int i = 0; i < length; i++) {
						// whether to do crossover

							// do crossover
						if (i < pt) {
							new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
							// Do mutation
							mutation(i, new_all_parents.data[count][i]);

							if (count + 1 < division[id]) {
								new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_2][i];
								// Do mutation
								mutation(i, new_all_parents.data[count + 1][i]);
							}

						}
						else {
							new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_2][i];
							// Do mutation
							mutation(i, new_all_parents.data[count][i]);

							if (count + 1 < division[id]) {
								new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_1][i];
								// Do mutation
								mutation(i, new_all_parents.data[count + 1][i]);
							}
						}
					}
				}
				else {
					for (int i = 0; i < length; i++) {
						// keep parents unchanged
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
						mutation(i, new_all_parents.data[count][i]);

						if (count + 1 < division[id]) {
							new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_2][i];
							mutation(i, new_all_parents.data[count + 1][i]);
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
				if (Check_Validity(new_all_parents.data[count])) {
					count++;
					if (count + 1 >= division[id]) {
						continue;
					}
				}
				else {
					if (count + 1 >= division[id]) {
						continue;
					}
					// check the second one if the first one invalid
					if (Check_Validity(new_all_parents.data[count + 1])) {
						for (int i = 0; i < length; i++) {
							new_all_parents.data[count][i] = new_all_parents.data[count + 1][i];
						}
						count++;
						continue;
					}
					else {
						continue;
					}
				}

				// Check the second one if the first one valid
				if (Check_Validity(new_all_parents.data[count])) {
					count++;
				}
			}
			std::cout << "validity check done" << endl;
			swapping_parent_parallel(division, id);

			// Now that the all_parents have been repopulated with new parents, check if the solution has converged 
			double new_performance = Evaluate_Circuit(all_parents.data[0], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
			double old_performance = Evaluate_Circuit(new_all_parents.data[0], simu_tol, sim_iter, num_units, germanium, waste, profit, cost);
			has_converged = Check_Convergence(old_performance, new_performance, cnvrg_tol);
			std::cout << "checking convergence" << endl;
			int recv_flag = 0;
			send_cnt = 0;
			if (has_converged)
			{
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						MPI_Isend(nullptr, 0, MPI_INT, i, id, MPI_COMM_WORLD, &send_request[send_cnt]);
						send_cnt++;
					}
				}
				steps++;
			}
			else // solution has not yet converged, send own parents to others and receive parents from others
			{
				send_cnt = 0;
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						all_parents.send_matrix_rows(i, id, &send_request[send_cnt]);
						send_cnt++;
					}
				}

				recv_cnt = 0;
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[recv_cnt]);
						recv_cnt++;
						count += division[i];
						all_parents.Clear_recv_Type();
					}
				}
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
			for (int i = 0; i < p; i++)
			{
				if (i != id)
				{
					MPI_Isend(&performance_list[0], division[id], MPI_DOUBLE, i, id, MPI_COMM_WORLD, &send_request[send_cnt]);
					send_cnt++;
				}
			}
			recv_cnt = 0;
			for (int i = 0; i < p; i++)
			{
				if (i != id)
				{
					MPI_Irecv(&performance_list[count], division[i], MPI_DOUBLE, i, i, MPI_COMM_WORLD, &recv_request[recv_cnt]);
					recv_cnt++;
					count += division[i];
				}
			}
			MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
			for (int i = 0; i < num_parents; i++)
			{
				std::cout << performance_list[i];
			}
			std::cout << "id: " << id << "finished updating performance list" << endl;
			int max_index, min_index;
			find_max_min(performance_list, num_parents, max_index, min_index);
			std::cout << "max index: " << max_index;
			// Choose the best and worst one
			best_performance = performance_list[max_index];
			best_index = max_index;
			worst_performance = performance_list[min_index];
			if (worst_performance < 0)
			{
				for (int i = 0; i < num_parents; i++)
				{
					performance_list[i] = (performance_list[i] - worst_performance + tol);
				}
			}

			// Wait for all performance lists to be received before calculating indices
			// From serial: cast a needle to find parents
			//int *pair[2];
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
				std ::cout << "tgt_parents" << tgt_parent_index_1 << " " << tgt_parent_index_2 << std::endl;
				//pair[0] = all_parents.data[tgt_parent_index_1];
				//pair[1] = all_parents.data[tgt_parent_index_2];

				// Crossover to generate children
				int pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

				if (RollingDice()) {
					// Do cross over and Mutation
					for (int i = 0; i < length; i++) {
						// whether to do crossover

							// do crossover
						if (i < pt) {
							new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
							// Do mutation
							mutation(i, new_all_parents.data[count][i]);

							if (count + 1 < division[id]) {
								new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_2][i];
								// Do mutation
								mutation(i, new_all_parents.data[count + 1][i]);
							}

						}
						else {
							new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_2][i];
							// Do mutation
							mutation(i, new_all_parents.data[count][i]);

							if (count + 1 < division[id]) {
								new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_1][i];
								// Do mutation
								mutation(i, new_all_parents.data[count + 1][i]);
							}
						}
					}
				}
				else {
					for (int i = 0; i < length; i++) {
						// keep parents unchanged
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
						mutation(i, new_all_parents.data[count][i]);

						if (count + 1 < division[id]) {
							new_all_parents.data[count + 1][i] = all_parents.data[tgt_parent_index_2][i];
							mutation(i, new_all_parents.data[count + 1][i]);
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
				if (Check_Validity(new_all_parents.data[count])) {
					count++;
					if (count + 1 >= division[id]) {
						continue;
					}
				}
				else {
					if (count + 1 >= division[id]) {
						continue;
					}
					// check the second one if the first one invalid
					if (Check_Validity(new_all_parents.data[count + 1])) {
						for (int i = 0; i < length; i++) {
							new_all_parents.data[count][i] = new_all_parents.data[count + 1][i];
						}
						count++;
						continue;
					}
					else {
						continue;
					}
				}

				// Check the second one if the first one valid
				if (Check_Validity(new_all_parents.data[count])) {
					count++;
				}
			}
			swapping_parent_parallel(division, id);
			MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &num_recv);

			if (num_recv != 0) // if convergence has not been determined
			{
				// Now that the all_parents have been repopulated with new parents, send and receive from other processors
				int send_cnt = 0;
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						all_parents.send_matrix_rows(i, id, &send_request[send_cnt]);
						send_cnt++;
					}
				}

				int recv_cnt = 0;
				for (int i = 0; i < p; i++)
				{
					if (i != id)
					{
						all_parents.receive_matrix_rows(i, i, count, count + division[i] - 1, &recv_request[recv_cnt]);
						recv_cnt++;
						count += division[i];
						all_parents.Clear_recv_Type();
					}
				}
				MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);
				steps++;
			}
			
		} while (num_recv != 0);
	}
	
	double end = clock();
	if (id == 0)
	{
		if (write_to_file) { all_parents.write_solution_vector(); }
		std::cout << "Total time (ms): " << (end - start) / CLOCKS_PER_SEC * 1000;
	}
	// Finished, clearing all memory
	all_parents.Clear_recv_Type();
	all_parents.Clear_send_Type();
	// delete[] pair;
	delete[] distribution;
	delete[] performance_list;
	delete[] division;
	delete[] recv_request;
	delete[] send_request;
	MPI_Finalize();
}