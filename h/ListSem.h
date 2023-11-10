/*
 * ListSem.h
 *
 *  Created on: Aug 1, 2020
 *      Author: OS1
 */

#ifndef LISTSEM_H_
#define LISTSEM_H_

struct NodeSem{
		KernelSem* semaphore;
		NodeSem* prev;
		NodeSem* next;

		NodeSem(KernelSem* sem){
			this->semaphore=sem;
			prev=0;
			next=0;
		}
	};

class ListSem {
public:

	//FIELDS
	volatile NodeSem* head;
	volatile NodeSem* tail;
	//predstavlja broj blokiranih niti na semaforu
	volatile int numberOfNodes;

	ListSem();
	virtual ~ListSem();

	void add(KernelSem* sem);
	KernelSem* getFirst();
};

#endif /* LISTSEM_H_ */
