/*
 * ListSem.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: OS1
 */
#include "System.h"
//#include "ListSem.h"

void locking();
void unlocking();

ListSem::ListSem() {
	head=0;
	tail=0;
	numberOfNodes=0;

}

ListSem::~ListSem() {
	locking();
	while(head){
		KernelSem* zaBrisanje=getFirst();
		delete zaBrisanje;
	}
	head=tail=0;
	unlocking();
}

//add at the end
void ListSem::add(KernelSem* sem){
	numberOfNodes++;
	locking();
	//empty list
	if(tail==0){
		tail= new NodeSem(sem);
		head=tail;
	}
	else{
		NodeSem * t=new NodeSem(sem);
		tail->next=t;
		t->prev=(NodeSem*)tail;
		tail=tail->next;
	}
	unlocking();
}

//NETESTIRANO
//izbacuje i vraca prvi element iz liste, nisam dealocirao taj element
KernelSem* ListSem::getFirst(){
	KernelSem* ret=0;
	if(head==0) return ret;
	locking();
	NodeSem* oldHead=(NodeSem*)head;
	unlocking();
	if(head==tail){
		locking();
		ret=head->semaphore;
		head=0;
		tail=0;
		numberOfNodes=0;
		//OBRISATI CVOR!!!
		delete oldHead;
		unlocking();
		return ret;
	}
	//DEALOCIRATI CVOR
	locking();
	ret=head->semaphore;
	head->next->prev=0;
	head=head->next;
	numberOfNodes--;
	delete oldHead;
	unlocking();
	return ret;
}
