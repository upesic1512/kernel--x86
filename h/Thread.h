/*
 * Thread.h
 *
 *  Created on: May 7, 2020
 *      Author: OS1
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <iostream.h>
#include <dos.h>

typedef unsigned long StackSize;
const StackSize defaultStackSize=4096;
typedef unsigned int Time; //time, x 55ms
const Time defaultTimeSlice=2; //default=2*55ms
typedef int ID;

/////////////////for signals
typedef void (*SignalHandler)();
typedef unsigned SignalId;
///////////////////////////////////

class PCB; //kernel's implementation of user thread


class Thread {
public:

	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread *getThreadById(ID id);


///////////////////	SIGNALS	///////////////////////////////////////

	//kaze obradi handlere
	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
//////////////////////////////////////////////////////////////////

protected:
	friend class PCB;
	Thread(StackSize stackSize=defaultStackSize, Time timeSlice=defaultTimeSlice);
	virtual void run(){}
public:
//private:
	PCB *myPCB;
	int started;
};

void dispatch();



#endif

/* THREAD_H_ */


