/*
 * LifeParallelImplementation.h
 */

#ifndef LIFEPARALLELIMPLEMENTATION_H_
#define LIFEPARALLELIMPLEMENTATION_H_

#include "Life.h"

class LifeParallelImplementation: public Life {
private:

	int procs;
    int rank;  
    int start_row;  
    int end_row;   


protected:
	void realStep();
public:
	LifeParallelImplementation();
	int numberOfLivingCells();
	double averagePollution();
	void oneStep();


	void exchangeData(int p, int r);
	void splitArray(int p, int r);
};

#endif /* LifeParallelImplementation_H_ */
