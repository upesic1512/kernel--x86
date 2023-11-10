#include "System.h"
#ifndef _kerSem_h_
#define _kerSem_h_

typedef unsigned int Time;

class KernelSem{

public:
	KernelSem(int init);

	int wait(Time maxTimeToWait);
	int signal(int n=0);

	int value() const {return val;}

	int val;
	ListPCB* blocked;
};



#endif
