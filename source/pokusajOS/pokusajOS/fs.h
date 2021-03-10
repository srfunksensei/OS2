// File: fs.h

#ifndef _FS_H_
#define _FS_H_

typedef unsigned long BytesCnt; 
typedef unsigned long EntryNum;

const unsigned int ENTRYCNT=64; 
const unsigned int FNAMELEN=8;
const unsigned int FEXTLEN=3;

struct Entry {
  char name[FNAMELEN];
  char ext[FEXTLEN];
  char attributes;
  char reserved[14];
  unsigned long size;
  unsigned int firstCluster;
};

typedef Entry Directory[ENTRYCNT];

class KernelFS;
class Partition;
class File;
class FS {
public:

  FS ();
 ~FS ();

  static char mount(Partition* partition); //montira particiju
                                     // vraca dodeljeno slovo
  static char unmount(char part);  //demontira particiju oznacenu datim
                // slovom vraca 0 u slucaju neuspeha i 1 u slucaju uspeha
  static char format(char part); //particija zadatu slovom se formatira sa
                // FAT16; vraca 0 u slucaju neuspeha i 1 u slucaju uspeha

  static BytesCnt freeSpace(char part); // vraca ukupan broj bajtova u
                        // slobodnim klasterima particije sa zadatim slovom
  static BytesCnt partitionSize(char part); // vraca ukupan broj bajtova
        //koji se koriste za smestanje podataka particije sa zadatim slovom

  char doesExist(char* fname); //argument je puna staza fajla
  File* open(char* fname, char mode);
  char deleteFile(char* fname);

  char createDir(char* dirname);
  char readDir(char* dirname, EntryNum, Directory &); 
         //drugim argumentom se zadaje broj ulaza od kog se pocinje citanje
  char deleteDir(char* dirname);
  char* pwd();               //tekuci direktorijum
  char  cd(char* dirname);   //promena tekuceg direktorijuma
private:
  KernelFS *myImpl;
}; 

#endif