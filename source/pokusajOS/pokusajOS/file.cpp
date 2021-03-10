// file: file.cpp

#include "file.h"
#include "kernelfile.h"

File::File(){
	myImpl = 0;
}

File::~File(){
	//if(myImpl != 0) myImpl = 0;
}

char File::write (BytesCnt cnt, char* buffer){
	return myImpl->write(cnt, buffer);
}
BytesCnt File::read (BytesCnt cnt, char* buffer){
	return myImpl->read(cnt, buffer);
}
char File::seek (BytesCnt cnt){
	return myImpl->seek(cnt);
}
BytesCnt File::getFileSize (){
	return myImpl->getFileSize();
}
char File::eof (){
	return myImpl->eof();
}
char File::truncate (){
	return myImpl->truncate();
}
void File::close (){
	myImpl->close();
}