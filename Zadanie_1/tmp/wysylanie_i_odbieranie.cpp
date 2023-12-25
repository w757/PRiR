

/*
 * LifeParallelImplementation.cpp
 *
 */
#include <mpi.h>
#include "LifeParallelImplementation.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <mutex>

LifeParallelImplementation::LifeParallelImplementation()
{
}


void LifeParallelImplementation::exchangeData(int p, int r)
{
   
  int procs = p;
    int rank = r;



    MPI_Barrier(MPI_COMM_WORLD);


 if ( rank > 0 ) {
        MPI_Send( &cells[start_row][0], size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD );
        MPI_Send( &pollution[start_row][0], size, MPI_INT, rank - 1, 2, MPI_COMM_WORLD );
	}



    if ( rank < procs-1 ) {
        MPI_Send( &cells[end_row][0], size, MPI_INT, rank + 1, 3, MPI_COMM_WORLD );
        MPI_Send( &pollution[end_row][0], size, MPI_INT, rank + 1, 4, MPI_COMM_WORLD );
    }



    if ( rank < procs-1 ) {
        MPI_Recv( &cells[end_row + 1][0], size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv( &pollution[end_row + 1][0], size, MPI_INT, rank + 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }



    if ( rank > 0 ) {
        MPI_Recv( &cells[start_row - 1][0], size, MPI_INT, rank - 1, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv( &pollution[start_row - 1][0], size, MPI_INT, rank - 1, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }


    // if ( rank > 0 ) {
    //     MPI_Send( &cells[0][start_row], size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD );
    //     MPI_Send( &pollution[0][start_row], size, MPI_INT, rank - 1, 2, MPI_COMM_WORLD );
    //    // std::cout <<rank << "send " << &cells[0][start_row] << "  ";
	// }



    // if ( rank < procs-1) {
    //     MPI_Send( &cells[0][end_row], size, MPI_INT, rank + 1, 3, MPI_COMM_WORLD );
    //     MPI_Send( &pollution[0][end_row], size, MPI_INT, rank + 1, 4, MPI_COMM_WORLD );
    //   //  std::cout <<rank << "send " << &cells[0][end_row] << "  " ;
    // }



    // if ( rank < procs-1 ) {
    //     MPI_Recv( &cells[0][end_row + 1], size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    //     MPI_Recv( &pollution[0][end_row + 1], size, MPI_INT, rank + 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    //   //  std::cout << rank << "rec " << &cells[0][end_row + 1] << "  " ;
    // }



    // if ( rank > 0 ) {
    //     MPI_Recv( &cells[0][start_row - 1], size, MPI_INT, rank - 1, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    //     MPI_Recv( &pollution[0][start_row - 1], size, MPI_INT, rank - 1, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    //    // std::cout << rank << "rec " << &cells[0][start_row - 1] << "  " ;
    // }


     MPI_Barrier(MPI_COMM_WORLD);

}


void LifeParallelImplementation::realStep()
{
    int currentState, currentPollution;
    int procs, rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    int tmp_end_row = 0;
    int tmp_start_row =0;
    if (start_row!=0){
        tmp_start_row = start_row-1;
    }
    else{
        tmp_start_row = start_row;
    }
    if (end_row!=size_1){
        tmp_end_row = end_row+1;
        
    }
    else{
        tmp_end_row = end_row;
     
    }

	for (int row = 1; row < size_1; row++){
		for (int col = tmp_start_row; col < tmp_end_row; col++)
	// for (int row = start_row; row < end_row; row++){
	// 	for (int col = 1; col < size_1; col++)


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
     exchangeData(procs, rank);


std::cout << std::endl;

    std::cout <<  "s " <<start_row<<  std::endl;
     std::cout <<  "e " <<end_row<<  std::endl;
    exchangeData(procs,rank);
    for (int i = 1; i<size_1; i++){
    for (int j = start_row; j<end_row; j++){
        
        std::cout <<  cells[i][j];

    }
     std::cout <<  std::endl;
    }

std::cout <<  std::endl;

}

void LifeParallelImplementation::oneStep()
{

//   realStep();

//  // MPI_Barrier(MPI_COMM_WORLD); //
// swapTables();



}


int LifeParallelImplementation::numberOfLivingCells() {
 //   std::cout << "  sumTable( cells ) - " << sumTable( cells ) << std::endl;
	return sumTable( cells );
}

double LifeParallelImplementation::averagePollution() {
 //   std::cout << "   sumTable( pollution ) - " <<  sumTable( pollution ) << std::endl;
	return (double)sumTable( pollution ) / size_1_squared / rules->getMaxPollution();
}


void LifeParallelImplementation::beforeFirstStep() {


    int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    if (rank == 0){
cells[0][0]=7;
cells[9][9]=1;
}


//  // podzial tabeli na liczbe procesow 


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


    

 
// // Rozeslanie danych z procesu 0 do reszty procesow



    // "Splaszczanie" tablic do formatu jednowymiarowego dla MPI
    int total_size = size * size;
    int* flattened_cells = new int[total_size];
    int* flattened_pollution = new int[total_size];

if (rank == 0) {
    // Kopiowanie danych do buforów jednowymiarowych tylko w procesie głównym
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            flattened_cells[i * size + j] = cells[i][j];
            flattened_pollution[i * size + j] = pollution[i][j];
        }
    }
}

// // Rozgłaszanie danych do wszystkich procesów
MPI_Bcast(flattened_cells, total_size, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Bcast(flattened_pollution, total_size, MPI_INT, 0, MPI_COMM_WORLD);

 MPI_Barrier(MPI_COMM_WORLD);

// W każdym procesie, "odsplaszczanie" danych z powrotem do dwuwymiarowych tablic
for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
        cells[i][j] = flattened_cells[i * size + j];
        pollution[i][j] = flattened_pollution[i * size + j];
    }
}

// Zwolnienie zasobów
delete[] flattened_cells;
delete[] flattened_pollution;

    

// // Po tej funkcjiwszystkie procesy maja ta sama tablice od procesu 0


}


////////////////////////////////////////

void LifeParallelImplementation::afterLastStep() {



    int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);



    if (rank == 1){
        cells[4][4]=5;
    }

    if (rank == 2){
        cells[9][3]=3;
        cells[5][5]=3;
    }



    int tmp_start_row = 0;
    int tmp_end_row = 0;
         
    int data_size[procs];

    for (int i=0; i<procs; i++){
        int rows = size / procs;
        int extra_rows_tmp = size % procs;

        auto my_min = [](int a, int b) {
            return a < b ? a : b;
        };

        tmp_start_row = i * rows + my_min(i, extra_rows_tmp);
        tmp_end_row = tmp_start_row + rows + (i < extra_rows_tmp ? 1 : 0) - 1;

        data_size[i]=size * (tmp_end_row - tmp_start_row + 1);

    
    }



// std::cout << "rank" << rank << std::endl;
// std::cout << "start" << start_row << std::endl;
// std::cout << "end" << end_row << std::endl;
// std::cout << "data_size[i]" << data_size[rank] << std::endl;
// OK

   if (procs != 1){


    int* global_flattened_cells = nullptr;
    int* global_flattened_pollution = nullptr;


    int local_data_size = size * (end_row - start_row + 1);
    int* local_flattened_cells = new int[local_data_size];
    // int* local_flattened_pollution = new int[local_data_size];



    if (rank == 0) {
        int tmp_sum=0;
        for (int i=0; i<procs; i++){

            tmp_sum=tmp_sum+data_size[i];
    
    }

        global_flattened_cells = new int[tmp_sum];
        //global_flattened_pollution = new int[procs * local_data_size];
    }


    // "Splaszczanie" dwuwymiarowych tablic do jednowymiarowych
    for (int i = 0; i < (end_row - start_row+1); ++i) {
        for (int j = 0; j < size; ++j) {
            local_flattened_cells[i * size + j] = cells[start_row + i][j];
        //    local_flattened_pollution[i * size + j] = pollution[start_row + i][j];
        }
    }




    int *recvcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        recvcounts = (int*)malloc(procs * sizeof(int));
        displs = (int*)malloc(procs * sizeof(int));

        displs[0] = 0; // Początkowe przesunięcie dla pierwszego procesu
        for (int i = 0; i < procs; i++) {
            recvcounts[i] = data_size[i]; // Ustawienie rozmiaru danych dla procesu i

          //  Ustawienie przesunięcia dla kolejnego procesu
            if (i > 0) {
                displs[i] = displs[i - 1] + recvcounts[i - 1];
            }
        }
    
    
    if (rank == 0){
        // recvcounts[0]=40;
        // recvcounts[1]=30;
        // recvcounts[2]=30;


        // displs[0]=0;
        // displs[1]=40;
        // displs[2]=70;

    }
    }


    // Użycie MPI_Gatherv
    MPI_Gatherv(local_flattened_cells, local_data_size, MPI_INT,
                global_flattened_cells, recvcounts, displs, MPI_INT,
                0, MPI_COMM_WORLD);


    if (rank == 0) {
        // Proces główny posiada teraz wszystkie dane w global_flattened_cells
        free(recvcounts);
        free(displs);
    }



    // std::cout << "rank" << rank << std::endl;

    // if (rank == 0) {
    // }
    // if (rank == 1) {
    //     sleep(2);
    // }
    // if (rank == 2) {
    //     sleep(4);
    // }




    if (rank == 0) {
        std::cout << "rank" << rank << std::endl;
            std::cout << std::endl;
        for (int i = 0; i < (size)*(size); ++i) {

            
            //    local_flattened_pollution[i * size + j] = pollution[start_row + i][j];
            std::cout << global_flattened_cells[i];
            
        }
    }










   }



}
