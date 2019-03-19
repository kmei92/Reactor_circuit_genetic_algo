#include <iostream>
#include <stdio.h>
#include <cmath>
#include <mpi.h>
#include <time.h>


using namespace std;
int id, p, tag_num = 1;

int **all_parents;
int num_parents = 100;
int num_units = 10;

void split_group(int *divisions) {

	int remaining = num_parents;
	for (int i = 0; i < p; i++)
	{
		int allocated = remaining / (p - i);
		divisions[i] = allocated;
		remaining -= allocated;
	}
}
//int answer_vector[2*num_units+1] = {0, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 9,
//				10, 11, 10, 11, 10, 11, 10, 11}  ;
//
//double Evaluate_Circuit(int *circuit_vector, double tolerance, int max_iterations) {
//
//  int vector_size = 2*num_units+1;
//
//  double Performance = 0.0; 
//  for (int i=0;i<vector_size;i++) {
//    //answer_vector is a predetermined answer vector (same size as circuit_vector)
//    Performance += (20-std::abs(circuit_vector[i]-answer_vector[i]))*100.0; 
//  }
//  return Performance; 
//}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int *division = new int[p];
	split_group(division);
	
	// divide up the work among processors to create parent vectors
	all_parents = new int *[num_parents];
	// initialize the parent vectors
	for (int i = 0; i < num_parents; i++)
	{
		all_parents[i] = new int[2 * num_units + 1];
	}

	int cnt = 0;
	for (int i = 0; i < division[id]; i++)
	{
		all_parents[i][0] = 0;
		for (int j = 1; j < 2 * num_units + 1; j += 2)
		{
			// generate random vector
			int index = j / 2;
			int top = rand() % (num_units + 2);
			int bot = rand() % (num_units + 2);
			while (top == index && bot == index && top == bot)
			{
				top = rand() % (num_units + 2);
				bot = rand() % (num_units + 2);
			}
			all_parents[i][j] = top;
			all_parents[i][j + 1] = bot;
		}
		cnt++;
	}

	// At this point, every processor has created its respective parent vectors
	// Begin receiving parent vectors from every other processor
	MPI_Request *recv_request = new MPI_Request[num_parents - division[id]];
	MPI_Request *send_request = new MPI_Request[division[id]];

	for (int i = 0; i < p; i++)
	{
		if (i != id)
		{
			for (int k = 0; k < division[id]; k++) // send from own processor
			{
				MPI_Isend(all_parents[k], (2 * num_units + 1), MPI_INT, i, tag_num, MPI_COMM_WORLD, &send_request[k]);
			}
		}
	}

	int recv_cnt = 0;
	for (int i = 0; i < p; i++)
	{

		if (i != id)
		{
			for (int j = 0; j < division[i]; j++) // open receive from other processors
			{
				MPI_Irecv(all_parents[cnt], (2 * num_units + 1), MPI_INT, i, tag_num, MPI_COMM_WORLD, &recv_request[recv_cnt]);
				cnt++;
				recv_cnt++;
			}
		}
	}
	MPI_Waitall(num_parents - division[id], recv_request, MPI_STATUSES_IGNORE);





	for (int i = 0; i < num_parents; i++)
	{
		delete[] all_parents[i];
	}
	delete[] all_parents;
	delete[] division;
	delete[] recv_request;
	delete[] send_request;
	MPI_Finalize();
}