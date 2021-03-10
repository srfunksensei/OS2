//test.cpp

#include <iostream>
#include "kernelfs.h"
#include "part.h"
#include "lista.h"
#include "File.h"
using namespace std;

int main(){
/*	Partition* p = new Partition ("p1.ini");

	cout<<KernelFS::mount(p)<<'\n';
	
	cout<<KernelFS::format('A')<<'\n';

	cout<<KernelFS::freeSpace('A')<<'\n';

	cout<<KernelFS::partitionSize('A')<<'\n';

	cout<<KernelFS::unmount('A')<<'\n';
	
*/
/*	
	char src[] = "C:\\milan\\os\\nesto";
	char result[MAX_PATH_LENGTH];
	char tmp[MAX_PATH_LENGTH];
	strcpy(tmp, src);
	int n = strlen(tmp);
	for(int i = n-1; i>0; i--)
		if(tmp[i] == '\\'){
			tmp[i] = '\0';
			break;
		}
	
	n = strlen(tmp);
	for(int i = 0; i<n && tmp[i]!= '\0'; i++) result[i] = tmp[i];
	result[n] = '\0';


	for(int i = 0; i<strlen(result); i++) cout<<result[i];
*/
/*
	KernelFS kfs;

	Partition* p = new Partition ("p1.ini");

	cout<<kfs.mount(p)<<'\n';
	cout<<kfs.format('A')<<'\n';
	cout<<kfs.createDir("A:\\dir1")<<'\n';
	cout<<kfs.createDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.unmount('A')<<'\n';
*/

/*	char ime[FNAMELEN] = {'\0'};
	char* src = "A:\\dir1\\dir2";

	int br = 0, i = 3;
	while(src[i] != '\\' && src[i]!= '\0'){
		ime[br] = src[i++];
		br++;
	}
	ime[br] = '\0';

	for(int i = 0; i<FNAMELEN;i++) cout<<ime[i];
*/

	
/*
	KernelFS kfs;
	Directory dir;

	Partition* p = new Partition ("p1.ini");

	cout<<kfs.mount(p)<<'\n';
	cout<<kfs.format('A')<<'\n';
	cout<<kfs.createDir("A:\\dir1")<<'\n';
	cout<<kfs.createDir("A:\\dir2")<<'\n';
	cout<<kfs.createDir("A:\\dir3")<<'\n';
	cout<<kfs.createDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.createDir("A:\\dir1\\dir2\\dir3")<<'\n';
	cout<<kfs.readDir("A:\\dir1", 0, dir)<<'\n';
/*	cout<<
		kfs.createDir("A:\\dir1\\dir2\\dir3")
		<<'\n';
/*	cout<<kfs.deleteDir("C:\\proba")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1\\dir2")<<'\n';
/	cout<<kfs.deleteDir("A:\\dir1\\dir2\\dir3")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1")<<'\n';
	cout<<kfs.deleteDir("A:\\dir2")<<'\n';
	cout<<kfs.deleteDir("A:\\dir3")<<'\n';
	
	cout<<kfs.unmount('A')<<'\n';
*/
	
	
/*
	Lista l;
	cout<<l.add("C:\\MILAN\\NESTO")<<'\n';
	cout<<l.add("C:\\MILAN\\NESTO1")<<'\n';
	cout<<l.add("C:\\MILAN\\NESTO2")<<'\n';
	cout<<l.add("C:\\MILAN\\NESTO3")<<'\n';

	cout<<l.doesExist("C:\\MILAN\\NESTO2")<<'\n';

	cout<<l.remove("C:\\MILAN\\NESTO")<<'\n';
	cout<<l.doesExist("C:\\MILAN\\NESTO")<<'\n';

	cout<<l.remove("C:\\MILAN\\NESTO2")<<'\n';
	cout<<l.doesExist("C:\\MILAN\\NESTO2")<<'\n';

	l.freeList();
*/
	cout<<"pocetak\n";
/*
	KernelFS kfs;
	Directory dir;

	Partition* p = new Partition ("p1.ini");

	cout<<kfs.mount(p)<<'\n';
	cout<<kfs.format('A')<<'\n';
	cout<<kfs.createDir("A:\\dir1")<<'\n';
	cout<<kfs.createDir("A:\\dir2")<<'\n';
	cout<<kfs.createDir("A:\\dir3")<<'\n';
	cout<<kfs.createDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.createDir("A:\\dir1\\dir2\\dir3")<<'\n';
	cout<<kfs.readDir("A:\\dir1", 0, dir)<<'\n';
	cout<<
		kfs.createDir("A:\\dir1\\dir2\\dir3")
		<<'\n';
	cout<<kfs.deleteDir("C:\\proba")<<'\n';
//	cout<<kfs.deleteDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1\\dir2\\dir3")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1\\dir2")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1")<<'\n';
	cout<<kfs.deleteDir("A:\\dir2")<<'\n';
	cout<<kfs.deleteDir("A:\\dir3")<<'\n';
	
	cout<<kfs.unmount('A')<<'\n';
*/
/*
	KernelFS kfs;
	File* file = 0, *file1 = 0;

	Partition* p = new Partition ("p1.ini");

	cout<<kfs.mount(p)<<'\n';
	cout<<kfs.format('A')<<'\n';
	file = kfs.open("A:\\fajl.txt", 'w');
	cout<<file<<'\n';
	cout<<kfs.createDir("A:\\dir1")<<'\n';
	file1 = kfs.open("A:\\dir1\\fajl.txt", 'w');
	cout<<file1<<'\n';
	


	char *buff = new char[2000];
	for(int i = 0; i<2000; i++){ 
		buff[i] = ('A' + (i% 70)) ;
		cout<<buff[i];
	}
	cout<<'\n';

	cout<<file->write(2000, buff)<<'\n';
	cout<<file->seek(0)<<'\n';
	cout<<file->read(60, buff)<<'\n';
	for(int i =0; i<60; i++) cout<<buff[i];
	cout<<'\n';

	cout<<file->getFileSize()<<'\n';

	cout<<file->seek(1960)<<'\n';
	cout<<file->read(100, buff)<<'\n';
	cout<<file->eof()<<'\n';

	for(int i =0; i<40; i++) cout<<buff[i];
	cout<<'\n';

	cout<<file->seek(1000)<<'\n';
	cout<<file->truncate()<<'\n';
	cout<<file->eof()<<'\n';
	cout<<file->getFileSize()<<'\n';

	cout<<file->seek(1900)<<'\n';
	cout<<file->truncate()<<'\n';
	cout<<file->eof()<<'\n';
	cout<<file->getFileSize()<<'\n';

//	cout<<kfs.open("A:\\fajl.txt", 'w')<<'\n';

//	cout<<kfs.open("A:\\fajl.txt", 'r')<<'\n';
//	cout<<kfs.open("A:\\fajl.txt", 'a')<<'\n';

	file->close();
	cout<<kfs.deleteFile("A:\\fajl.txt")<<'\n';
	file1->close();
	cout<<kfs.deleteFile("A:\\dir1\\fajl.txt")<<'\n';
	cout<<kfs.deleteDir("A:\\dir1")<<'\n';
*/	
	cout<<"\nkraj"<<'\n';
	return 0;
}