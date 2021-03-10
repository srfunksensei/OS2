// file: lista.cpp

//#include "kernelfile.h"
#include "lista.h"
#include "kernelfs.h"
#include <iostream>
using namespace std;


void Lista::brisi () {                          // Praznjenje liste.
  while (prvi) { Elem* stari = prvi; prvi = prvi->sled; delete stari; }
  posl = 0; duz = 0;
}

void Lista::naPocetak(char* ime, KernelFile* kf){
	prvi = new Elem (ime, kf, prvi);  duz++;
	if(prvi == 0) posl = prvi;
}

KernelFile* Lista::uzmi (char* i) {
  Elem *tek = prvi, *pret = 0;
  if(prvi == 0) return 0;

  while (tek)
	  if (strncmp(i , tek->ime, MAX_PATH_LENGTH) != 0) { 
      pret = tek; tek = tek->sled;
    } else {
		return tek->kf;
	  } 
  return 0;
}

KernelFile* Lista::uzmiSaKraja () {
	Elem *tek = prvi;
	if(prvi == 0) return 0;

	KernelFile* kf = 0;
	int br = duz - 1;
	if(br > 0){
		while(br>0){
			tek = tek->sled;
			br--;
		}
		kf = posl->kf;
		posl = tek; posl->sled = 0;
		duz--;
	} else if (br == 0){
		kf = prvi->kf;
		posl = prvi = 0;
		duz = 0;
	}
	
	return kf;
}

int Lista::izbaci (char* i) {
  Elem *tek = prvi, *pret = 0;
  while (tek)
	  if (strncmp(i , tek->ime, MAX_PATH_LENGTH) != 0) { 
      pret = tek; tek = tek->sled;
    } else {
      Elem *stari = tek;
      tek = tek->sled;
      if (!pret) prvi = tek; else pret->sled = tek;
	  if(tek == 0) posl = pret; //if(stari == posl)
      delete stari;
	  duz--;
	  return 1;
    } 
	return 0;
}

ostream& operator<< (ostream& it, const Lista& lst) { // Pisanje.
  it << '(';
  for (Lista::Elem* tek=lst.prvi; tek; tek=tek->sled)
  { it << tek->ime; it << ','; it<< tek->kf;
    if (tek->sled) it << ',';  it<<'\n';}
  return it << ')';
}

bool Lista::imaOtvorenih(){
	Elem* tek = prvi;
	while(tek->sled){
		if(tek->kf->getMode() != 'c') return true;
		tek = tek->sled;
	}
	return false;
}