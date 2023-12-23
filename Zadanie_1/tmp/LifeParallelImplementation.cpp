

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
     //exchangeData(procs, rank);


// std::cout << std::endl;

//     std::cout <<  "s " <<start_row<<  std::endl;
//      std::cout <<  "e " <<end_row<<  std::endl;
//     exchangeData(procs,rank);
//     for (int i = 1; i<size_1; i++){
//     for (int j = start_row; j<end_row; j++){
        
//         std::cout <<  cells[i][j];

//     }
//      std::cout <<  std::endl;
//     }

// std::cout <<  std::endl;

}

void LifeParallelImplementation::oneStep()
{


  realStep();

 // MPI_Barrier(MPI_COMM_WORLD); //
swapTables();




 
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

    
// for (int x=0; x<3; x++){
//      if (rank==x){
//      sleep(2);



    // for ( int row = 1; row < size_1; row++ ){
	// 	for( int col = 1; col < size_1; col++ )
	// 		std::cout << cells[ row ][ col ];

    //     std::cout  << std::endl;
    // }



//      }
// }
 // podzial tabeli na liczbe procesow 

    int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

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
 
 
// Rozeslanie danych z procesu 0 do reszty procesow


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Barrier(MPI_COMM_WORLD);


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

// Rozgłaszanie danych do wszystkich procesów
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

    

// Po tej funkcjiwszystkie procesy maja ta sama tablice od procesu 0

     MPI_Barrier(MPI_COMM_WORLD);



}


////////////////////////////////////////

void LifeParallelImplementation::afterLastStep() {

  int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    // std::cout << "  ??????rank - " << rank << std::endl;
	//  std::cout << "  ?????sumTable( cells ) - " << sumTable( cells ) << std::endl;
    //  std::cout << "  ?????sumTable( pollution ) - " << sumTable( pollution ) << std::endl;
    int sum = 0;


	for ( int row = 1; row < size_1; row++ )
		for( int  col = start_row; col <= end_row; col++)
			sum += cells[ row ][ col ];

     std::cout << "  rank - " << rank << "cells=" << sum<< std::endl;

    // int rank, procs;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &procs);



    //     if (procs != 1){
    // // Rozmiar danych wysyłanych przez każdy proces
    // int local_data_size = size * (end_row - start_row);
    // int* local_flattened_cells = new int[local_data_size];
    // int* local_flattened_pollution = new int[local_data_size];

    // // "Splaszczanie" dwuwymiarowych tablic do jednowymiarowych
    // for (int i = 0; i < (end_row - start_row); ++i) {
    //     for (int j = 0; j < size; ++j) {
    //         local_flattened_cells[i * size + j] = cells[start_row + i][j];
    //         local_flattened_pollution[i * size + j] = pollution[start_row + i][j];
    //     }
    // }

    // // Proces główny przygotowuje bufory na otrzymanie danych ze wszystkich procesów
    // int* global_flattened_cells = nullptr;
    // int* global_flattened_pollution = nullptr;
    // if (rank == 0) {
    //     global_flattened_cells = new int[procs * local_data_size];
    //     global_flattened_pollution = new int[procs * local_data_size];
    // }
    //  MPI_Barrier(MPI_COMM_WORLD);
    // // Zbieranie danych ze wszystkich procesów w procesie o ranku 0
    // MPI_Gather(local_flattened_cells, local_data_size, MPI_INT,
    //            global_flattened_cells, local_data_size, MPI_INT,
    //            0, MPI_COMM_WORLD);
    // MPI_Gather(local_flattened_pollution, local_data_size, MPI_INT,
    //            global_flattened_pollution, local_data_size, MPI_INT,
    //            0, MPI_COMM_WORLD);

    // // Proces główny odsplaszcza dane do dwuwymiarowych tablic cells i pollution
    // if (rank == 0) {
    //     for (int i = 0; i < procs; ++i) {
    //         for (int j = 0; j < local_data_size; ++j) {
    //             int row = (i * local_data_size / size) + (j / size);
    //             int col = j % size;
    //             cells[row][col] = global_flattened_cells[i * local_data_size + j];
    //             pollution[row][col] = global_flattened_pollution[i * local_data_size + j];
    //         }
    //     }
    //     // Tutaj możesz przetwarzać dane w cells i pollution...

    //     delete[] global_flattened_cells;
    //     delete[] global_flattened_pollution;
    // }

    // // Zwolnienie lokalnej pamięci
    // delete[] local_flattened_cells;
    // delete[] local_flattened_pollution;

    //     }

    //        	  std::cout << "  !!!!rank - " << rank << std::endl;
	//     std::cout << "  !!!!!sumTable( cells ) - " << sumTable( cells ) << std::endl;
}












// void LifeParallelImplementation::afterLastStep() {
  

//     MPI_Barrier(MPI_COMM_WORLD);
        
//     int rank, procs;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &procs);

//     // Lokalne dane w każdym procesie
//     int local_data[size];

//     // Wypełnianie lokalnych danych przykładowymi wartościami
//     for (int i = start_row; i < end_row; ++i) {
//         local_data[i] = rank * 100 + i; // Przykładowe dane
//     }

//     // Proces główny przygotowuje bufor na otrzymanie danych ze wszystkich procesów
//     int* global_data = nullptr;
//     if (rank == 0) {
//         global_data = new int[procs * size];
//     }

//     // Zbieranie danych ze wszystkich procesów w procesie o ranku 0
//     MPI_Gather(local_data, size, MPI_INT,
//                global_data, size, MPI_INT,
//                0, MPI_COMM_WORLD);

//     // W tym miejscu, proces o ranku 0 ma wszystkie zebrane dane w global_data

//     // Czyszczenie pamięci w procesie głównym
//     if (rank == 0) {
//         // Tutaj możesz przetwarzać global_data...

//         delete[] global_data;
//     }



//     //-----------------------


//     MPI_Barrier(MPI_COMM_WORLD);
// }





// void LifeParallelImplementation::afterLastStep() {


//     MPI_Barrier(MPI_COMM_WORLD);

//     int rank, procs;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &procs);

//     // Rozmiar danych wysyłanych przez każdy proces
//     int local_data_size = size * (end_row - start_row);
//     int* local_flattened_data = new int[local_data_size];

//     // "Splaszczanie" dwuwymiarowej tablicy do jednowymiarowej

//     // for (int row = 1; row < size_1; row++){
// 	// 	for (int col = start_row; col < end_row+1; col++){
//     //         local_flattened_data[row + col*size] = cells[row][col];
//     //     }
//     // }

//     for (int i = 0; i < (end_row - start_row); ++i) {
//         for (int j = 0; j < size; ++j) {
//             local_flattened_data[i * size + j] = cells[start_row + i][j];
//         }
//     }

//     // Proces główny przygotowuje bufor na otrzymanie danych ze wszystkich procesów
//     int* global_flattened_data = nullptr;
//     if (rank == 0) {
//         global_flattened_data = new int[procs * local_data_size];
//     }

//     // Zbieranie danych ze wszystkich procesów w procesie o ranku 0
//     MPI_Gather(local_flattened_data, local_data_size, MPI_INT,
//                global_flattened_data, local_data_size, MPI_INT,
//                0, MPI_COMM_WORLD);

//   MPI_Barrier(MPI_COMM_WORLD);
// if (rank == 0) {
//     for (int i = 0; i < procs; ++i) {
//         for (int j = 0; j < local_data_size; ++j) {
//             int row = ...;  // Oblicz odpowiedni wiersz
//             int col = ...;  // Oblicz odpowiednią kolumnę
//             cells[row][col] = global_flattened_data[i * local_data_size + j];
//         }
//     }
// }



//    // Rozmiar danych wysyłanych przez każdy proces
//     int local_data_size_2 = size * (end_row - start_row);
//     int* local_flattened_data_2 = new int[local_data_size_2];

//     // "Splaszczanie" dwuwymiarowej tablicy do jednowymiarowej
//     for (int i = 0; i < (end_row - start_row); ++i) {
//         for (int j = 0; j < size; ++j) {
//             local_flattened_data_2[i * size + j] = pollution[start_row + i][j];
//         }
//     }

//     // Proces główny przygotowuje bufor na otrzymanie danych ze wszystkich procesów
//     int* global_flattened_data_2 = nullptr;
//     if (rank == 0) {
//         global_flattened_data_2 = new int[procs * local_data_size_2];
//     }

//     // Zbieranie danych ze wszystkich procesów w procesie o ranku 0
//     MPI_Gather(local_flattened_data_2, local_data_size_2, MPI_INT,
//                global_flattened_data_2, local_data_size_2, MPI_INT,
//                0, MPI_COMM_WORLD);


//     MPI_Barrier(MPI_COMM_WORLD);
//     // Zwolnienie lokalnej pamięci
//     delete[] local_flattened_data;
//
//}

