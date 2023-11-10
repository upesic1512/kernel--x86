/*
 * ListOrd.cpp
 *
 *  Created on: Aug 27, 2020
 *      Author: OS1
 */

#include "ListOrd.h"
#include <iostream.h>

void locking();
void unlocking();

ListOrd::ListOrd() {
	head=0;
	tail=0;
}

ListOrd::~ListOrd() {
	locking();
	while(head){
		getFirst();
	}
	head=tail=0;
	unlocking();
}

void ListOrd::add(unsigned number){
	locking();
	//cout<<"Dodajem broj: "<<number<<endl;
	//empty list
	if(tail==0){
		tail= new NodeOrd(number);
		head=tail;
	}
	else{
		NodeOrd* t=new NodeOrd(number);
		tail->next=t;
		t->prev=(NodeOrd*)tail;
		tail=tail->next;
	}
	unlocking();
}

//vraca -1 ako nema vise signala
unsigned ListOrd::getFirst(){
	int ret=-1;
	if(head==0) return ret;
	locking();
	NodeOrd* oldHead=(NodeOrd*)head;
	unlocking();
	if(head==tail){
		locking();
		ret=head->number;
		delete oldHead;
		head=0;
		tail=0;
		//OBRISATI CVOR!!!
		unlocking();
		return ret;
	}
	//DEALOCIRATI CVOR
	locking();
	ret=head->number;
	head->next->prev=0;
	head=head->next;
	delete oldHead;
	unlocking();
	return ret;
}

void ListOrd::removeNode(NodeOrd* toRemove){

	locking();
		 NodeOrd* prevNode = toRemove->prev;
		 NodeOrd* nextNode = toRemove->next;
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

		delete toRemove;

	unlocking();
}

