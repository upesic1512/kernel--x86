/*
 * IVTEntry.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: OS1
 */

#include "event.h"
#include "KernelEv.h"
#include "IVTEntry.h"
#include "System.h"
#include <dos.h>


void locking();
void unlocking();

extern IVTEntry *ivtEntries[];


//prvo se pravi IVTEntry pa onda Event
//konstuktor uzme staru prekidnu rutinu i podmetne novu
IVTEntry::IVTEntry(IVTNo numEntry, pInterrupt newRoutine) {
locking();
myKernelEv=0;
#ifndef BCC_BLOCK_IGNORE
oldRout=getvect(numEntry);
setvect(numEntry, newRoutine);
#endif
//mozda trebam da pazim da se ne prepisuje preko elementa u ivtEntries
ivtEntries[numEntry]=this;
unlocking();

}

void IVTEntry::callOld(){
	this->oldRout();
}

void IVTEntry::signal(){
	if(myKernelEv==0)
		return;
	myKernelEv->signal();
}

IVTEntry::~IVTEntry() {
	locking();
	ivtEntries[myKernelEv->ivtNo]=0;
#ifndef BCC_BLOCK_IGNORE
	setvect(myKernelEv->ivtNo, oldRout);
#endif
	unlocking();
}

