//file: monitor.h

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <windows.h>
#include <iostream>
using namespace std;

#define MAX_SEM_CNT 10

class KernelFile;

class Monitor{

public:
	Monitor();
	~Monitor(){}

	int startRead();
	int endRead();

	int startWrite();
	int endWrite();

	int startApend();
	int endApend();

private:
	int nr, nw, na;  // num of r/w
	int dr, dw, da;  //delayed r/w

	HANDLE rMutex, wMutex, aMutex; //mutex r/w
	HANDLE db;  //mutexDB
};

#endif