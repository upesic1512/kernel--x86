#include "System.h"

extern PCB* running;
extern ListPCB* allPCB;
extern ListSem* allSemaphores;

void locking();
void unlocking();


KernelSem::KernelSem(int init){
	locking();
	val=init;
	/*lock
	cout<<"semaphor val: "<<val<<endl;
	unlock*/
	blocked=new ListPCB();
	//dodato i zakomentarisani ispisi
	allSemaphores->add(this);
	unlocking();

	/*lock
		cout<<"number of nodes: "<<blocked->numberOfNodes<<endl;
	unlock*/
	if(blocked->head!=0){
	/*lock
		cout<<"head ZAOSTALA VREDNOST "<<endl;
	unlock*/

	}
}

int KernelSem::signal(int n){
	if(n<0) return n;
	if(n==0)
		n=1;
	locking();
	if(val<0){
		val+=n;
		int i=0;
		for(;i<n; i++){
			PCB* t=blocked->getFirst();
			//dodato
			if(t->state==PCB::FINISHED){
				i--;
				continue;
			}
			if(t==0){
				locking();
				cout<<"NE POSTOJI BLOKIRANA NIT!"<<endl;
				unlocking();
	unlocking();
				return i;
			}
			t->state=PCB::READY;
			Scheduler::put(t);
		}
	unlocking();
		return n;
	}else{
		val+=n;
		/*lock
		cout<<"signal kad nema blokiranih niti!"<<endl;
		unlock*/
	unlocking();
		return 0;
	}

};

//TREBA NAPRAVITI RAZLICITE POVRATNE VREDNOSTI ZA BUDJENJA
int KernelSem::wait(Time maxTimeToWait){
	int ret=1;
	if(maxTimeToWait==0){
		locking();
		maxTimeToWait=-1;
		unlocking();
	}
	locking();
	if(--val<0){
		volatile PCB* t=running;
		t->state=PCB::BLOCKED;
		blocked->add((PCB*)t);
		t->timeToWait=maxTimeToWait;
	unlocking();
		dispatch();

		locking();
		t=running;
		unlocking();
		if(t==0){
			locking();
			cout<<"OGROMNA GRESKA, running null"<<endl;
			unlocking();
		}
		if(t->timeToWait==0){
			ret=0;
		}
		return ret;
	}
	unlocking();
		return ret;
}
