/*
 * LifeParallelImplementation.h
 */

#ifndef LIFEPARALLELIMPLEMENTATION_H_
#define LIFEPARALLELIMPLEMENTATION_H_

#include "Life.h"
#include "Alloc.h"
// ... reszta kodu ...


class LifeParallelImplementation: public Life {
private:
	//int p;
	int r;
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
	virtual void afterLastStep() override;
	virtual void beforeFirstStep() override;
	void check();

	void exchangeData(int p, int r);

};

#endif /* LIFEPARALLELIMPLEMENTATION_H_ */
