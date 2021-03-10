//file: kernelfile.h

#ifndef _KERNEL_FILE_H_
#define _KERNEL_FILE_H_

#include "file.h"
#include "PartInfo.h"
#include "Monitor.h"
#include <iostream>
using namespace std;

class Monitor;

class KernelFile {
public:

	KernelFile ( PartInfo* pi);
  ~KernelFile();
  KernelFile(const KernelFile* kf);

  char write (BytesCnt, char* buffer); 
  BytesCnt read (BytesCnt, char* buffer);
  char seek (BytesCnt);
  BytesCnt getFileSize ();
  char eof ();
  char truncate ();
  void close (); 

  /* metoda koja postavlja prvi klaster fajla */
  void setFirstCls(ClusterNo );

  /* metoda koja postavlja trenutnu posziciju kursora u fajlu */
  int setCurrentPos(BytesCnt);

  /* metoda koja vraca EOF */
  BytesCnt getEOF();

  /* metoda koja daje sledeci broj klastera */
  ClusterNo getNextCls(ClusterNo , char* );

  /* metoda koja postavlja fajl za koji je vezan */
  void setFile(File* );

  /* metoda koja vraca i postavlja mod */
  char getMode();
  void setMode(char);

private:
  friend class FS;
  friend class KernelFS;

  File* file;
  PartInfo* p;

  char* cls; //klaster u kome nam je kursor

  char mode; // r, w, a

  BytesCnt
	  currentPos, //tekuca pozicija
	  endOfFile; //kraj fajla (velicina fajla)

  ClusterNo firstCls; //prvi klaster fajla
  ClusterSize clsSize; //velicina klastera
	  
  int inCls, // u kom smo klasteru trenutno
	numOfCls; //na koliko se klastera prostire fajl

  Monitor fileMonitor;
  HANDLE mutex;

  bool copy;
};

#endif