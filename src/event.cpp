#include "System.h"
#include "event.h"
#include "KernelEv.h"

void locking();
void unlocking();


Event::Event(IVTNo ivtNo){
	locking();
	myImpl=new KernelEv(ivtNo);
	unlocking();
}

void Event::signal(){
	myImpl->signal();
}


void Event::wait(){
	myImpl->wait();
}

Event::~Event(){
	delete myImpl;
}
