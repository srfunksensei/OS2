// file : Lista.h

#ifndef _LISTA_H_
#define _LISTA_H_

#include <iostream>
using namespace std;

#define DUZINA 50

class KernelFile;

class Lista {
protected:
  struct Elem {                // ELEMENT LISTE:
    char* ime; 
	KernelFile* kf;
	Elem* sled;      // - sadrzaj i pokazivac na sledeci element,
	Elem (char* i, KernelFile* k = 0, Elem* s=0)    // - konstruktor.
      {		ime = new char[DUZINA];
		  strcpy( ime, i); kf = k; sled = s;
	}
  };
  Elem *prvi, *posl;           // Prvi i poslednji element liste.
  int duz;                     // Duzina liste.
private:
  void brisi ();                      // Praznjenje liste.
public:                                              // Konstruktori:
  Lista () { prvi = posl = 0; duz = 0; }            // - prazna lista,
  Lista (char* i, KernelFile* kf) { prvi = posl = new Elem (i, kf); duz = 1; } // - konverzija,
  ~Lista () { brisi (); }                           // Unistavanje.
  
  int duzina () const { return duz; }                // Duzina liste.
  void naPocetak (char* ime, KernelFile* kf);        // Dodavanje na pocetak.
  KernelFile* uzmi (char* ime);        // Uzimanje elementa zi liste
  KernelFile* uzmiSaKraja();	//za FIFO
  int izbaci(char* );

  bool imaOtvorenih();	//proverava da li ima otvorenih fajlova

  friend ostream& operator<< (ostream&, const Lista&); // Pisanje.
};

#endif