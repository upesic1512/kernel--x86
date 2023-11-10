//pretpostavljeni memorijski model: huge

#include "System.h"
//#include "IVTEntry.h"

#include <STDIO.H>
#include <STDARG.H>

int t=-1;
volatile Time ts;

void tick();

void idleFun(){
	while(1){
		//syncPrintf("IDLE");
	};
};

PCB* mainPCB=0;
PCB* idlePCB=new PCB(1024,1,idleFun);
volatile PCB* running;
volatile ListPCB* allPCB=new ListPCB();
volatile ListSem* allSemaphores=new ListSem();
class IVTEntry;
volatile IVTEntry* ivtEntries[256];

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int brojac = 20;
volatile int zahtevana_promena_konteksta = 0;
volatile int cnt=0;

void locking(){
	cnt++;
}

void unlocking(){
	cnt--;
}

//////////////////signals///////////////////////////
volatile int signalBlockedGlobally[16]={0};
//volatile ListHan* signalHandlers[16]={0};

typedef void (*SignalHandler)();
typedef unsigned SignalId;


////////////////////////////////////////////////////



int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	locking();
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlocking();
		return res;
}

//DOBRE SU ZAGRADE SAMO LOSE SKUPLJA
void interrupt timer(){	// prekidna rutina
	if (!zahtevana_promena_konteksta && cnt==0){
		brojac--;
		tick();

		//OSLOBODI PCB AKO JE DOVOLJNO CEKAO NA SEMAFORU! (dodato)
		NodeSem* curSem=0;
		if(allSemaphores!=0){
		curSem=(NodeSem*)allSemaphores->head;
		//syncPrintf("Broj semafora : %d\n", allSemaphores->numberOfNodes);
		}

		//prodji kroz sve semafore
		for(;curSem!=0; curSem=curSem->next){

			//trenutna lista blokiranih na tekicem semaforu
			ListPCB* curList=curSem->semaphore->blocked;
			//curNode - trenutno se ulazi u cvor u listi blokiranih na trenutnom semaforu
			Node* curNode=(Node*)(curSem->semaphore->blocked->head);
			//dok se ne dodje do kraja liste blokiranih cvorova na semaforu
			while(curNode!=0){
				if(curNode->pcb->timeToWait!=0){
					curNode->pcb->timeToWait--;
				}
				//syncPrintf("id: %d, timeToWait: %d \n", curNode->pcb->myID, curNode->pcb->timeToWait);

				if(curNode->pcb->timeToWait==0){
					//izbaci iz liste blokiranih, ubaci u Scheduler i stavi mu stanje na READY

					PCB* curPCB=curNode->pcb;
					Node* toRemove=curNode;
					curNode=curNode->next;
					curList->removeNode(toRemove);
					curSem->semaphore->val++;

					/////////signali///////////////
					if(curPCB->state==PCB::FINISHED)
						continue;
					///////////////////////////////
					curPCB->state=PCB::READY;
					Scheduler::put(curPCB);
					continue;
				}
				curNode=curNode->next;
			}
		}

	}

	if (brojac == 0 || zahtevana_promena_konteksta) {
		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		running->sp = tsp;
		running->ss = tss;
		running->bp = tbp;

		if(running->state==PCB::READY)
			Scheduler::put((PCB*)running);

		running= Scheduler::get();	// Scheduler
		if (running==0){
			running=idlePCB;
		}

		//PROBLEM AKO NEMA SPREMNIH NITI !!!

		tsp = running->sp;
		tss = running->ss;
		tbp = running->bp;

		brojac = running->timeSlice;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}

		//obradi pristigle signale
		//for(int i=15; i>=0; i--){
			//if(running->signalled[i]){
				//cout<<"signalizirano za "<<running->myID<<" signal "<<i<<endl;

		for(NodeOrd* tek=(NodeOrd*)running->signalQueue->head; tek!=0;){
			//if(tek==0) cout<<"PRISTUPAS NULL"<<endl;
			//cout<<"signalizirano za "<<running->myID<<" signal "<<tek->number<<endl;
			if(tek->number<0 || tek->number>15){ continue;}
			if(!signalBlockedGlobally[tek->number] && !running->signalBlocked[tek->number]){
				for(NodeHandler* cur=(NodeHandler*)running->signalHandlers[tek->number]->head; cur!=0; cur=cur->next){
					if(cur->handler!=0){
						//cout<<i<<endl;
						cur->handler();
					}
				}
				NodeOrd* toRemove=tek;
				tek=tek->next;
				running->signalQueue->removeNode(toRemove);
			}
			else{
				tek=tek->next;
			}
		}
				/*if(!signalBlockedGlobally[i] && !running->signalBlocked[i]){
					for(NodeHandler* cur=(NodeHandler*)running->signalHandlers[i]->head; cur!=0; cur=cur->next){
						if(cur->handler!=0){
							//cout<<i<<endl;
							cur->handler();
						}
					}

					running->signalled[i]=0;

				}*/
		//	}
		//}

	}

	if(!zahtevana_promena_konteksta) asm int 60h;
	zahtevana_promena_konteksta = 0;

}

void dispatch(){ // sinhrona promena konteksta
	asm cli;
	zahtevana_promena_konteksta = 1;
	timer();
	asm sti;
}


//zeroHandler


void zeroHandler(){
	running->state=PCB::FINISHED;
		if(running->waitingPCBs==0){
			locking();
			cout<<"Niko ne ceka, kraj!"<<endl;
			unlocking();
		}
		//oni koji su pozvali waitToComplete
		locking();
		if(running->waitingPCBs!=0){
			PCB* cur=running->waitingPCBs->getFirst();
			while(cur!=0){
				//PCB* t=cur;
				cur->state=PCB::READY;
				Scheduler::put(cur);
				cur=running->waitingPCBs->getFirst();
			}
		}

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

//sacuvaj staru prekidnu rutinu za tajmer
//-----------------------------------------------------------------------------------------------------------------

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

// postavlja novu prekidnu rutinu (TIMER)
void inic(){
	//SETOVANJE PREKIDNE RUTINE TIMER-a
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}

	//PRAVLJENJE MAIN PCB-a
		locking();
		mainPCB = new PCB();
		running = mainPCB;
		unlocking();
	//pravljenje liste niza listi za Handlere
		/*for (int i=0; i<16; i++){
			signalHandlers[i]=new ListHan();
		}
		signalHandlers[0]->add(zeroHandler);
		*/

}

// vraca staru prekidnu rutinu (TIMER)
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}

	//dealokacija mainPCB

	delete mainPCB;
	//mozda da ne brises IDLE
	delete idlePCB;
	PCB* tek=0;
	tek=allPCB->getFirst();
	while(tek!=0){
		delete tek;
		tek=allPCB->getFirst();
	}
	delete allPCB;
	allPCB=0;
	//dodaj brisanje semafora!
	delete allSemaphores;
	allSemaphores=0;

}


int userMain(int argc, char* argv[]);


int main(int argc, char* argv[]){
  inic();
  //doSomething();
  int ret=userMain(argc, argv);
  PCB* pom=Scheduler::get();

  /*if(pom==0){
	  syncPrintf("Scheduler prazan, dobro je\n");
  }
  else
	  syncPrintf("NIJE prazan!!!!");*/

  syncPrintf("GOTOV userMain ret=");
  locking();
  cout<<ret<<endl;
  unlocking();

  restore();
  return 0;
}

