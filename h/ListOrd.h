/*
 * ListOrd.h
 *
 *  Created on: Aug 27, 2020
 *      Author: OS1
 */

#ifndef LISTORD_H_
#define LISTORD_H_

struct NodeOrd{
		unsigned number;
		NodeOrd* prev;
		NodeOrd* next;

		NodeOrd(int num){
			this->number=num;
			prev=0;
			next=0;
		}
	};

class ListOrd {
public:

	volatile NodeOrd* head;
	volatile NodeOrd* tail;

	ListOrd();
	virtual ~ListOrd();

	void add(unsigned);
	unsigned getFirst();
	void removeNode(NodeOrd* toRemove);
};

#endif /* LISTORD_H_ */
