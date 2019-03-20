#pragma once
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

class Cmatrix
{
public:
	int ** data;
	int rows, columns;

	Cmatrix()
	{
		data = nullptr;
		rows = 0;
		columns = 0;
	}
	void Set_Size(int nrows, int ncolumns);
	Cmatrix(int nrows, int ncolumns);
	~Cmatrix();
	
	void setup_MPI_row_send(int min_row, int max_row); // setup MPI data types to send rows
	void setup_MPI_row_recv(int min_row, int max_row); // setup MPI data types to receive rows
	void send_matrix_rows(int destination, int tag_num, MPI_Request *request); // member function to send an entire set of rows
	void receive_matrix_rows(int source, int tag_num, int min_row, int max_row, MPI_Request *request);	// member function to receive an entire set of rows
	void Clear_send_Type(); // member function to clear the created MPI data types
	void Clear_recv_Type(); // member function to clear the created MPI data types
	void write_solution_vector();
	
private:
	MPI_Datatype *row_send_type, *row_recv_type;// four MPI data types to send and receive the left and right columns
};


