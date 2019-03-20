#include <iostream>
#include <stdio.h>
#include <cmath>
#include <mpi.h>
#include <time.h>
#include "../../includes/CCircuit.h"
#include "../../includes/Genetic_Algorithm.h"
#include "../../includes/Cmatrix.h"

using namespace std;

int id, p;
int *division;			// processor divisions
//int **all_parents;
//int **new_all_parents; // from serial
int num_parents = 100;
int length = num_units * 2 + 1;
Cmatrix all_parents;
Cmatrix new_all_parents;
double gene_change_rate = 0.01; // from serial
double mutation_rate = 0.9; // from serial

double *performance_list;
int *performance_index;
double *distribution;

bool write_to_file = false;

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

// From serial
void find_max_min(double *performance_list, int &max_index, int &min_index) {

	double min_value = performance_list[0];
	double max_value = performance_list[0];
	max_index = 0;
	min_index = 0;

	for (int i = 1; i < num_parents; i++) {
		if (performance_list[i] > max_value) {
			max_value = performance_list[i];
			max_index = i;
		}

		if (performance_list[i] < min_value) {
			min_value = performance_list[i];
			min_index = i;
		}
	}

}

// From serial
void select_parent(int &tgt_parent_index_1, int &tgt_parent_index_2) {
	int pt1, pt2;
	pt1 = rand() % int(distribution[num_parents]);
	pt2 = rand() % int(distribution[num_parents]);

	while (pt1 == pt2) {
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

// From serial: whether to mutation
bool RollingDice() {
	int i = 1 + rand() % 100;
	if (i < mutation_rate * 100)
		return true;
	else
		return false;
}

// From serial
void mutation(int count, int i) {
	int needle;
	needle = 1 + rand() % 100;
	if (needle > 100 - 100 * gene_change_rate) {
		if (i == 0) {
			new_all_parents.data[count][i] = rand() % num_units;
		}
		else {
			new_all_parents.data[count][i] = rand() % (num_units + 2);
		}
	}
}

// From serial: swap the old and new parents
void swapping_parent() {
	for (int i = 0; i < num_parents; i++) {
		for (int j = 0; j < length; j++) {
			int tmp;
			tmp = all_parents.data[i][j];
			all_parents.data[i][j] = new_all_parents.data[i][j];
			new_all_parents.data[i][j] = tmp;
		}
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

// From serial
//double UpdateFunction(int *vec, int size) {
//	int sum = 0;
//	double std, avg;
//	for (int i = 0; i < size; i++) {
//		sum += vec[i];
//	}
//	avg = sum / size;
//	for (int i = 0; i < size; i++) {
//		std += pow(abs(vec[i] - avg), 2.0);
//	}
//	std = pow(std, 0.5);
//	return std;
//}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	// clock_t start = (double) clock();
	// divide up the work among processors to create parent vectors
	division = new int[p];
	split_group(division);
	
	// initialize the parent vectors
	all_parents.Set_Size(num_parents, length);
	new_all_parents.Set_Size(division[id], length);

	// initialize tracking vectors
	distribution = new double[num_parents + 1];
	performance_list = new double[num_parents];
	performance_index = new int[num_parents];

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

	do
	{
		// SERIAL BOTTLENECK - EVERY PROCESSOR HAS TO UNDERSTAND THE PERFORMANCE OF THE CURRENT GENERATION
		//---------------------------
		// run the circuit simulator
		// Update function
		//---------------------------
		for (int i = 0; i < num_parents; i++)
		{
			performance_list[i] = Evaluate_Circuit(all_parents.data[i], 0.00001, 30, num_units, 10, 100, 100.0, 500.0);
		}

		// From serial: Evaluate the performance
		// find the max and min value
		int max_index, min_index;
		find_max_min(performance_list, max_index, min_index);
		// Choose the best and worst one
		best_performance = performance_list[max_index];
		best_index = max_index;
		worst_performance = performance_list[min_index];
		// From serial: if the performance is negative
		if (worst_performance < 0)
		{
			for (int i = 0; i < num_parents; i++)
			{
				performance_list[i] = (performance_list[i] - worst_performance + tol);
			}
		}

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
		if (id == 0)
		{
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
				select_parent(tgt_parent_index_1, tgt_parent_index_2);
				//pair[0] = all_parents.data[tgt_parent_index_1];
				//pair[1] = all_parents.data[tgt_parent_index_2];
				// Crossover to generate children
				int pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

				// Do cross over and Mutation
				for (int i = 0; i < length; i++)
				{
					if (i < pt)
					{
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
						// Decide whether to do mutation
						if (RollingDice())
						{
							// Do mutation
							mutation(count, i);
						}
					}
					else
					{
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_2][i];
						if (RollingDice())
						{
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
				if (Check_Validity(new_all_parents.data[count]))
				{
					count++;
				}
			}
			swapping_parent_parallel(division, id);

			// Now that the all_parents have been repopulated with new parents, send own parents to others and receive parents from others
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
		else
		{
			count = 0;
			while (count < division[id])
			{
				int tgt_parent_index_1;
				int tgt_parent_index_2;
				select_parent(tgt_parent_index_1, tgt_parent_index_2);
				//pair[0] = all_parents.data[tgt_parent_index_1];
				//pair[1] = all_parents.data[tgt_parent_index_2];

				// Crossover to generate children
				int pt = rand() % length;
				while (pt == 0 || pt == length)
				{
					pt = rand() % length;
				}

				// Do cross over and Mutation
				for (int i = 0; i < length; i++)
				{
					if (i < pt)
					{
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_1][i];
						// Decide whether to do mutation
						if (RollingDice())
						{
							// Do mutation
							mutation(count, i);
						}
					}
					else
					{
						new_all_parents.data[count][i] = all_parents.data[tgt_parent_index_2][i];
						if (RollingDice())
						{
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
				if (Check_Validity(new_all_parents.data[count]))
				{
					count++;
				}
			}
			swapping_parent_parallel(division, id);

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
	} while (best_performance < exp_performance && steps < max_steps);
	// clock_t end = (double)clock();
	if (id == 0)
	{
		if (write_to_file) { all_parents.write_solution_vector(); }
		//std::cout << "Total time (ms): " << (end - start) / CLOCKS_PER_SEC * 1000;
	}
	// Finished, clearing all memory
	all_parents.Clear_recv_Type();
	all_parents.Clear_send_Type();
	// delete[] pair;
	delete[] distribution;
	delete[] performance_list;
	delete[] performance_index;
	delete[] division;
	delete[] recv_request;
	delete[] send_request;
	MPI_Finalize();
}