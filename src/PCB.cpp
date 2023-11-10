#include "System.h"
//#include "ListHan.h"

extern PCB* running;
extern ListPCB* allPCB;
//extern ListHan* signalHandlers[];
extern int signalBlockedGlobally[];
ID PCB::uniqueID=0;
void zeroHandler();

void locking();
void unlocking();

//POKRECE RUN METODU I KAD SE ZAVRSI STAVALJA STATUS NA GOTOVO I TRAZI DISPATCH
void PCB::wrapper(){

	running->myThread->run();

	running->state=PCB::FINISHED;
	if(running->waitingPCBs==0){
		lock
		cout<<"Niko ne ceka, kraj!"<<endl;
		unlock
	}
	//oni koji su pozvali waitToComplete
	lock
	if(running->waitingPCBs!=0){
		PCB* cur=running->waitingPCBs->getFirst();
		while(cur!=0){
			//PCB* t=cur;
			cur->state=PCB::READY;
			/*lock
			cout<<"nit vracena u Scheduler"<<cur->myID<<endl;
			unlock*/
			Scheduler::put(cur);
			cur=running->waitingPCBs->getFirst();
		}
	}
	unlock

	locking();
	//javi sebi da si se zavrsio i obradi te handlere
	if(!signalBlockedGlobally[2] && !running->signalBlocked[2]){
		for(NodeHandler* cur=(NodeHandler*)running->signalHandlers[2]->head; cur!=0; cur=cur->next){
			cur->handler();
		}
	}

	//signaliziraj roditelju
	running->parent->signalled[1]=1;
	running->parent->signalQueue->add(1);

	unlocking();
	dispatch();
}

//PCB idle niti
//idleID=0
PCB::PCB(StackSize stackSize, Time timeSlice, void (*body)()){
	stackSize/=sizeof(unsigned);
	if(stackSize>maxStackSize) stackSize=maxStackSize;
	stack = new unsigned[stackSize];

	stack[stackSize-5] =0x200;//setovan I fleg u
                      // pocetnom PSW-u za nit
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize-1] = FP_SEG(body);
	stack[stackSize-2] = FP_OFF(body);

	stack[stackSize-6] = FP_SEG(body);
	stack[stackSize-7] = FP_OFF(body);

sp = FP_OFF(stack+stackSize-16); //svi sacuvani registri
 //pri ulasku u interrupt
 //rutinu
	ss = FP_SEG(stack+stackSize-16);
#endif
	bp=sp;
	state = IDLE;
	this->timeToWait=-1;
	this->timeSlice=timeSlice;
	this->waitingPCBs=new ListPCB();
	this->myID=0;
	//dodato
	this->timeToWait=-1;
	this->myThread=0;
	this->stackSize=stackSize;

	this->parent=running;
	for (int i=0; i<16; i++){
		this->signalHandlers[i]=new ListHan();
		this->signalBlocked[i]=0;
		this->signalled[i]=0;
	}
	this->signalQueue=new ListOrd();
}


//KREIRA PCB KOJI CE DA IZVRSAVA NIT
PCB::PCB(StackSize stackSize, Time timeSlice, Thread* myThread){

	this->myThread=myThread;
	stackSize/=sizeof(unsigned);
		if(stackSize>maxStackSize) stackSize=maxStackSize;
		stack = new unsigned[stackSize];

		stack[stackSize-5] =0x200;//setovan I fleg u
	                      // pocetnom PSW-u za nit
	#ifndef BCC_BLOCK_IGNORE
		stack[stackSize-1] = FP_SEG(myThread);
		stack[stackSize-2] = FP_OFF(myThread);

		stack[stackSize-6] = FP_SEG(PCB::wrapper);
		stack[stackSize-7] = FP_OFF(PCB::wrapper);

		sp = FP_OFF(stack+stackSize-16); //svi sacuvani registri
	 //pri ulasku u interrupt
	 //rutinu
		ss = FP_SEG(stack+stackSize-16);
	#endif
		bp=sp;
		state = READY;
		//dodato
		this->timeToWait=-1;
		this->timeSlice=timeSlice;
		//dodato beskonacno vreme izvrsavanja
		if(timeSlice==0){
			this->timeSlice=-1;
			lock
			//cout<<"Beskonacno vreme izvrsavanja "<<this->timeSlice<<endl;
			unlock
		}
		this->myID=++PCB::uniqueID;
		this->waitingPCBs=new ListPCB();
		allPCB->add(this);
		this->stackSize=stackSize;

		this->parent=running;
		for (int i=0; i<16; i++){
			this->signalHandlers[i]=new ListHan();
			for(NodeHandler* cur=(NodeHandler*)parent->signalHandlers[i]->head; cur!=0; cur=cur->next){
				if(cur->handler!=0){
					this->signalHandlers[i]->add(cur->handler);
				}
			}
			this->signalBlocked[i]=parent->signalBlocked[i];
			this->signalled[i]=parent->signalled[i];
		}
		this->signalQueue=new ListOrd();


}

//PRAVNJENJE PCB-a main niti
//mainID=999
PCB::PCB(){
	stackSize=1024;
	stack = new unsigned[stackSize];

	stack[stackSize-5] =0x200;//setovan I fleg u
					  // pocetnom PSW-u za nit
#ifndef BCC_BLOCK_IGNORE

	sp = FP_OFF(stack+stackSize-16);
//svi sacuvani registri
 //pri ulasku u interrupt
 //rutinu
	ss = FP_SEG(stack+stackSize-16);
#endif
	bp=sp;
	state = READY;

	this->timeSlice=40;
	this->waitingPCBs=new ListPCB();
	//dodato
	this->timeToWait=-1;
	this->myID=999;
	this->myThread=0;
	for (int i=0; i<16; i++){
		this->signalHandlers[i]=new ListHan();
		if(i==0){
			signalHandlers[i]->add(zeroHandler);
		}
		this->signalBlocked[i]=0;
		this->signalled[i]=0;
	}
	this->parent=running;
	this->signalQueue=new ListOrd();

}

PCB::~PCB(){
	lock
	parent=0;
	Node* cur=(Node*)(allPCB->head);
	for(;cur!=0;cur=cur->next){
		if(this==cur->pcb){
			allPCB->removeNode(cur);
			break;
		}
	}
	if(stack){
		delete[] stack;
		stack=0;
	}
	if(waitingPCBs){
		delete waitingPCBs;
		waitingPCBs=0;
	}

	unlock
}


//signals

void PCB::registerHandler(SignalId signal, SignalHandler handler){
	locking();
	this->signalHandlers[signal]->add(handler);
	//cout<<"dodat handler za signal "<<signal<<endl;
	unlocking();
}

void PCB::unregisterAllHandlers(SignalId id){
	locking();
	SignalHandler h=this->signalHandlers[id]->getFirst();
	while(h!=0){
		h=this->signalHandlers[id]->getFirst();
	}

	unlocking();
}


void PCB::signal(SignalId signal){
	locking();
	if(signal>15)
		cout<<"NENORMALAN signal"<<endl;
	if(signal!=1 || signal!=2){
		this->signalled[signal]=1;
		this->signalQueue->add(signal);
	}
	//cout << "thread:" << myID << ":" << signal << ":" << this->signalled[signal] << endl;
	unlocking();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	locking();
	if(this->signalHandlers[id]->head==0) return;
	int nadjen=0;
	NodeHandler* t1=(NodeHandler*)this->signalHandlers[id]->head;

	//trazenje nodova sa zadatim handlerima
	for(; t1!=0; t1=t1->next){
		if(t1->handler==hand1){
			nadjen=1;
			break;
		}
	}
	if(!nadjen) return;
	nadjen=0;
	NodeHandler* t2=(NodeHandler*)this->signalHandlers[id]->head;

	for(; t2!=0; t2=t2->next){
		if(t2->handler==hand2){
			nadjen=1;
			break;
		}
	}
	if(!nadjen) return;

	//zameni im mesta
	NodeHandler* pom1=t1->prev;
	NodeHandler* pom2=t1->next;
	t1->prev=t2->prev;
	t1->next=t2->next;
	t2->prev=pom1;
	t2->next=pom2;
	int headSwapped=0;
	if(t1==this->signalHandlers[id]->head){
		this->signalHandlers[id]->head=t2;
		headSwapped=1;
	}
	if(t2==this->signalHandlers[id]->head && !headSwapped){
		this->signalHandlers[id]->head=t1;
	}

	int tailSwapped=0;
	if(t1==this->signalHandlers[id]->tail){
		this->signalHandlers[id]->tail=t2;
		headSwapped=1;
	}
	if(t2==this->signalHandlers[id]->tail && !tailSwapped){
		this->signalHandlers[id]->tail=t1;
	}

	unlocking();
}
