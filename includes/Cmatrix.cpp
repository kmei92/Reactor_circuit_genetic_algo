#include "Cmatrix.h"
#include <iostream>
#include <sstream>
#include <fstream>

Cmatrix::Cmatrix(int nrows, int ncolumns)
{
	rows = nrows;
	columns = ncolumns;

	data = new int *[rows];
	for (int i = 0; i < rows; i++)
	{
		data[i] = new int[columns];
	}
	row_send_type = nullptr;
	row_recv_type = nullptr;
};
void Cmatrix::Set_Size(int nrows, int ncolumns)
{
	rows = nrows;
	columns = ncolumns;
	data = new int *[rows];
	for (int i = 0; i < rows; i++)
	{
		data[i] = new int[columns];
	}
};
Cmatrix::~Cmatrix()
{
	for (int i = 0; i < rows; i++)
		delete[] data[i];
	delete[] data;

	int flag;
	MPI_Finalized(&flag);
	if (!flag && row_send_type != nullptr)
		MPI_Type_free(row_send_type);
	delete row_send_type;
	if (!flag && row_recv_type != nullptr)
		MPI_Type_free(row_recv_type);
	delete row_recv_type;
};
void Cmatrix::setup_MPI_row_send(int min_row, int max_row)
{
	if (row_send_type == nullptr)
	{
		vector<int> block_length;
		vector<MPI_Aint> addresses;
		vector<MPI_Datatype> typelist;

		block_length.reserve(max_row - min_row + 1);
		addresses.reserve(max_row - min_row + 1);
		typelist.reserve(max_row - min_row + 1);

		int cnt = 0;
		for (int i = min_row; i <= max_row; i++) // max row is included in the datatype
		{
			block_length.push_back(columns);
			MPI_Aint temp;
			MPI_Get_address(&data[i][0], &temp);	// this is the address of the rows
			addresses.push_back(temp);
			typelist.push_back(MPI_INT);
			cnt++;
		}

		row_send_type = new MPI_Datatype;
		MPI_Type_create_struct(cnt, block_length.data(), addresses.data(), typelist.data(), row_send_type);
		MPI_Type_commit(row_send_type);
	}
};
void Cmatrix::setup_MPI_row_recv(int min_row, int max_row)
{
	if (row_recv_type == nullptr)
	{
		vector<int> block_length;
		vector<MPI_Aint> addresses;
		vector<MPI_Datatype> typelist;

		block_length.reserve(max_row - min_row + 1);
		addresses.reserve(max_row - min_row + 1);
		typelist.reserve(max_row - min_row + 1);

		int cnt = 0;
		for (int i = min_row; i <= max_row; i++) // max row is included in the send
		{
			block_length.push_back(columns);
			MPI_Aint temp;
			MPI_Get_address(&data[i][0], &temp);	// this is the address of the rows
			addresses.push_back(temp);
			typelist.push_back(MPI_INT);
			cnt++;
		}

		row_recv_type = new MPI_Datatype;
		MPI_Type_create_struct(cnt, block_length.data(), addresses.data(), typelist.data(), row_recv_type);
		MPI_Type_commit(row_recv_type);
	}
};
void Cmatrix::send_matrix_rows(int destination, int tag_num, MPI_Request *request)
{
	MPI_Isend(MPI_BOTTOM, 1, *row_send_type, destination, tag_num, MPI_COMM_WORLD, request);
};

void Cmatrix::receive_matrix_rows(int source, int tag_num, int min_row, int max_row, MPI_Request *request)
{
	setup_MPI_row_recv(min_row, max_row);
	MPI_Irecv(MPI_BOTTOM, 1, *row_recv_type, source, tag_num, MPI_COMM_WORLD, request);
};

void Cmatrix::Clear_send_Type() // member function to clear the created MPI data types
{
	if (row_send_type != nullptr) { MPI_Type_free(row_send_type); }
	delete row_send_type;
	row_send_type = nullptr;
};
void Cmatrix::Clear_recv_Type() // member function to clear the created MPI data types
{
	if (row_recv_type != nullptr) { MPI_Type_free(row_recv_type); }
	delete row_recv_type;
	row_recv_type = nullptr;
};

void Cmatrix::write_solution_vector()
{
	// writes the solution vector to a file - NOTE: only processor 0 should call this function
	
	ofstream final_vec;
	final_vec.open("../../solution_vector.txt", ios_base::out);
	for (int i = 0; i < columns; i++)
	{
			final_vec << data[0][i] << " ";
	}
	final_vec.close();
}