
#include "fs.h"
#include "kernelfs.h"

FS::FS(){
	myImpl = new KernelFS();
}

FS::~FS(){
	delete myImpl;
}

char FS::mount(Partition* partition){
	return KernelFS::mount(partition);
}

char FS::unmount(char part){
	return KernelFS::unmount(part);
}

char FS::format(char part){
	return KernelFS::format(part);
}

BytesCnt FS::freeSpace(char part){
	return KernelFS::freeSpace(part);
}

BytesCnt FS::partitionSize(char part){
	return KernelFS::partitionSize(part);
}


char FS::doesExist(char* fname){
	return myImpl->doesExist(fname);
}

File* FS::open(char* fname, char mode){
	return myImpl->open(fname, mode);
}

char FS::deleteFile(char* fname){
	return myImpl->deleteFile(fname);
}

char FS::createDir(char* dirname){
	return myImpl->createDir(dirname);
}

char FS::readDir(char* dirname, EntryNum num, Directory &dir){
	return myImpl->readDir(dirname, num, dir);
}

char FS::deleteDir(char* dirname){
	return myImpl->deleteDir(dirname);
}

char* FS::pwd(){
	return myImpl->pwd();
}

char FS::cd(char* dirname){
	return myImpl->cd(dirname);
}