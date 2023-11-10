/*
 * System.h
 *
 *  Created on: May 15, 2020
 *      Author: OS1
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


//header files
#include "PCB.h"
#include "Thread.h"
#include "SCHEDULE.h"
#include <dos.h>
#include <iostream.h>

//#include "event.h"
#include "ListPCB.h"
#include "kerSem.h"
#include "semaphor.h"
#include "ListSem.h"
//#include "kernelEv.h"
//#include "IVTEntry.h"


//LOCKS
#define lock asm{\
 		pushf;\
 		cli;\
}
#define unlock asm popf



#endif /* SYSTEM_H_ */
