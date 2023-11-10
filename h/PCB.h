// PCB.h


#ifndef PCB_H_
#define PCB_H_
#include "SCHEDULE.h"
#include<dos.h>
#include "Thread.h"
class ListPCB;
#include "ListHan.h"
#include "ListOrd.h";

const StackSize maxStackSize=65535;

class PCB{
public:
	enum State{
		FINISHED,
		READY,
		BLOCKED,
		IDLE
	};

	unsigned sp;
	unsigned ss;
	unsigned bp;
	PCB::State state;
	unsigned *stack;
	Time timeSlice;
	StackSize stackSize;
	Thread* myThread;
	static ID uniqueID;
	ID myID;
	ListPCB* waitingPCBs;
	Time timeToWait;

	//signals
	PCB* parent;
	ListOrd* signalQueue;
	int signalled[16];
	ListHan* signalHandlers[16];
	int signalBlocked[16];

	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void signal(SignalId signal);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);


	//constructors

	PCB();
	PCB(StackSize stackSize, Time timeSlice, void (*body)());
	PCB(StackSize stackSize, Time timeSlice, Thread* myThread);

	//destructor
	~PCB();
	//methods

	static void wrapper();

};


#endif
