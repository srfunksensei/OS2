
#ifndef _PART_H_
#define _PART_H_


typedef unsigned long BlockSize;
typedef unsigned long BlockNo;
typedef unsigned long ClusterNo;
typedef unsigned long ClusterSize;

class PartitionImpl;

class Partition {
public:
	Partition(char *);
	virtual BlockSize getBlockSize() const; //vraca velicinu bloka u bajtovima
	virtual BlockNo getStartBlock() const; //vraca redni broj prvog bloka koji pripada particiji
	virtual BlockNo getNumOfBlocks() const; //vraca broj blokova koji pripadaju particiji

	virtual long getNumOfSectors() const; //vraca broj sektora na jednom cilindru jedne povrsi
	virtual long getNumOfCylinders() const; //vraca broj cilindara
	virtual long getNumOfHead() const; //vraca broj glava

	virtual int readBlock(BlockNo, char *buffer); //cita zadati blok i u slucaju uspjeha vraca 1; u suprotnom 0
	virtual int writeBlock(BlockNo, const char *buffer); //upisuje zadati blok i u slucaju uspjeha vraca 1; u suprotnom 0

	virtual ClusterSize getClusterSize() const; //vraca tekucu velicinu klastera u blokovima
	virtual int setClusterSize(BlockNo); //postavlja velicinu klastera u blokovima
	
	virtual int setClusterOffset(BlockNo); //postavlja logicki broj prvog bloka klastera broj 2 (prvog klastera koji se koristi)
	virtual BlockNo getClusterOffset() const; //vraca logicki broj bloka kojim pocinje klaster 2

	virtual int readCluster(ClusterNo, char *buffer); //cita zadati klaster i u slucaju uspjeha vraca 1; u suprotnom 0
	virtual int writeCluster(ClusterNo, const char *buffer); //upisuje zadati klaster i u slucaju uspjeha vraca 1; u suprotnom 0

	virtual ~Partition();
private:
	PartitionImpl *myImpl;
};

#endif