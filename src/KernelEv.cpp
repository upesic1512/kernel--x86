/*
 * KernelEv.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: OS1
 */

#include "KernelEv.h"
#include "System.h"
#include "event.h"
#include "IVTEntry.h"

extern PCB* running;
extern IVTEntry *ivtEntries[];
void locking();
void unlocking();


KernelEv::KernelEv(IVTNo ivtNo) {
	locking();
		this->ivtNo=ivtNo;
		value=0;
		blocked=0;
		parent=running;
		ivtEntries[ivtNo]->myKernelEv=this;
	unlocking();
}

KernelEv::~KernelEv() {
	locking();
	value=0;
	parent=0;
	unlocking();
}

//poziva se samo iz prekidne rutine
void KernelEv::signal(){
	/*locking();
	cout<<"PRIMLJEN SIGNAL"<<endl;
	unlocking();*/
	locking();
	if(!blocked){
		value=1;
	}
	else{
		if(parent!=0){
		blocked=0;
		parent->state=PCB::READY;
		Scheduler::put(parent);
		}
	}
	unlocking();
}

//na eventu moze da se blokira samo nit koja ga je kreirala
void KernelEv::wait(){
	locking();

	if(running==parent){
		if(value==0){
			blocked=1;
			parent->state=PCB::BLOCKED;
	unlocking();
			dispatch();
		}
		else{
			value=0;

	unlocking();
		}
	}
}
