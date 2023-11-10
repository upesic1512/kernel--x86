/*
 * ListHan.cpp
 *
 *  Created on: Aug 26, 2020
 *      Author: OS1
 */

#include "ListHan.h"

void locking();
void unlocking();

ListHan::ListHan() {
	head=0;
	tail=0;
}


ListHan::~ListHan() {
	locking();
	while(head){
		getFirst();
	}
	head=tail=0;
	unlocking();
}


void ListHan::add(SignalHandler handler){
	locking();
	//empty list
	if(tail==0){
		tail= new NodeHandler(handler);
		head=tail;
	}
	else{
		NodeHandler * t=new NodeHandler(handler);
		tail->next=t;
		t->prev=(NodeHandler*)tail;
		tail=tail->next;
	}
	unlocking();
}

SignalHandler ListHan::getFirst(){
	SignalHandler ret=0;
	if(head==0) return ret;
	locking();
	NodeHandler* oldHead=(NodeHandler*)head;
	unlocking();
	if(head==tail){
		locking();
		ret=head->handler;
		delete oldHead;
		head=0;
		tail=0;
		//OBRISATI CVOR!!!
		unlocking();
		return ret;
	}
	//DEALOCIRATI CVOR
	locking();
	ret=head->handler;
	head->next->prev=0;
	head=head->next;
	delete oldHead;
	unlocking();
	return ret;
}
