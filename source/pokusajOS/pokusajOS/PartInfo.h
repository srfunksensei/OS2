// file: PartInfo.h

#ifndef _PART_INFO_H_
#define _PART_INFO_H_

#include "part.h"

class PartInfo{
public:
	PartInfo(Partition* p);
	~PartInfo();

	void setLetter (char c);
	char getLetter () const;

	void setFat1 (BlockNo );
	BlockNo getFat1 () const;

	void setFat2 (BlockNo );
	BlockNo getFat2 () const;

	void setRoot(BlockNo);
	BlockNo getRoot() const;

	Partition* getPart() const;

	int loadFat(BlockNo, char* fat);
	int storeFat(BlockNo, const char* fat);

	int loadRoot(BlockNo, char* root);
	int storeRoot(BlockNo, const char* root);

private:
	Partition* part;
	char letter;

	BlockNo fat1_start, fat2_start;
	BlockNo root_start;
	ClusterNo frst_free_cluster;

};

#endif