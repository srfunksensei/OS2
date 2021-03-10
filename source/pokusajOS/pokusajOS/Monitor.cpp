//file: monitor.cpp

#include "lista.h"
#include "Monitor.h"
#include "KernelFS.h"
#include <windows.h>
#include <iostream>
using namespace std;

Monitor::Monitor(){
	nr = nw = na = dr = dw = da = 0;
	rMutex = CreateSemaphore(NULL, 0, MAX_SEM_CNT, NULL);
	wMutex = CreateSemaphore(NULL, 0, MAX_SEM_CNT, NULL);
	aMutex = CreateSemaphore(NULL, 0, MAX_SEM_CNT, NULL);
	db = CreateSemaphore(NULL, 1, 1, NULL);
}

int Monitor::startRead(){
	WaitForSingleObject(db, INFINITE);
	if(nw > 0 || na > 0){
		dr++; ReleaseSemaphore(db, 1, NULL); 
		ReleaseMutex(KernelFS::mutexFile);
		WaitForSingleObject(rMutex, INFINITE);
		WaitForSingleObject(KernelFS::mutexFile, INFINITE);
	}
	nr++;
	if(dr > 0) {dr--; ReleaseSemaphore(rMutex, 1, NULL);}
	else ReleaseSemaphore(db, 1, NULL);

	return nr;
}

int Monitor::endRead(){
	WaitForSingleObject(db, INFINITE);
	nr--;
	if (nr == 0 && dw > 0) { dw--; ReleaseSemaphore(wMutex, 1, NULL);}
	else if( nr==0 && da > 0){ da--; ReleaseSemaphore(aMutex, 1, NULL);}
	else ReleaseSemaphore(db, 1, NULL);

	return nr;
}

int Monitor::startWrite(){
	WaitForSingleObject(db, INFINITE);
	if (nr > 0 || nw > 0 || na > 0) { 
		dw++; ReleaseSemaphore(db, 1, NULL); 
		ReleaseMutex(KernelFS::mutexFile);
		WaitForSingleObject(wMutex, INFINITE); 
		WaitForSingleObject(KernelFS::mutexFile, INFINITE);
	}
	nw = nw + 1;
	ReleaseSemaphore(db, 1, NULL);

	return nw;
}

int Monitor::endWrite(){
	WaitForSingleObject(db, INFINITE);
	nw--;
	if (dr > 0) { dr--; ReleaseSemaphore(rMutex, 1, NULL); }
	else if (dw > 0) { dw--; ReleaseSemaphore(wMutex, 1, NULL);}
	else if(da > 0) { da--; ReleaseSemaphore(aMutex, 1, NULL);}
	else ReleaseSemaphore(db, 1, NULL);

	return nw;
}

int Monitor::startApend(){
	WaitForSingleObject(db, INFINITE);
	if (nr > 0 || nw > 0 || na > 0) { 
		da++; ReleaseSemaphore(db, 1, NULL); 
		ReleaseMutex(KernelFS::mutexFile);
		WaitForSingleObject(aMutex, INFINITE); 
		WaitForSingleObject(KernelFS::mutexFile, INFINITE);
	}
	na = na + 1;
	ReleaseSemaphore(db, 1, NULL);

	return na;
}

int Monitor::endApend(){
	WaitForSingleObject(db, INFINITE);
	na--;
	if (dr > 0) { dr--; ReleaseSemaphore(rMutex, 1, NULL); }
	else if(da > 0) { da--; ReleaseSemaphore(aMutex, 1, NULL);}
	else if (dw > 0) { dw--; ReleaseSemaphore(wMutex, 1, NULL);}
	
	else ReleaseSemaphore(db, 1, NULL);

	return na;
}