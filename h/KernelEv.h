/*
 * KernelEv.h
 *
 *  Created on: Aug 10, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_
//#include "System.h"

class PCB;
typedef unsigned char IVTNo;

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	virtual ~KernelEv();

	//METHODES
	void signal();
	void wait();

	//FIELDS
	int value;
	int blocked;
	PCB* parent; //nit koja je kreirala dogadjaj i koja moze da se blokira cekajuci na njega
	IVTNo ivtNo;
};

#endif /* KERNELEV_H_ */
