// file: kernelfile.cpp

#include "file.h"
#include "Monitor.h"
#include "kernelfile.h"
#include "kernelfs.h"
#include <iostream>
using namespace std;

KernelFile::KernelFile ( PartInfo* pi){
	this->currentPos = 0;
	this->endOfFile = 0;
	this->firstCls = 0;
	this->inCls = 1;
	this->numOfCls = 1;
	this->mode = 'c';

	this->p = pi;
	this->clsSize = p->getPart()->getClusterSize() * p->getPart()->getBlockSize();
	this->cls = new char[clsSize];

	fileMonitor = Monitor();
	mutex = CreateMutex( NULL,FALSE, NULL);

	copy = false;

}
KernelFile::~KernelFile(){
	delete [] cls;
	p = 0;
	delete file;
}

KernelFile::KernelFile(const KernelFile* kf){
	copy = true;

	this->currentPos = currentPos;
	this->endOfFile = kf->endOfFile;
	this->firstCls = kf->firstCls;
	this->inCls = kf->inCls;
	this->numOfCls = kf->numOfCls;
	this->mode = kf->mode;

	this->p = kf->p;
	this->clsSize = kf->clsSize;
	this->cls = new char[this->clsSize]; //kf->cls;
	//for(int i = 0; i< this->clsSize; i++) this->cls[i] = kf->cls[i];
	if( this->p->getPart()->readCluster(this->firstCls, this->cls) == 0) cout<<"GRESKA\n";

	fileMonitor = kf->fileMonitor;
	
}
char KernelFile::write (BytesCnt cnt, char* buffer){
	if(mode == 'c') return 0;
	if( currentPos > endOfFile) return 0;

	BytesCnt forWrite = cnt;

	/* pisi do kraja klastera, ako si upisao premalo dovuci 
	novi klaster (a prethodni vrati na particiju)i upisi u njega */
	
	for(int i = 0; i< forWrite; i++){
		if((currentPos % clsSize) != 0 || currentPos == 0){
			cls [currentPos % clsSize]  = buffer[i];
			currentPos++;
			if (currentPos > endOfFile) endOfFile++;
			else if (currentPos == endOfFile) return 1;

		} else {

			ClusterNo first = firstCls;
			char fat[131072]; //KernelFS::FatSizeByte;
			WaitForSingleObject(KernelFS::mutexFAT, INFINITE);
			p->loadFat(p->getFat1() , fat);
			ReleaseMutex(KernelFS::mutexFAT);

			if ( inCls == numOfCls){
				unsigned long i;
				for(i = 4; i<KernelFS::FatSizeByte; i+=2){
					if(fat[i] == 0 && fat[i+1]== 0) {
						fat[i] = (char)0xFF;
						fat[i+1] = (char)0xFF;

						numOfCls++;

						p->storeFat(p->getFat1(), fat);
						p->storeFat(p->getFat2(), fat);

						break;
					}
				}

				int num = 1;
				while ( inCls > num){
					first = getNextCls(first, fat);
					num++;
				}

				//azuriramo ulaz u fatu za fajl
				fat[first * 2] = (char)( ((unsigned short)i/2) & 0x00FF);
				fat[first * 2 + 1] = (char)( ((unsigned short)i/2) & 0xFF00);
				
				p->storeFat(p->getFat1() , fat);
				p->storeFat(p->getFat2() , fat);

				
			} else {
				int num = 1;
				while ( inCls > num){
					first = getNextCls(first, fat);
					num++;
				}
			}
			

			if( p->getPart()->writeCluster(first, cls) == 0) return 0;
			first = getNextCls(first, fat);
			inCls++;
			if( p->getPart()->readCluster(first, cls) == 0) return 0;

			cls [currentPos % clsSize]  = buffer[i];
			currentPos++;
			if (currentPos >= endOfFile) endOfFile++;
		}		
	}

	return 1;
}
BytesCnt KernelFile::read (BytesCnt cnt, char* buffer){
	if(mode == 'c') return 0;
	if( currentPos > endOfFile) return 0;

	BytesCnt forRead = cnt;
	if( currentPos+ cnt > endOfFile) forRead = endOfFile - currentPos;

	/* citaj do kraja klastera, ako si procitao premalo dovuci 
	sledeci klaster (a prethodni vrati na particiju)i citaj iz njega */
	
	for(int i = 0; i< forRead; i++){
		if((currentPos % clsSize) != 0 || currentPos == 0){
			buffer[i] = cls [currentPos % clsSize];
			currentPos++;

		} else {

			ClusterNo first = firstCls;
			char fat[131072];//KernelFS::FatSizeByte
			p->loadFat(p->getFat1() , fat);

			int num = 1;
			while ( inCls > num){
				first = getNextCls(first, fat);
				num++;
			}
			

			first = getNextCls(first, fat);
			inCls++;
			if( p->getPart()->readCluster(first, cls) == 0) return 0;

			buffer[i] = cls [currentPos % clsSize];
			currentPos++;
		}		
	}

	return forRead > 0? forRead : 0;
}
char KernelFile::truncate (){
	
	if(mode == 'c') return 0;
	if( currentPos > endOfFile) return 0;
	if( currentPos == endOfFile) return 1;

	unsigned long cnt = endOfFile - currentPos;
	int num = cnt / clsSize; num++;
	
	if(inCls == num) endOfFile = currentPos;
	else if (inCls > num){//unazad
		ClusterNo first = firstCls,last = firstCls;
		char fat[131072];//KernelFS::FatSizeByte
		p->loadFat(p->getFat1() , fat);

		inCls = 1;

		int num1 = 1;
		while(num1< numOfCls){
			last = getNextCls(last, fat);
			num1++;
		}

		num1 = 1;
		while ( num > num1){
			first = getNextCls(first, fat);
			num1++;
			inCls++;
		}
		
		//if( p->getPart()->writeCluster(last, cls) == 0) return 0;
		if( p->getPart()->readCluster(first, cls) == 0) return 0;

		endOfFile = currentPos;

	}else if (inCls < num){//unapred
		ClusterNo first = firstCls,last = firstCls;
		char fat[131072];//KernelFS::FatSizeByte
		p->loadFat(p->getFat1() , fat);

		int num1 = 1;
		while (num1 < inCls) {
			first = getNextCls(first, fat);
		}

		inCls = 1;

		while ( num > inCls) {
			last = getNextCls(last, fat);
			inCls++;

		}

		//if( p->getPart()->writeCluster(first, cls) == 0) return 0;
		if( p->getPart()->readCluster(last, cls) == 0) return 0;

		endOfFile = currentPos;
	}

	return 1;
}

char KernelFile::seek (BytesCnt cnt){

	if(cnt < 0 || cnt > endOfFile) return 0;
	if(mode == 'c')	return 0;

	int num = cnt / clsSize; num++;
	
	if(inCls == num) currentPos = (cnt == 0? 0: cnt-1);
	else if (inCls > num){//unazad
		ClusterNo first = firstCls,last = firstCls;
		char fat[131072];//KernelFS::FatSizeByte
		p->loadFat(p->getFat1() , fat);

		inCls = 1;

		int num1 = 1;
		while(num1< numOfCls){
			last = getNextCls(last, fat);
			num1++;
		}

		num1 = 1;
		while ( num > num1){
			first = getNextCls(first, fat);
			num1++;
			inCls++;
		}
		
		//if( p->getPart()->writeCluster(last, cls) == 0) return 0;
		if( p->getPart()->readCluster(first, cls) == 0) return 0;

		currentPos = (cnt == 0? 0: cnt-1);

	}else if (inCls < num){//unapred
		ClusterNo first = firstCls,last = firstCls;
		char fat[131072];//KernelFS::FatSizeByte
		p->loadFat(p->getFat1() , fat);

		int num1 = 1;
		while (num1 < inCls) {
			first = getNextCls(first, fat);
		}

		inCls = 1;

		while ( num > inCls) {
			last = getNextCls(last, fat);
			inCls++;

		}

		//if( p->getPart()->writeCluster(first, cls) == 0) return 0;
		if( p->getPart()->readCluster(last, cls) == 0) return 0;

		currentPos = (cnt == 0? 0: cnt-1);
	}

	return 1;
}

BytesCnt KernelFile::getFileSize (){
	return endOfFile + 1;
}

char KernelFile::eof (){
	if(endOfFile == currentPos) return 2;
	if(endOfFile > currentPos) return 0;
	if(endOfFile < currentPos) return 1;
	return 0;
}

void KernelFile::close (){
	if (mode == 'c') return;

	int nr;
	switch(mode){
		case 'r' : 
			nr = fileMonitor.endRead(); 
			if(this->copy == false && nr > 0){
				return;
			}
			break;
		case 'w': fileMonitor.endWrite(); break;
		case 'a': fileMonitor.endApend(); break;
	}

	mode = 'c';
	ReleaseMutex(KernelFS::mutexL);
}  

// POSTAVLJANJE PRVOG KLASTERA FAJLA
void KernelFile::setFirstCls(ClusterNo cls){
	firstCls = cls;
}
//POSTAVLJANJE KURSORA U FAJLU
int KernelFile::setCurrentPos(BytesCnt cnt){
	int num = cnt / clsSize; num++;
	
	ClusterNo first = firstCls;
	char fat[131072]; //KernelFS::FatSizeByte
	p->loadFat(p->getFat1() , fat);

	if(inCls == num) currentPos = cnt;
	else {
		inCls = 1;
		
		int num1 = 1;
		while ( num > num1){
			first = getNextCls(first, fat);
			num1++;
			inCls++;
		}
		
		if( p->getPart()->writeCluster(first, cls) == 0) return 0;
		first = getNextCls(first, fat);
		inCls++;
		if( p->getPart()->readCluster(first, cls) == 0) return 0;

		currentPos = cnt;

	}

	return 1;
}
//VRACANJE KRAJA FAJLA
BytesCnt KernelFile::getEOF(){
	return endOfFile;
}
//DOBIJANJE SLEDECEG KLASTERA
ClusterNo KernelFile::getNextCls(ClusterNo cluster, char* fat){
	unsigned long ulaz = cluster *2;
	unsigned short result = 0xFFFF;

	result = (unsigned short)(fat[ulaz + 1]<<8)|(fat[ulaz]);

	return result;
}
//POSTAVLJANJE FAJLA
void KernelFile::setFile(File * f){
	this->file = f;
	f->myImpl = this;
}
//VRACANJE I POSTAVLJANJE MODA U KOM JE FAJL
char KernelFile::getMode(){
	return mode;
}
void KernelFile::setMode(char c){
	mode = c;
}
