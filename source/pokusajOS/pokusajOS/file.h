// File: file.h

#ifndef _FILE_H_
#define _FILE_H_

#include "fs.h"

class KernelFile;

class File {
public:

  ~File();

  char write (BytesCnt, char* buffer); 
  BytesCnt read (BytesCnt, char* buffer);
  char seek (BytesCnt);
  BytesCnt getFileSize ();
  char eof ();
  char truncate ();
  void close ();  

private:
  friend class FS;
  friend class KernelFS;
  friend class KernelFile;
  File ();  //objekat fajla se moze kreirati samo otvaranjem

  KernelFile *myImpl;
};

#endif