/*
 * IVTEntry.h
 *
 *  Created on: Aug 10, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_
//#include "System.h"

class KernelEv;

typedef void interrupt (*pInterrupt)(...);


class IVTEntry {
public:
	//unutar konstruktora primiti broj prekida, ulaz IV tabele, procita adresu te prekidne rutine, sacuva staru prekidnu rutinu, i tu da podmetne novu prekidnu rutinu koja zove signal nad ovim kernelEventom

	IVTEntry(IVTNo ivtNo, pInterrupt newRoutine);
	virtual ~IVTEntry();

	//FILEDS
	pInterrupt oldRout;
	KernelEv* myKernelEv;


	//METHODS
	void callOld();
	void signal();
};

#endif /* IVTENTRY_H_ */
