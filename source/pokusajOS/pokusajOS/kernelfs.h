// file: KernelFS.h

#ifndef _KERNEL_FS_H_
#define _KERNEL_FS_H_

#include "file.h"
#include "fs.h"
#include "kernelfile.h"
#include "kernelfs.h"
#include "lista.h"
#include "part.h"
#include "PartInfo.h"
#include "Monitor.h"
#include <windows.h>
#include <iostream>
using namespace std;

#define MAX_PATH_LENGTH 100

class KernelFS {
	
	static PartInfo* partArray[26];
	static int partLetter[26]; //koje je slovo zauzeto
	
	static unsigned long BootSectorSizeByte;
	static unsigned long FatSizeByte;
	static unsigned long RootSizeByte;

	char* pwdName;

	static Lista listOfAllFiles;// lista svih fajlova
	static Lista listCopy;

	friend class PartInfo;
	friend class KernelFile;
	friend class Monitor;

	static HANDLE mutexFile, mutexDir; //za medjusobno iskljucenje
	static HANDLE mutexFAT, mutex;
	static HANDLE mutexL;

public:
  KernelFS ();
  ~KernelFS ();

  static char mount(Partition* partition); //montira particiju
                                     // vraca dodeljeno slovo
  static char unmount(char part);  //demontira particiju oznacenu datim
                // slovom vraca 0 u slucaju neuspeha i 1 u slucaju uspeha
  static char format(char partL); //particija zadatu slovom se formatira sa
                // FAT16; vraca 0 u slucaju neuspeha i 1 u slucaju uspeha

  static BytesCnt freeSpace(char part); // vraca ukupan broj bajtova u
                        // slobodnim klasterima particije sa zadatim slovom
  static BytesCnt partitionSize(char part); // vraca ukupan broj bajtova
        //koji se koriste za smestanje podataka particije sa zadatim slovom

  char doesExist(char* fname); //argument je puna staza fajla
  File* open(char* fname, char mode);
  char deleteFile(char* fname);

  char createDir(char* dirname);
  char readDir(char* dirname, EntryNum, Directory&); 
         //drugim argumentom se zadaje broj ulaza od kog se pocinje citanje
  char deleteDir(char* dirname);
  char* pwd();               //tekuci direktorijum
  char  cd(char* dirname);   //promena tekuceg direktorijuma



  /* metoda koja pravi apsolutnu putanju od relativne */
	void createApsolutePath (char* src, char* result);

	/* metoda koja dohvata roditeljsku putanju */
	void getParentPath(char* src, char* result);

	/*metoda koja dohvata samo ime direktorijuma */
	void getName(char* src, char* result);

	/* metoda koja ispituje da li treba da se montira u root */
	bool inRoot(char* src);
	
	/* metoda koja broji koliko ima kosih crta u putanji */
	int countNames(char* path);

	/* metoda koja vraca klaster poslednjeg montiranog dir-a  */
	unsigned int find( char* path, char* root, ClusterNo cs, PartInfo* part);

	/* metoda koja cita jedan Entry iz klastera */
	int readEntry (char* cls, int num, Entry& ent);

	/* metoda koja secka na ime fajla i extenziju */
	void cutName(char* src, char* name, char* ext);

	/* metoda koja pravi apsolutnu putanju fajla */
	void createApsoluteFileName (char* src, char* result);

};

#endif