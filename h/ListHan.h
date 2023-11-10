/*
 * ListHan.h
 *
 *  Created on: Aug 26, 2020
 *      Author: OS1
 */

#ifndef LISTHAN_H_
#define LISTHAN_H_

typedef void (*SignalHandler)();
typedef unsigned SignalId;

struct NodeHandler{
		SignalHandler handler;
		NodeHandler* prev;
		NodeHandler* next;

		NodeHandler(SignalHandler handler){
			this->handler=handler;
			prev=0;
			next=0;
		}
	};

class ListHan {
public:

	volatile NodeHandler* head;
	volatile NodeHandler* tail;

	ListHan();
	virtual ~ListHan();

	void add(SignalHandler);
	SignalHandler getFirst();
};

#endif /* LISTHAN_H_ */
