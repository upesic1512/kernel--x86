/*
 * ListPCB.h
 *
 *  Created on: May 16, 2020
 *      Author: OS1
 */

#ifndef LISTPCB_H_
#define LISTPCB_H_
#include "System.h"

typedef unsigned int Time;
	struct Node{
		PCB* pcb;
		Node* prev;
		Node* next;

		Node(PCB* pcb){
			this->pcb=pcb;
			prev=0;
			next=0;
		}
	};


class ListPCB {
public:

	//FIELDS
	volatile Node* head;
	volatile Node* tail;
	//predstavlja broj blokiranih niti na semaforu
	volatile int numberOfNodes;

	//CONSTRUCTORS
	ListPCB();
	~ListPCB();

	//METHODS
	void add(PCB*);
	PCB* getFirst();
	void removeNode(Node* toRemove);

};

#endif /* LISTPCB_H_ */
