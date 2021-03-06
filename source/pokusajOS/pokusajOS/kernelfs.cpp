#include "kernelfs.h"
#include "fs.h"
#include "part.h"
#include "file.h"
#include "kernelfile.h"
#include "lista.h"
#include <windows.h>
#include <cstring>
#include <iostream>
using namespace std;


PartInfo* KernelFS::partArray[]={0};
int KernelFS::partLetter[]={0};

unsigned long KernelFS::BootSectorSizeByte=512;
unsigned long KernelFS::FatSizeByte=131072;
unsigned long KernelFS::RootSizeByte=16384;

HANDLE KernelFS::mutex = CreateMutex( NULL,FALSE, NULL);
HANDLE KernelFS::mutexFile = CreateMutex( NULL,FALSE, NULL);
HANDLE KernelFS::mutexDir = CreateMutex( NULL,FALSE, NULL);
HANDLE KernelFS::mutexFAT = CreateMutex( NULL,FALSE, NULL);
HANDLE KernelFS::mutexL = CreateMutex( NULL,FALSE, NULL);
Lista KernelFS::listOfAllFiles = Lista();
Lista KernelFS::listCopy = Lista();

KernelFS::KernelFS() {
	pwdName = new char[MAX_PATH_LENGTH];
	pwdName[0] = '\0';

}

KernelFS::~KernelFS(){
	delete [] pwdName;
}

char KernelFS::mount(Partition *partition){
	WaitForSingleObject(mutex, INFINITE);
	if( partition != 0){
	  int i;
	  for( i =0; i<26; i++){
		  if(partLetter[i]==0){
			partLetter[i] = 1;
			partArray[i] = new PartInfo(partition);
			partArray[i]->setLetter('A' + i);

			ReleaseMutex(mutex);
			return partArray[i]->getLetter();
		  }
	  }
	  
	}
	
	ReleaseMutex(mutex);
	return '\0';
}

char KernelFS::unmount(char part){

	while(listOfAllFiles.imaOtvorenih()){
		WaitForSingleObject(mutexL, INFINITE);
	}

	WaitForSingleObject(mutex, INFINITE);
	if( 'A' <= part && part <= 'Z'){
	  for(int i = 0; i<26; i++){
		  if(partLetter[i] != 0){
		  char letter = partArray[i]->getLetter();
			  if(part == letter){
			
				partLetter[i] = 0;

				Partition* p = partArray[i]->getPart();
				delete p;

				partArray[i] = 0;

				ReleaseMutex(mutex);
				return '1';
			  }
		  }
	  }
	}

	ReleaseMutex(mutex);
	return '0';
	

}

char KernelFS::format(char partL){
	WaitForSingleObject(mutex, INFINITE);
	if( 'A' <= partL && partL <= 'Z'){
		for(int i  = 0; i<26 ; i++){
			if(partLetter[i]!=0){
				if(partArray[i]->getLetter() == partL){
		  Partition* part = partArray[i]->getPart();
		  BlockNo bs = part->getBlockSize();
		  BlockNo nb = part->getNumOfBlocks();
		  ClusterNo cs = part->getClusterSize();
		BlockNo co = part->getClusterOffset();
		BlockNo start = part->getStartBlock();
		long nh = part->getNumOfHead();
		long ns = part->getNumOfSectors() * part->getNumOfCylinders();
		
		long bytesInSector = nb * bs / ns;
		long numSectorsInClusters = cs * bs / ns;
		long brSkrivenihSektora = 2*cs/ns;
		int numOfSectorsPerCylinder = part->getNumOfSectors();

		char buff[512]; //KernelFS::BootSectorSizeByte
		if(buff == 0) return '0';
		
		*(buff + 0x00) = '0';
		*(buff + 0x01) = '0';
		*(buff + 0x02) = '0';

		
		strcpy((buff + 0x03) , "IR3OS2  ");

		
		*(buff + 0x0b)=((bytesInSector & 0xFF00) >> 8);
		*(buff + 0x0c)=(bytesInSector & 0xFF);

		*(buff + 0x0d)=(numSectorsInClusters & 0xFF); 
		
		*(buff + 0x0e)='1';
		*(buff + 0x0f) = '0';

		*(buff + 0x10)='2';

		*(buff + 0x11)=(0x200 & 0xFF00) >> 8;
		*(buff + 0x12)=(0x200 & 0xFF);

		if (ns == 0){
				*(buff + 0x20) = ((ns & 0xFF000000) >> 24);
				*(buff + 0x21) = ((ns & 0xFF0000) >> 16);
				*(buff + 0x22) = ((ns & 0xFF00) >> 8);
				*(buff + 0x23) = (ns & 0xFF); 

				*(buff + 0x11) = '0';
				*(buff + 0x12) = '0';

			}
			else if (ns>0xFFFF){
				*(buff + 0x11) = ((ns & 0xFF00) >> 8);
				*(buff + 0x12) = (ns & 0xFF); 

				*(buff + 0x20) = '0';
				*(buff + 0x21) = '0';
				*(buff + 0x22) = '0';
				*(buff + 0x23) = '0';
			}
		
		
		*(buff + 0x15) = (char) 0xf8; 

	unsigned long mod = (nb - co) % cs;
	unsigned long clusterNumber = (nb - co)/cs;

	if (mod != 0)	clusterNumber++;
	
	clusterNumber = clusterNumber + 2; // First two virtual entries 
	
	long FATsize = clusterNumber*2 / bytesInSector;


		*(buff + 0x16) = ((FATsize & 0xFF00) >> 8);
		*(buff + 0x17) = (FATsize & 0xFF);

		*(buff + 0x18) = ((numOfSectorsPerCylinder & 0xFF00) >> 8);
		*(buff + 0x19)  = (numOfSectorsPerCylinder & 0xFF);

		*(buff + 0x1a) = (nh & 0xFF00) >> 8;
		*(buff + 0x1b) = (nh & 0xFF);
		
		*(buff + 0x1c) = ((brSkrivenihSektora & 0xFF000000) >> 24);
		*(buff + 0x1d) = ((brSkrivenihSektora & 0xFF0000) >> 16);
		*(buff + 0x1e) = ((brSkrivenihSektora & 0xFF00) >> 8);
		*(buff + 0x1f) = (brSkrivenihSektora & 0xFF); 

		*(buff + 0x24) = '1';
		*(buff + 0x25) = '0';

		*(buff + 0x26) = (char) 0x29;

		*(buff + 0x27) = '0';
		*(buff + 0x28) = '0';
		*(buff + 0x29) = '0';
		*(buff + 0x2a) = '0';

		strcpy((buff + 0x2b) , "DATA       ");

		strcpy((buff + 0x36) , "FAT16   ");

		for (int j=0x3E; j<0x1FE; j++) buff[j] = 0;


		*(buff + 0x1fe) = (char)0x55;
		*(buff + 0x1ff) = (char)0xAA;

		// writeBlock
		
		mod = sizeof(buff)%bs;
		unsigned long n = sizeof(buff)/bs;

		if (mod != 0) n++;

		for(unsigned long j = 0; j<n; j++){
			part->writeBlock(start + j, buff + j*bs);
		}

	// KRAJ BOOT SEKTORA

		//insert fat1 & fat2
		char fat1[131072];//KernelFS::FatSizeByte
		if(fat1 == 0 ) return '0';

		fat1[0]=(char)0xf8;
		fat1[1]=(char)0;
		fat1[2]=(char)0xff;
		fat1[3]=(char)0xff;
		for(unsigned int j=4;j<KernelFS::FatSizeByte;j++) fat1[j]=(char)0;
		
		// writeBlock
		start+=n;
		partArray[i]->setFat1(start);

		mod = sizeof(fat1)%bs;
		n = sizeof(fat1)/bs;
		//start = start+ KernelFS::BootSectorSizeByte;
		
		if (mod != 0) n++;

		for(unsigned long j = 0; j<n; j++){
			part->writeBlock(start + j, fat1 + j*bs);
		}

		start+=n;
		partArray[i]->setFat2(start);
		//start = start+ KernelFS::BootSectorSizeByte + KernelFS::FatSizeByte;

		for(unsigned long j = 0; j<n; j++){
			part->writeBlock(start + j, fat1 + j*bs);
		}

	//KRAJ FATA
		char root[16384]; //KernelFS::RootSizeByte
		if (root == 0) return '0';
		
		*(root + 0x00) = partL;
		*(root + 0x01) = ':';
		*(root + 0x02) = ' ';
		*(root + 0x03) = ' ';
		*(root + 0x04) = ' ';
		*(root + 0x05) = ' ';
		*(root + 0x06) = ' ';
		*(root + 0x07) = ' ';

		*(root + 0x08) = (char) 0;;
		*(root + 0x09) = (char) 0;;
		*(root + 0x0a) = (char) 0;;

		*(root + 0x0b) = (char) 0x08;

		for (int j=0x0c; j<0x20; j++) root[j] = (char) 0;
		for(unsigned long j = 0x20; j<KernelFS::RootSizeByte; j++) root[j] = (char) 0;

		// writeBlock
		start+=n;
		partArray[i]->setRoot(start);

		mod = sizeof(root)%bs;
		n = sizeof(root)/bs;
		//start = start+ KernelFS::BootSectorSizeByte + 2*KernelFS::FatSizeByte;

		if (mod != 0) n++;
		
		for(unsigned long j = 0; j<n; j++){
			part->writeBlock(start + j, root + j*bs);
		}

	//KRAJ ROOT

	//CLUSTER_OFFSET_SIZE
		BlockNo offSet=(KernelFS::BootSectorSizeByte+2*KernelFS::FatSizeByte+KernelFS::RootSizeByte)/bs;
		part->setClusterSize(2);
		part->setClusterOffset(offSet);
		
		ReleaseMutex(mutex);
		return 1;
	}
	}
	}
	}
	
	ReleaseMutex(mutex);
	return '0';
}

BytesCnt KernelFS::freeSpace(char part){
	if (part<'A' || part > 'Z') return -1;
	for(int i = 0; i<26 ; i++){
		if(partLetter[i]!=0){
			if(partArray[i]->getLetter() == part){
		BytesCnt freeSpaceBytes = 0;
		
		Partition* p = partArray[i]->getPart();

		//num of fat entrys
		BlockNo numOfBlocks = p->getNumOfBlocks();
		BlockNo clusterOffset = p->getClusterOffset();
		ClusterSize clusterSize = p->getClusterSize();
		BlockSize blockSize = p->getBlockSize();

		int mod = (numOfBlocks - clusterOffset) % clusterSize;
		ClusterNo clusterNumber = (numOfBlocks - clusterOffset)/clusterSize;

		if (mod != 0) clusterNumber++;
		
//DOVUCI FAT
		int numOfBlocks1 = KernelFS::FatSizeByte / blockSize;
		if (KernelFS::FatSizeByte % blockSize) numOfBlocks1++;
		
		BlockNo start = partArray[i]->getFat1();

		char* fat = new char[numOfBlocks * blockSize];
		for(int j = 2; j<numOfBlocks1; j++)
			p->readBlock( start , fat + j*blockSize);

		for(ClusterNo j = 2; j<clusterNumber; j++)
			if(fat[j] == 0) freeSpaceBytes++;					

		delete[] fat;

		freeSpaceBytes = 2 * freeSpaceBytes * clusterSize * blockSize;

		return freeSpaceBytes;
	}
		}
	}
	return 0;
}

BytesCnt KernelFS::partitionSize(char part){
	if (part<'A' || part > 'Z') return -1;

	for(int i = 0; i<26; i++){
		if(partLetter[i]!=0){
			if(partArray[i]->getLetter() == part){
				Partition* p = partArray[i]->getPart();
				BlockNo numOfBlocks = p->getNumOfBlocks();
				BlockNo clusterOffset = p->getClusterOffset();
				ClusterSize clusterSize = p->getClusterSize();
				BlockSize blockSize = p->getBlockSize();

				int mod = (numOfBlocks - clusterOffset) % clusterSize;
				int clusterNumber = (numOfBlocks - clusterOffset)/clusterSize;

				if (mod != 0) clusterNumber++;
				int partitionSize = clusterNumber * clusterSize * blockSize;
		
			return partitionSize;
	}
		}
	}
  return 0;
}


char* KernelFS::pwd(){
	return this->pwdName;
}


char KernelFS::doesExist(char* fname){
	char path[MAX_PATH_LENGTH];
	createApsolutePath(fname, path);

	int numNames = countNames(path);

	PartInfo* part = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == path[0]) {
			part = partArray[i];
			break;
		}
	}

	if(part == 0) return 0;
	if( strlen(path) == 3 || strlen(path) == 2) return 1;
	
	unsigned long cs = part->getPart()->getClusterSize() * part->getPart()->getBlockSize();
	char 
		*root = new char[KernelFS::RootSizeByte];
		
	if( root == 0) return 0;

	if (part->loadRoot(part->getRoot(), root) ==0 ) return 0;


	char * pomRoot = &root[32];
	Entry* nizEnt, *nizEnt1;


	char pom[MAX_PATH_LENGTH], pom1[FNAMELEN];
	unsigned int first_cls = 0;
	Partition* p = part->getPart();
	int  j = 3;

	nizEnt = (Entry *) pomRoot;

	for( int i = 0; i< countNames(path); i++){
		if(i == 0) {
			int k= 0 , ulaz = 0;
			while(path[j] != '\\' && path[j] != '\0' && path[j]!= '.'){
				pom[k++] = path[j++];
			}
			pom[k] = '\0';
			numNames--;

			for( k = 32; k<KernelFS::RootSizeByte; k+=sizeof(Entry), ulaz++){
				strncpy(pom1, root + k, FNAMELEN);
				if ( strcmp(pom, pom1) == 0) {
					if(numNames == 0) {
						delete [] root;

						pomRoot = 0;
						nizEnt = 0;
						nizEnt1 = 0;
					
						return 1;

					}
					first_cls = nizEnt[ulaz].firstCluster;

					break;
				}
			}
			if( k == KernelFS::RootSizeByte) return 0;
		} else{

			char *ccls = new char[cs];
			if (ccls == 0) return 0;

			if( p->readCluster(first_cls, ccls) == 0) return 0;
			nizEnt1 = (Entry *) ccls;

			int k= 0, ulaz = 0;
			j++;
			while(path[j] != '\\' && path[j] != '\0' && path[j]!= '.'){
				pom[k++] = path[j++];
			}
			pom[k] = '\0';
			numNames--;

			for( k = 0; k<cs; k+=sizeof(Entry), ulaz++){
				strncpy(pom1, ccls + k, FNAMELEN);
				if ( strcmp(pom, pom1) == 0) {
					if(numNames == 0) {
						delete [] root;

						pomRoot = 0;
						nizEnt = 0;
						nizEnt1 = 0;
					
						return 1;

					}

					first_cls = nizEnt1[ulaz].firstCluster;

					break;
				}
			}
			if( k == cs) return 0;
			
			delete [] ccls;
		}
	}

	pomRoot = 0;
	nizEnt = 0;
	nizEnt1 = 0;

	delete [] root;

	return 0;
}

File* KernelFS::open(char* fname, char mode){ 

	WaitForSingleObject(mutexFile, INFINITE);

	if( mode != 'r' && mode != 'w' && mode != 'a') return 0;

	char tmp[MAX_PATH_LENGTH];
	createApsolutePath(fname, tmp);

	PartInfo* p = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == tmp[0]) {
			p = partArray[i];
			break;
		}
	}

	if(p == 0) return 0;

	File* tempFile = 0;
	KernelFile* ret = 0;

	int nr = 0;

	switch( mode){
		case 'r' : 
			if(doesExist(tmp) == 0) return 0;
			
			ret = listOfAllFiles.uzmi(tmp);
			if(ret == 0) return 0;

			nr = ret->fileMonitor.startRead();
			
			if( nr > 1) {
				KernelFile* novi;
				novi = new KernelFile(ret);
				ret = novi;
				ret->setCurrentPos(0);
				listCopy.naPocetak(tmp, ret);
				novi = 0;
			}
			
			ret->setMode('r');

			tempFile = new File();
			ret->setFile(tempFile);

			ret->seek(0);
			
			break;
		case 'w' : 
			if(doesExist(tmp) == 0){ //ako ne postoji, pravimo ga
				unsigned long cs  = p->getPart()->getClusterSize() * p->getPart()->getBlockSize(),first_cls;
				char pom[MAX_PATH_LENGTH];
				char name[FNAMELEN], ext[FEXTLEN];
				char 
					*fat = new char[KernelFS::FatSizeByte],
					*root = new char[KernelFS::RootSizeByte];

				if (fat == 0) return 0;
				if( root == 0) return 0;

				WaitForSingleObject(mutexFAT, INFINITE);
				if (p->loadRoot(p->getRoot(), root) ==0 ) return 0;
				if (p->loadFat(p->getFat1(), fat) == 0) return 0;
				ReleaseMutex(mutexFAT);

				// root, FAT_first_free_cluster = FF, cluster

				//rezervisemo klaster
				int i;
				for(i = 4; i<KernelFS::FatSizeByte; i+=2){
					if(fat[i] == 0 && fat[i+1]== 0) {
						fat[i] = (char)0xFF;
						fat[i+1] = (char)0xFF;
						break;
					}
				}

				if( inRoot(tmp)){
					//ako pravimo u root- u

					//trazimo prvi slobodan ulaz u root-u
					unsigned int j;
					for(j = 32; j<KernelFS::RootSizeByte; j+= sizeof(Entry) ){
				
						if( root[j + 11] == 0xE5 ||
							root[j + 11] == 0xe5 ||
							root[j + 11] == 0x00 ||
							root[j + 11] == '0') break;
					}

					if (j == KernelFS::RootSizeByte) return 0;

					//pravimo novi Entry za fajl
					//pravimo novi Dir za fajl sa jednim Enrty-em, 
					//posto ne radi konverziju (char* ) Entry
					Directory newDir;
			
					getName(tmp, pom); //celo ime sa ext
					cutName(pom, name, ext);
					strncpy(newDir[0].name, name , FNAMELEN);
					strncpy(newDir[0].ext, ext, FEXTLEN);
					newDir[0].attributes = 0x01;	//uzeo sam zato sto mi je 0x00 oznacava slobodan ulaz
					for (int j=0; j<14; j++) (newDir[0].reserved)[j] = 0;
					newDir[0].size = 0;
					newDir[0].firstCluster = i/2;
					
					//upis u root
					char* nesto = (char* ) newDir;
					for(int ii =0; ii<sizeof(Entry); ii++)root[j + ii] = nesto[ii];
					if (p->storeRoot(p->getRoot(), root) ==0 ) return 0;

					nesto = 0;
					delete [] root;

				}else {
					//ako nije montiranje u root-u

					char *cls  = new char[cs];
					if( cls == 0) return 0;

					char pom1[FNAMELEN + FEXTLEN];

					if ((first_cls = find(tmp, root, cs, p)) == 0) return 0;
					if( p->getPart()->readCluster(first_cls, cls) == 0) return 0;

					delete [] root;

					//trazimo koji je ulaz u klasteru slobodan
					int  ii, ulaz = 2;
					for(ii = 2*sizeof(Entry); ii<cs; ii+= sizeof(Entry), ulaz++ ){
				
						if( cls[ii + 11] == 0xE5 ||
							cls[ii + 11] == 0xe5 ||
							cls[ii + 11] == 0x00 ||
							cls[ii + 11] == '0') break;
					}

					if (ii == cs) return 0;


					Entry newEnt;

					getName(tmp, pom1);
					cutName(pom1, name, ext);
					strncpy(newEnt.name, name , FNAMELEN);
					strncpy(newEnt.ext, ext, FEXTLEN);
					newEnt.attributes = 0x01;	
					for (int j=0; j<14; j++) newEnt.reserved[j] = 0;
					newEnt.size = 0;
					newEnt.firstCluster = i/2;
					

					//upisujemo u klaster novi Entry i vracamo na particiju
					Entry* nizEnt;
					nizEnt = (Entry *) cls;
					nizEnt[ulaz] = newEnt;
					cls = (char *) nizEnt;
		
					if( p->getPart()->writeCluster(first_cls, cls) == 0) return 0;

					delete [] nizEnt;


				}

				//vracanje FAT-a na particiju
				if (p->storeFat(p->getFat1(), fat) == 0) return 0;
				if (p->storeFat(p->getFat2(), fat) == 0) return 0;

				delete [] fat;

				ret = new KernelFile(  p);
				ret->setFirstCls(i/2);
				
				//ubacujemo u listu svih fajlova
				listOfAllFiles.naPocetak(tmp, ret);
			
			}else {// ako fajl postoji

				ret = listOfAllFiles.uzmi(tmp);
				if (ret == 0) return 0;
			}

			ret->fileMonitor.startWrite();

			ret->setMode('w');
			tempFile = new File();
			ret->setFile(tempFile);

			//ret->seek(0);
			//ret->truncate();

			break;
		case 'a' : 
			/* proverimo da li fajl postoji, ako da nadjemo ga u listi,
			i postavimo mu pokazivac na kraj fajla */

			if(doesExist(tmp) == 0) return 0;
			

			ret = listOfAllFiles.uzmi(tmp);
			if( ret == 0) return 0;

			ret->fileMonitor.startApend();

			tempFile = new File();
			ret->setFile(tempFile);

			ret->setMode('a');
			ret->setCurrentPos(ret->getEOF());
						
			break;
		default : return 0;	
	}
	ReleaseMutex(mutexFile);
	return ret->file;
}



char KernelFS::deleteFile(char* fname){ 
	char tmp[MAX_PATH_LENGTH];
	createApsolutePath(fname, tmp);

	if (doesExist(tmp) == 0) return 0;

	KernelFile* ret = 0;
	ret = listOfAllFiles.uzmi(tmp);
	if( ret == 0) return 0;
	if (ret->getMode() != 'c') return 0;


	PartInfo* p = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == tmp[0]) {
			p = partArray[i];
			break;
		}
	}

	if(p == 0) return 0;

	char 
		*fat = new char[KernelFS::FatSizeByte],
		*root = new char[KernelFS::RootSizeByte];

	if (fat == 0) return 0;
	if( root == 0) return 0;

	WaitForSingleObject(mutexFile, INFINITE);

	if (p->loadRoot(p->getRoot(), root) ==0 ) return 0;
	if (p->loadFat(p->getFat1(), fat) == 0) return 0;

	char pom[MAX_PATH_LENGTH];
	getName( tmp, pom);
	char ime[FNAMELEN], ext[FEXTLEN];
	
	cutName(pom, ime, ext);

	if(inRoot(tmp)){
		
		unsigned int j;
		for(j = 32; j<KernelFS::RootSizeByte; j+= sizeof(Entry) ){
			
			if( root[j + 11] == 0x01 &&
				strncmp(root + j, ime, FNAMELEN) == 0) break;
		}

		if (j == KernelFS::RootSizeByte) return 0;

		//brisemo iz root-a i fata
		for( int i = 0; i<sizeof(Entry); i++) root[j + i] = (char) 0;
		
		ClusterNo first = ret->firstCls;
		for(unsigned long i = 4; i<KernelFS::FatSizeByte && 
			first != 0xFFFF; i+=2){
			if(i == first * 2){
				first = ret->getNextCls(first, fat);

				fat[i] = (char) 0x00;
				fat[i + 1] = (char) 0x00;
			}
		}
		
		if (p->storeRoot(p->getRoot(), root) == 0) return 0;

	}else{//ako je u nekom dir- u
		unsigned long cs  = p->getPart()->getClusterSize() * p->getPart()->getBlockSize(),first_cls;
		char *cls = new char[cs];
		if( cls == 0) return 0;

		char pom1[FNAMELEN];

		if ((first_cls = find(tmp, root, cs, p)) == 0) return 0;
		if( p->getPart()->readCluster(first_cls, cls) == 0) return 0;

		//trazimo koji je ulaz u klasteru od fajla
		int  ii, ulaz = 2;
		for(ii = 2*sizeof(Entry); ii<cs; ii+= sizeof(Entry), ulaz++ ){
			
			if( cls[ii + 11] == 0x01 &&
				strncmp( cls + ii, ime, FNAMELEN) == 0) break;
		}

		if (ii == cs) return 0;

		//brisemo iz klastera Entry i vracamo na particiju
		for(int i = 0; i<sizeof(Entry); i++) cls[ii + i] = (char) 0;

		if( p->getPart()->writeCluster(first_cls, cls) == 0) return 0;

		delete [] cls;

		//brisemo iz fata
		ClusterNo first = ret->firstCls;
		for(unsigned long i = 4; i<KernelFS::FatSizeByte && first != 0xFFFF; i+=2){
			if(i == first * 2){
				first = ret->getNextCls(first, fat);

				fat[i] = (char) 0x00;
				fat[i + 1] = (char) 0x00;
			}
		}
	}
	
	if (p->storeFat(p->getFat1(), fat) == 0) return 0;
	if (p->storeFat(p->getFat2(), fat) == 0) return 0;

	//izbacujemo iz liste svih fajlova
	listOfAllFiles.izbaci(tmp);


	delete [] root;
	delete [] fat;

	ReleaseMutex(mutexFile);
	return 1;
}

char KernelFS::createDir(char* dirname){ 
	char tmp[MAX_PATH_LENGTH];
	createApsolutePath(dirname, tmp);

	if (doesExist(tmp) == 1) return 0;

	WaitForSingleObject(mutexDir, INFINITE);

	PartInfo* p = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == tmp[0]) {
			p = partArray[i];
			break;
		}
	}

	if(p == 0) return 0;
	
	unsigned long cs  = p->getPart()->getClusterSize() * p->getPart()->getBlockSize(),first_cls;
	char pom[MAX_PATH_LENGTH];
	char 
		*cls = new char[cs],
		*fat = new char[KernelFS::FatSizeByte],
		*root = new char[KernelFS::RootSizeByte];

	if (fat == 0) return 0;
	if( root == 0) return 0;
	if( cls == 0) return 0;

	WaitForSingleObject(mutexFAT, INFINITE);
	if (p->loadRoot(p->getRoot(), root) ==0 ) return 0;
	if (p->loadFat(p->getFat1(), fat) == 0) return 0;
	ReleaseMutex(mutexFAT);

	// root, FAT_first_free_cluster = FF, cluster
	

	//rezervisemo klaster
	int i;
	for(i = 4; i<KernelFS::FatSizeByte; i+=2){
		if(fat[i] == 0 && fat[i+1]== 0) {
			fat[i] = (char)0xFF;
			fat[i+1] = (char)0xFF;
			break;
		}
	}

	if( inRoot(tmp)){
		//ako je root

		if( p->getPart()->readCluster(i/2, cls) == 0) return 0;

		//trazimo prvi slobodan ulaz u root-u
		unsigned int j;
		for(j = 32; j<KernelFS::RootSizeByte; j+= sizeof(Entry) ){
			
			if( root[j + 11] == 0xE5 ||
				root[j + 11] == 0xe5 ||
				root[j + 11] == 0x00 ||
				root[j + 11] == '0') break;
		}

		if (j == KernelFS::RootSizeByte) return 0;


		Directory newDir;
		
		getName(tmp, pom);
		strncpy(newDir[0].name, pom , FNAMELEN);
		strncpy(newDir[0].ext, "dir", FEXTLEN);
		newDir[0].attributes = 0x10;	
		for (int j=0; j<14; j++) (newDir[0].reserved)[j] = 0;
		newDir[0].size = 0;
		newDir[0].firstCluster = i/2;

		getParentPath(tmp, pom );
		strncpy(newDir[1].name, pom , FNAMELEN);
		strcpy(newDir[1].ext, "\0");
		newDir[1].attributes = 0x08;	
		for (int i=0; i<14; i++) (newDir[1].reserved)[i] = 0;
		newDir[1].size = 0;
		newDir[1].firstCluster = 1; //root_cls = 1
		
	//upisivanje u klaster
		delete [] cls;

		cls = (char * )newDir;
		for(int ii =2* sizeof(Entry); ii<cs; ii++)cls[ii] = (char)0;

	//upis u root
		for(int ii =0; ii<sizeof(Entry); ii++)root[j + ii] = cls[ii];
		if (p->storeRoot(p->getRoot(), root) ==0 ) return 0;
		
		delete [] root;

	}else {
		//ako je poddirektorijum
		char pom1[FNAMELEN];

		if ((first_cls = find(tmp, root, cs, p)) == 0) return 0;
		if( p->getPart()->readCluster(first_cls, cls) == 0) return 0;

		delete [] root;

		//trazimo koji je ulaz u klasteru slobodan
		int  ii, ulaz = 2;
		for(ii = 2*sizeof(Entry); ii<cs; ii+= sizeof(Entry), ulaz++ ){
			
			if( cls[ii + 11] == 0xE5 ||
				cls[ii + 11] == 0xe5 ||
				cls[ii + 11] == 0x00 ||
				cls[ii + 11] == '0') break;
		}

		if (ii == cs) return 0;


		Entry newEnt;

		getName(tmp, pom1);
		strncpy(newEnt.name, pom1 , FNAMELEN);
		strncpy(newEnt.ext, "dir", FEXTLEN);
		newEnt.attributes = 0x10;	
		for (int j=0; j<14; j++) newEnt.reserved[j] = 0;
		newEnt.size = 0;
		newEnt.firstCluster = i/2;

		//upisujemo u klaster novi Entry i vracamo na particiju
		Entry* nizEnt;
		nizEnt = (Entry *) cls;
		nizEnt[ulaz] = newEnt;
		cls = (char *) nizEnt;
	
		if( p->getPart()->writeCluster(first_cls, cls) == 0) return 0;

		delete [] nizEnt;

		//dovlacimo novi klaster za tekuci dir koji pravimo
		if( p->getPart()->readCluster(i/2, cls) == 0) return 0;

		Directory newDir;
		newDir[0] = newEnt;

		getParentPath(tmp, pom );
		getName(pom, pom1);
		strncpy(newDir[1].name, pom1 , FNAMELEN);
		strcpy(newDir[1].ext, "\0");
		newDir[1].attributes = 0x08;	
		for (int i=0; i<14; i++) (newDir[1].reserved)[i] = 0;
		newDir[1].size = 0;
		newDir[1].firstCluster = first_cls;
		
	//upisivanje u klaster
		cls = (char * )newDir;
		for(int ii =2* sizeof(Entry); ii<cs; ii++)cls[ii] = (char)0;

	} // kraj poddirektorijuma

	//vracanje klastera i FAT-a na particiju
	if (p->storeFat(p->getFat1(), fat) == 0) return 0;
	if (p->storeFat(p->getFat2(), fat) == 0) return 0;
	if( p->getPart()->writeCluster(i/2, cls) == 0) return 0;

	delete [] fat;
	cls = 0;
	
	ReleaseMutex(mutexDir);
	return 1;
}

char KernelFS::readDir(char* dirname, EntryNum num, Directory& dir){ 
	char tmp[MAX_PATH_LENGTH];
	createApsolutePath(dirname, tmp);

	if (doesExist(tmp) == 0) return 0;

	PartInfo* p = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == tmp[0]) {
			p = partArray[i];
			break;
		}
	}

	if(p == 0) return 0;

	WaitForSingleObject(mutexDir, INFINITE);

	unsigned long first_cls,
		cs  = p->getPart()->getClusterSize() * p->getPart()->getBlockSize();
	char *cls = new char[cs],
		 *root = new char[KernelFS::RootSizeByte];
	Entry* nizEnt;
		   

	if( root == 0) return 0;
	if( cls == 0) return 0;

	if (p->loadRoot(p->getRoot(), root) ==0 ) return 0;

	int j = 0, //broj procitanih ulaza
		numE = num;

	if( strlen(tmp) == 3 || strlen(tmp) == 2){

		char* pomRoot = &root[32];
		nizEnt = (Entry*) pomRoot;

		Entry ent;
		if(num == 0){//ako treba od pocetka

			//mora ovako jer je sale pogresio oko struktura
			strncpy(ent.name, root, FNAMELEN);
			strncpy(ent.ext, root + FNAMELEN, FEXTLEN);
			ent.attributes = root[FNAMELEN + FEXTLEN];
			for(int i = 0; i<15; i++) ent.reserved[i] = root[FNAMELEN + FEXTLEN + 1 + i];
			ent.firstCluster = 0;
			ent.size = 0;

			dir[j++] = ent;
		}
		
		for(int i = numE; i<ENTRYCNT - 1; i++){
			char name[FNAMELEN]; name[0] = '\0';
			strncpy(name, nizEnt[i].name, FNAMELEN);
			if( name[0] != '\0' && name[0] != ' ' && name[0] != 0 && name[0] != '0')dir[j++] = nizEnt[i];
			else break;
		}

		pomRoot = 0;
	}else {
		//sirimo putanju
		int n = strlen(tmp);
		tmp[n++] = '\\'; tmp[n] = '\0';

		if ((first_cls = find(tmp, root, cs, p)) == 0) return 0;  //klaster pretposlednjeg dir-a
		if( p->getPart()->readCluster(first_cls, cls) == 0) return 0;

		nizEnt = (Entry *) cls;

		for(int i = num; i<ENTRYCNT; i++){
			if (nizEnt[i].attributes != 0x00 )dir[j++] = nizEnt[i];
			else break;
		}
	}

	nizEnt = 0;
	delete[] root;
	delete[] cls;

	ReleaseMutex(mutexDir);
	return j;
}


char KernelFS::deleteDir(char* dirname){ 
	char tmp[MAX_PATH_LENGTH];
	createApsolutePath(dirname, tmp);

	if (doesExist(tmp) == 0) return 0;

	PartInfo* p = 0;
	for(int i = 0; i<26; i++){
		if(partArray[i] != 0 && partArray[i]->getLetter() == tmp[0]) {
			p = partArray[i];
			break;
		}
	}

	if(p == 0) return 0;
	
	unsigned long cs = p->getPart()->getClusterSize() * p->getPart()->getBlockSize(), first_cls;
	char 
		*fat = new char[KernelFS::FatSizeByte],
		*root = new char[KernelFS::RootSizeByte];
		

	if (fat == 0) return 0;
	if( root == 0) return 0;

	WaitForSingleObject(mutexFAT, INFINITE);
	if (p->loadRoot(p->getRoot(), root) ==0 ) return 0;
	if (p->loadFat(p->getFat1(), fat) == 0) return 0;
	ReleaseMutex(mutexFAT);

	char pom[FNAMELEN];
	getName(tmp, pom);

	WaitForSingleObject(mutexDir, INFINITE);

	if( inRoot(tmp)){
		//ako je root
		
		char ime[FNAMELEN];
		char* dst = &root[32];  //VAZNO!!!!!!!!!!
	
		Entry *nizE = (Entry *) dst;

		unsigned int j, ulaz = 0;
		for(j = 32; j<KernelFS::RootSizeByte; j+= sizeof(Entry) , ulaz++){
			
			strcpy(ime , nizE[ulaz].name);

			if (strncmp(ime, pom, FNAMELEN) == 0){
				first_cls = nizE[ulaz].firstCluster;

				for(int ii = 0; ii<sizeof(Entry); ii++) root[j+ ii] = (char) 0;
				
				fat[first_cls * 2] = (char) 0;
				fat[first_cls * 2 + 1] = (char) 0;

				break;
			}
			
		}
		
		if (j == KernelFS::RootSizeByte) return 0;

		if (p->storeRoot(p->getRoot(), root) ==0 ) return 0;

		dst = 0;
		nizE = 0;
				
	}else {
		//ako je poddirektorijum
		Directory myDir;
		if(readDir(tmp,0,myDir) >2 ){
			delete [] fat;
			delete [] root;
			return 0;

		}

		first_cls = find(tmp, root, cs, p);
		if (first_cls == 0) return 0;

		char *cls = new char[cs];
		if( cls == 0) return 0;

		if( p->getPart()->readCluster(first_cls, cls) == 0) return 0;

		//trazimo u kojem je ulazu u klasteru nas dir
		int  ii, ulaz;
		Entry* nizEnt;
		nizEnt = (Entry *)cls;

		for(ii = 2*sizeof(Entry), ulaz = 2; ii<cs; ii+= sizeof(Entry), ulaz++ ){
			char pom1[FNAMELEN];
			for(int k = 0; k<FNAMELEN; k++) pom1[k] = cls[ii + k];
			if(strncmp(pom1, pom, FNAMELEN) == 0){
				unsigned int first_cls_pom = nizEnt[ulaz].firstCluster;
				
				//oslobadjamo u FAT- u redni broj klastera
				fat[first_cls_pom * 2] = (char) 0;
				fat[first_cls_pom * 2 + 1] = (char) 0;

				//brisemo Entry iz roditeljskog klastera
				for(int i =0; i<FNAMELEN; i++)nizEnt[ulaz].name[i] = (char) 0;
				for(int i =0; i<FEXTLEN; i++)nizEnt[ulaz].ext[i] = (char) 0;
				nizEnt[ulaz].attributes = 0xe5;	
				for (int i=0; i<14; i++) (nizEnt[ulaz].reserved)[i] = 0;
				nizEnt[ulaz].size = 0;
				nizEnt[ulaz].firstCluster = 0;

				//vracamo roditeljski klaster na particiju
				cls = (char * )nizEnt;
				if( p->getPart()->writeCluster(first_cls, cls) == 0) return 0;

				break;
			}
			
		}

		delete nizEnt;//ovim brisemo i cls

		if (ii == cs) return 0;

	}

	//vracanje FAT-a na particiju
	if (p->storeFat(p->getFat1(), fat) == 0) return 0;
	if (p->storeFat(p->getFat2(), fat) == 0) return 0;

	delete [] fat;
	delete [] root;
	
	ReleaseMutex(mutexDir);
	return 1;
}
 
char KernelFS::cd(char* dirname){ 
	char tmp[MAX_PATH_LENGTH];

	createApsolutePath(dirname, tmp);

	char slovo = tmp[0];
	if ( partLetter[slovo - 'A'] == 0) return 0; //ako ne postoji

	if (strlen(tmp) == 3){  //ako je root
		strcpy(pwdName, tmp);
		return 1;
	}

	for(int i = 0; i<26; i++)
	{
		if(partArray[i] != 0 && partArray[i]->getLetter() == slovo)
		{
			if( doesExist(tmp) == 0 ) return 0;
			strcpy(pwdName, tmp);
			return 1;
		}
	}

	return 0;
}


//KREIRANJE APSOLUTNE PUTANJE FAJLA
void KernelFS::createApsolutePath(char *src, char *result){
	if( src[1] == ':'){
		int n = strlen(src);
		result[0] = toupper(src[0]);
		if(src[n] == '\\') src[n] = '\0';
		for(int i = 1; i<n || src[i] == '\0'; i++) result[i] = src[i];
		return ;
	}

	if(src[0] == '.' && src[1]== '\\'){
		strcpy(result , pwdName);

		int pwdLen = strlen(result);
		if(result[pwdLen] != '\\') result[pwdLen] = '\\';
		strcpy(result + pwdLen + 1, src + 2);
		return;
	
	}

	if(src[0] == '.' && src[1] == '.'){
		int br = 0, j =0, n;
		char pom[MAX_PATH_LENGTH], name[MAX_PATH_LENGTH];
		strcpy( pom, src);
		while(pom[0] == '.' && pom[1] == '.'){
			strcpy(pom, pom + 3); 
			br++;
		}
		strcpy( name,pom);

		while(br > 0){
			strcpy(pom , strrchr(pwdName, '\\'));
			j+=strlen(pom);
			n = strlen(pwdName) - strlen(pom);
			strncpy(pom, pwdName, n);
			pom[n] = '\0';
			
			br--;
		}
		
		int pwdLen = strlen(pom);
		if(pom[pwdLen] != '\\') {pom[pwdLen++] = '\\'; pom[pwdLen] = '\0'; }
		strcat(pom, name);

		strcpy(result, pom);
		
		return;
	}

	if(src[0]!= '.' && src[1]!= ':' && src[1]!= '\\' && src[1]!= '.'){
		char ok[MAX_PATH_LENGTH];
		strcpy(ok, pwdName);
		int n = strlen(ok);
		if(n != 3){strcat(ok, "\\");}
		
		strcat(ok, src);

		strcpy(result, ok);
		
		return;
	}

}

//DOHVATANJE PUTANJE RODITELJA
void KernelFS::getParentPath(char* src, char* result){
	char tmp[MAX_PATH_LENGTH];
	strcpy(tmp, src);
	int n = strlen(tmp);
	for(int i = n-1; i>0; i--)
		if(tmp[i] == '\\'){
			tmp[i] = '\0';
			break;
		}
	
	n = strlen(tmp);
	for(int i = 0; tmp[i]!= '\0'; i++) result[i] = tmp[i];
	//strcpy(result, tmp);
	result[n] = '\0';

}

//DOHVATANJE SAMO IMENA DIREKTORIJUMA
void KernelFS::getName(char *src, char *result){
	char pom[MAX_PATH_LENGTH];
	strcpy(pom , strrchr(src, '\\'));
	strcpy(result, pom + 1);
}

//ISPITUJE DA LI TREBA DA SE MONTIRA U ROOT
bool KernelFS::inRoot(char *src){
	int br = 0;
	for(int i =0; i< strlen(src); i++)
		if (src[i] == '\\') br++;
	return br == 1;

}
//BROJANJE KOLIKO IMA DIREKTORIJUMA U PUTANJI
int KernelFS::countNames(char* path){
	int br = 0, i = 0;
	while(path[i] != '\0'){
		if(path[i++] == '\\') br++;
	}
	return br;
}
//VRACANJE KLASTERA POSLEDNJE MONTIRANOG DIR-A
unsigned int KernelFS::find (char* path, char* root, ClusterNo cs, PartInfo* part){
	
	char * pomRoot = &root[32];
	Entry* nizEnt, *nizEnt1;


	char pom[MAX_PATH_LENGTH], pom1[FNAMELEN];
	unsigned int first_cls = 0;
	Partition* p = part->getPart();
	int  j = 3;

	nizEnt = (Entry *) pomRoot;

	for( int i = 0; i< countNames(path)- 1; i++){
		if(i == 0) {
			int k= 0 , ulaz = 0;
			while(path[j] != '\\'){
				pom[k++] = path[j++];
			}
			pom[k] = '\0';

			for( k = 32; k<KernelFS::RootSizeByte; k+=sizeof(Entry), ulaz++){
				strncpy(pom1, root + k, FNAMELEN);
				if ( strcmp(pom, pom1) == 0) {
					first_cls = nizEnt[ulaz].firstCluster;

					break;
				}
			}
			if( k == KernelFS::RootSizeByte) return 0;
		} else{

			char *ccls  = new char[cs];
			if (ccls == 0) return 0;
			

			if( p->readCluster(first_cls, ccls) == 0) return 0;
			if (ccls == 0) return 0;
			nizEnt1 = (Entry *) ccls;

			int k= 0, ulaz = 2;
			j++;
			while(path[j] != '\\'){
				pom[k++] = path[j++];
			}
			pom[k] = '\0';

			for( k = 2*sizeof(Entry); k<cs; k+=sizeof(Entry), ulaz++){
				strncpy(pom1, ccls + k, FNAMELEN);
				if ( strcmp(pom, pom1) == 0) {
					first_cls = nizEnt1[ulaz].firstCluster;

					break;
				}
			}
			if( k == cs) return 0;
			
			delete [] ccls;
		}
	}

	pomRoot = 0;
	nizEnt = 0;
	nizEnt1 = 0;

	return first_cls;

}

//CITANJE ENTRY-A IZ KLASTERA
int KernelFS::readEntry(char *cls, int num, Entry &ent){
	Entry* pomEnt = (Entry *) cls;
	
	ent = pomEnt[num];

	return 1;
}

//SECKANJE NA IME I EXT
void KernelFS::cutName(char *src, char *name, char *ext){
	int i = 0 ,k = 0;
	while(src[i]!= '.' && i<FNAMELEN){
		name[i] = src[i];
		i++;
	}
	name[i++] = '\0';

	while(k<FEXTLEN){
		ext[k++] = src[i++];
	}
	if(!(k == FEXTLEN)) ext[k] = '\0';
}

//KREIRANJE APSOLUTNE PUTANJE FAJLA
void KernelFS::createApsoluteFileName(char *src, char *result){
	if( src[1] == ':'){
		int n = strlen(src);
		result[0] = toupper(src[0]);
		if(src[n] == '\\') src[n] = '\0';
		for(int i = 1; i<n || src[i] == '\0'; i++) result[i] = src[i];
		return ;
	}

	if(src[0] == '.' && src[1]== '\\'){
		strcpy(result , pwdName);

		int pwdLen = strlen(result);
		if(result[pwdLen] != '\\') result[pwdLen] = '\\';
		strcpy(result + pwdLen + 1, src + 2);
		return;
	
	}

	if(src[0] == '.' && src[1] == '.'){
		int br = 0, j =0, n;
		char pom[100];
		while(src[0] == '.' && src[1] == '.'){
			strcpy(pom, src + 3);
			strcpy(src, pom);
			br++;
		}

		while(br > 0){
			strcpy(pom , strrchr(pwdName, '\\'));
			j+=strlen(pom);
			n = strlen(pwdName) - strlen(pom);
			strncpy(pom, pwdName, n);
			pom[n] = '\0';
			strcpy(pwdName, pom);
			
			br--;
		}
		strncat(result, pwdName, strlen(pwdName) - j);
		int pwdLen = strlen(result);
		if(result[pwdLen] != '\\') result[pwdLen] = '\\';
		strcat(result + pwdLen + 1, src);
		
		return;
	}

	if(src[0]!= '.' && src[1]!= ':' && src[1]!= '\\' && src[1]!= '.'){
		char ok[MAX_PATH_LENGTH];
		strcpy(ok, pwdName);
		int pwdLen = strlen(ok);
		if(ok[pwdLen - 1] != '\\') ok[pwdLen] = '\\';
		strcat(ok, src);

		strcpy(result, ok);
		
		return;
	}
}
