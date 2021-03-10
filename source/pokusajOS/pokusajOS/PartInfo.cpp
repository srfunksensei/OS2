// file: PartInfo.cpp

#include "PartInfo.h"
#include "kernelfs.h"
#include "part.h"

PartInfo::PartInfo(Partition* p){
		this->part = p;
}

PartInfo::~PartInfo(){ 
	delete part;
}

void PartInfo::setLetter(char c){
	this->letter = c;
}

char PartInfo::getLetter() const{
	return letter;
}

void PartInfo::setFat1(BlockNo n){
	this->fat1_start = n;
}

void PartInfo::setFat2(BlockNo n){
	this->fat2_start = n;
}

BlockNo PartInfo::getFat1() const{
	return fat1_start;
}

BlockNo PartInfo::getFat2() const{
	return fat2_start;
}

Partition* PartInfo::getPart() const{
	return part;
}

BlockNo PartInfo::getRoot() const{
	return root_start;
}

void PartInfo::setRoot(BlockNo start){
	root_start = start;
}

int PartInfo::loadFat(BlockNo start, char* fat){
	Partition * p = this->getPart();
		BlockSize blockSize = p->getBlockSize();

		int numOfBlocks1 = KernelFS::FatSizeByte / blockSize;
		if (KernelFS::FatSizeByte % blockSize) numOfBlocks1++;

		for(int j = 0; j<numOfBlocks1; j++)
			if(p->readBlock( start + j , fat + j*blockSize) == 0) return 0;

		return 1;
}

int PartInfo::storeFat(BlockNo start, const char* fat){
	Partition * p = this->getPart();
	BlockSize blockSize = p->getBlockSize();

	int numOfBlocks1 = KernelFS::FatSizeByte / blockSize;
	if (KernelFS::FatSizeByte % blockSize) numOfBlocks1++;
	
	for(int j = 0; j<numOfBlocks1; j++)
			if(p->writeBlock( start + j , fat + j*blockSize) == 0) return 0;

	return 1;
}

int PartInfo::loadRoot(BlockNo start, char* root){
	Partition * p = this->getPart();

		BlockSize blockSize = p->getBlockSize();

		int numOfBlocks1 = KernelFS::RootSizeByte / blockSize;
		if (KernelFS::RootSizeByte % blockSize) numOfBlocks1++;

		for(int j = 0; j<numOfBlocks1; j++)
			if(p->readBlock( start + j, root + j*blockSize)== 0) return 0;

		return 1;
}

int PartInfo::storeRoot(BlockNo start, const char* root){
	Partition * p = this->getPart();
	BlockSize blockSize = p->getBlockSize();

	int numOfBlocks1 = KernelFS::RootSizeByte / blockSize;
	if (KernelFS::RootSizeByte % blockSize) numOfBlocks1++;
	
	for(int j = 0; j<numOfBlocks1; j++)
		if(p->writeBlock( start + j , root + j*blockSize) == 0) {
			return 0;
		}
	return 1;
}
