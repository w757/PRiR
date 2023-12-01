/*
 * LifeSequentialImplementation.h
 */

#ifndef LIFESEQUENTIALIMPLEMENTATION_H_
#define LIFESEQUENTIALIMPLEMENTATION_H_

#include "Life.h"

class LifeSequentialImplementation: public Life {
private:
	int procs;
    int rank;  
    int start_row;  
    int end_row;   


protected:
	void realStep();
public:
	LifeSequentialImplementation();
	int numberOfLivingCells();
	double averagePollution();
	void oneStep();


	void exchangeData(int p, int r);
	void splitArray(int p, int r);
};

#endif /* LIFESEQUENTIALIMPLEMENTATION_H_ */
