/*
 * LifeParallelImplementation.cpp
 */

#ifndef LIFEPARALLELIMPLEMENTATION_H_
#define LIFEPARALLELIMPLEMENTATION_H_

#include "Life.h"

class LifeParallelImplementation: public Life {
protected:
	void realStep();
public:
	LifeParallelImplementation();
	int numberOfLivingCells();
	double averagePollution();
	void oneStep();
};

#endif /* LIFEPARALLELIMPLEMENTATION_H_ */
