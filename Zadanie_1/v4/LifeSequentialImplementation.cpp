/*
 * LifeSequentialImplementation.cpp
 *
 */
#include <mpi.h>
#include "LifeSequentialImplementation.h"


LifeSequentialImplementation::LifeSequentialImplementation()
{
}

void LifeSequentialImplementation::splitArray(int p, int r)
    {
    
    int procs = p;
    int rank = r;
	
    int rows_per_process = (size) / procs;   
    start_row = rank * rows_per_process;
	end_row = start_row + rows_per_process - 1;
	

}

void LifeSequentialImplementation::exchangeData(int p, int r)
{
   
    int procs = p;
    int rank = r;

    MPI_Barrier(MPI_COMM_WORLD);

    if ( rank > 0 ) {
        MPI_Send( &cells[start_row][0], size_1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD );
        MPI_Send( &pollution[start_row][0], size_1, MPI_INT, rank - 1, 2, MPI_COMM_WORLD );
	}



    if ( rank < procs-1 ) {
        MPI_Send( &cells[end_row][0], size_1, MPI_INT, rank + 1, 3, MPI_COMM_WORLD );
        MPI_Send( &pollution[end_row][0], size_1, MPI_INT, rank + 1, 4, MPI_COMM_WORLD );
    }



    if ( rank < procs-1 ) {
        MPI_Recv( &cells[end_row + 1][0], size_1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv( &pollution[end_row + 1][0], size_1, MPI_INT, rank + 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }



    if ( rank > 0 ) {
        MPI_Recv( &cells[start_row - 1][0], size_1, MPI_INT, rank - 1, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv( &pollution[start_row - 1][0], size_1, MPI_INT, rank - 1, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}


void LifeSequentialImplementation::realStep()
{
    int currentState, currentPollution;
    int procs, rank;


	for (int row = 1; row < size_1; row++){
		for (int col = start_row; col < end_row; col++)
        {
            currentState = cells[row][col];
            currentPollution = pollution[row][col];
            cellsNext[row][col] = rules->cellNextState(currentState, liveNeighbours(row, col),
                                                       currentPollution);
            pollutionNext[row][col] =
                rules->nextPollution(currentState, currentPollution, pollution[row + 1][col] + pollution[row - 1][col] + pollution[row][col - 1] + pollution[row][col + 1],
                                     pollution[row - 1][col - 1] + pollution[row - 1][col + 1] + pollution[row + 1][col - 1] + pollution[row + 1][col + 1]);
        }
	}


    exchangeData(procs,rank);
}

void LifeSequentialImplementation::oneStep()
{

    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    splitArray(procs,rank);
    realStep();
	swapTables();
}

int LifeSequentialImplementation::numberOfLivingCells() {
	return sumTable( cells );
}

double LifeSequentialImplementation::averagePollution() {
	return (double)sumTable( pollution ) / size_1_squared / rules->getMaxPollution();
}

