#ifndef _event_h_
#define _event_h_


typedef unsigned char IVTNo;
class KernelEv;
#include "IVTEntry.h"

//PREPAREENTRY makro
#ifndef BCC_BLOCK_IGNORE
#define PREPAREENTRY(ivtNo, callOldFlag) \
void interrupt inter##ivtNo(...); \
IVTEntry ivtEntry##ivtNo(ivtNo, inter##ivtNo); \
void interrupt inter##ivtNo(...) { \
	ivtEntry##ivtNo.signal(); \
	if(callOldFlag) \
		ivtEntry##ivtNo.callOld(); \
}
#endif



class Event{
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal(); // can call KernelEv

private:
	KernelEv* myImpl;
};



#endif
