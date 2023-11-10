/*
 * Thread.cpp
 *
 *  Created on: May 7, 2020
 *      Author: OS1
 */
#include "System.h"
extern PCB* running;
extern ListPCB* allPCB;
extern int signalBlockedGlobally[];

void locking();
void unlocking();

//constructors

Thread::Thread(StackSize stackSize, Time timeSlice){
	lock
	started=0;
	myPCB=new PCB(stackSize, timeSlice, this);
	unlock
}

Thread::~Thread(){
	waitToComplete();
	delete myPCB;
	myPCB=0;
}

void Thread::start(){
	lock
	/*lock
	cout<<"stareted Thread id: "<<myPCB->myID<<endl;
	unlock*/
	if(!started){
		started=1;
		Scheduler::put(myPCB);
	}
	/*lock
		cout<<"nit stavljena u Scheduler: "<<myPCB->myID<<endl;
	unlock*/
	unlock
}

ID Thread::getRunningId(){
	return running->myID;
}

//NETESTIRANO!

ID Thread::getId(){
	lock
	if(myPCB!=0){
		unlock
		return myPCB->myID;
	}
	//ako je nemam svoj PCB
	unlock
	return -1;
}

//moze da dohvati samo korisnicke niti
Thread* Thread::getThreadById(ID id){
	lock
	if(allPCB->head==0){
		unlock
		return 0;
	}
	volatile Node* cur=allPCB->head;
	for(; cur!=0; cur=cur->next){
		if(cur->pcb->myID==id){
			unlock
			return cur->pcb->myThread;
		}
	}
	cout<<"Ne postoji nit sa zadatim ID-jem"<<endl;
	unlock
	return 0;
}

//Da li treba nit da moze da pozove vise puta waitToClomplete, tj da ceka na vise niti zavrsi pre nego se odblokira?
void Thread::waitToComplete(){

	if(this->myPCB->state==PCB::FINISHED)
		return;
	locking();
	/*lock
	cout<<"BLOKIRA SE NIT ID: "<< getRunningId()<<endl<< "this ID: "<< this->myPCB->myID<<endl;
	unlock*/
	PCB* t=running;
	t->state=PCB::BLOCKED;
	this->myPCB->waitingPCBs->add(t);
	unlocking();
	dispatch();

}

//signals

void Thread::blockSignalGlobally(SignalId signal){
	signalBlockedGlobally[signal]=1;
}

void Thread::unblockSignalGlobally(SignalId signal){
	signalBlockedGlobally[signal]=0;
}

void Thread::blockSignal(SignalId signal){
	this->myPCB->signalBlocked[signal]=1;
}

void Thread::unblockSignal(SignalId signal){
	this->myPCB->signalBlocked[signal]=0;
}

void Thread::registerHandler(SignalId signal, SignalHandler handler){
	this->myPCB->registerHandler(signal,handler);
}

void Thread::unregisterAllHandlers(SignalId id){

	this->myPCB->unregisterAllHandlers(id);
}

void Thread::signal(SignalId signal){
	this->myPCB->signal(signal);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	this->myPCB->swap(id,hand1,hand2);
}


