/*
 * ListPCB.cpp
 *
 *  Created on: May 16, 2020
 *      Author: OS1
 */

#include "ListPCB.h"
void locking();
void unlocking();

//CONSTRUCTORS
ListPCB::ListPCB() {
	head=0;
	tail=0;
	numberOfNodes=0;
}

ListPCB::~ListPCB() {
	locking();
	while(head){
		getFirst();
	}
	head=tail=0;
	unlocking();
}


//METHODS

//add at the end
void ListPCB::add(PCB* pcb){
	numberOfNodes++;
	locking();
	//empty list
	if(tail==0){
		tail= new Node(pcb);
		head=tail;
	}
	else{
		Node * t=new Node(pcb);
		tail->next=t;
		t->prev=(Node*)tail;
		tail=tail->next;
	}
	unlocking();
}

//izbacuje i vraca prvi element iz liste, nisam dealocirao taj element
PCB* ListPCB::getFirst(){
	PCB* ret=0;
	if(head==0) return ret;
	locking();
	Node* oldHead=(Node*)head;
	unlocking();
	if(head==tail){
		locking();
		ret=head->pcb;
		delete oldHead;
		head=0;
		tail=0;
		numberOfNodes=0;
		//OBRISATI CVOR!!!
		unlocking();
		return ret;
	}
	//DEALOCIRATI CVOR
	locking();
	ret=head->pcb;
	head->next->prev=0;
	head=head->next;
	numberOfNodes--;
	delete oldHead;
	unlocking();
	return ret;
}

void ListPCB::removeNode(Node* toRemove){

	locking();
	/*Node* curNode=(Node*)head;

	//dok je trenutni cvor razlicit od onog za ukloniti idi dalje
	for(; curNode!=toRemove && curNode!=0; curNode=curNode->next);*/
//ako sam nasao cvor za izbaciti, izbacujem ga iz liste
		 Node* prevNode = toRemove->prev;
		 Node* nextNode = toRemove->next;
		// Head node does not have a previous node
		if(head==tail){
			head=tail=0;
		}
		else{
			if (prevNode==0){
				head = 0;
				head = nextNode;
				head->prev = 0;
			}
			else if(nextNode==0){
				// Tail does not have a next node
				tail = 0;
				tail = prevNode;
				tail->next = 0;
			}
			else{
				// Is somewhere in the middle of the linked list
				// Set current node to null
				toRemove=0;
				// connect the previous and next nodes together
				prevNode->next = nextNode;
				nextNode->prev = prevNode;
			}
		}
		numberOfNodes--;
		delete toRemove;

	unlocking();
}




