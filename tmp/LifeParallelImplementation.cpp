/*
 * LifeParallelImplementation.cpp
 *
 */
#include <mpi.h>
#include "LifeParallelImplementation.h"
#include <stdlib.h>
#include <iostream>

LifeParallelImplementation::LifeParallelImplementation()
{
}


void LifeParallelImplementation::exchangeData(int p, int r)
{
   
    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //
    int end_tmp_c [size_1];
    int end_tmp_p [size_1];
    int start_tmp_c [size_1];
    int start_tmp_p [size_1];

    for (int i = 1; i<size_1; i++){
        start_tmp_c [i] = cells[start_row][i];

        

        end_tmp_c [i] = cells[end_row][i];
        start_tmp_p [i] = pollution[start_row][i];
        end_tmp_p [i] = pollution[start_row][i];

    }


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


//TO DO 

cells[start_row - 1][i] czy cells[i1][start_row]


     MPI_Barrier(MPI_COMM_WORLD);


        for (int i = 1; i<size_1; i++){
        //if (start_tmp_c [i] != cells[start_row - 1][i]) {std::cout << "ERR" << std::endl;}
        {std::cout << cells[start_row - 1][i] << std::endl;}
        }
        // end_tmp_c [i] != cells[end_row +1][i];
        // start_tmp_p [i] != pollution[start_row -1 ][i];
        // end_tmp_p [i] != pollution[start_row + 1][i];

    
}


void LifeParallelImplementation::realStep()
{
    int currentState, currentPollution;
    int procs, rank;
    // std::cout << "  start_row: " << start_row << std::endl;
    // std::cout << "  end_row - " << end_row + 1<< std::endl;

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

void LifeParallelImplementation::oneStep()
{

    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    realStep();

    swapTables();


	
    // std::cout << "P" << rank << " cells_sum- " << sum_c  << std::endl;
    // std::cout << "P" << rank << " poll_sum- " << sum_p  << std::endl;

}

int LifeParallelImplementation::numberOfLivingCells() {
    //std::cout << "  sumTable( cells ) - " << sumTable( cells ) << std::endl;
	return sumTable( cells );
}

double LifeParallelImplementation::averagePollution() {
    //std::cout << "   sumTable( pollution ) - " <<  sumTable( pollution ) << std::endl;
	return (double)sumTable( pollution ) / size_1_squared / rules->getMaxPollution();
}


void LifeParallelImplementation::beforeFirstStep() {

    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
    if (procs != 1){
           
    int rows_per_process = size / procs;
    int extra_rows = size % procs;

    auto my_min = [](int a, int b) {
        return a < b ? a : b;
    };

    start_row = rank * rows_per_process + my_min(rank, extra_rows);
    end_row = start_row + rows_per_process + (rank < extra_rows ? 1 : 0) - 1;
    }

    else {
    start_row = 1;
    end_row = size_1;
    }
    std::cout << "  Rank: " << rank << std::endl;
    std::cout << "  start_row: " << start_row << std::endl;
    std::cout << "  end_row - " << end_row << std::endl;

     MPI_Barrier(MPI_COMM_WORLD);
     check();
}


void LifeParallelImplementation::check() {
size_t allocatedMemory = sizeof(int*) * size + sizeof(int) * size * size;

}


void LifeParallelImplementation::afterLastStep() {

}





