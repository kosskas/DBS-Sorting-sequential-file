#include <iostream>
#include <string>
#include "Tape.h"
#include <math.h>
#define inputPath "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\rekordy"
#define tmp1Path "t1"
#define tmp2Path "t2"

using namespace std;

//rec -1 bo liczony takze pusty
int r = 0,nOfRecords = 0, firstRun = 1;

Tape* switchTape(Tape* current, Tape* a, Tape* b);
bool distribute(Tape* input, Tape* t1, Tape* t2);
void merge(Tape* t1, Tape* t2, Tape* dest);

int main(int argc, char** argv) {
/*
- o nie ma korzystaæ z bufora,od razu do pliku
pewne cleary siê powtarzaj¹
*/

	bool ownRec = false, eachStep = 0, begEnd = 0;
	string fileName = "";//"D:\\PG\\5sys\\Struktury baz danych\\Projekt\\Project1\\Release\\test1";
	
	if (argc == 1) {
		printf("Usage:\n-h - help\n-o - write own records until q is pressed\n-s - show file before sorting and after\n-r - show file after every run\n-f FILENAME - run own test file");
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {
		string opt(argv[i]);
		if (opt == "-f" && i + 1 < argc) {
			fileName = argv[i + 1];
			i++;
		}
		if (opt == "-o") {
			ownRec = true;
			fileName = "temp";
		}
		if (opt == "-s")
			begEnd = true;
		if (opt == "-r")
			eachStep = true;
		if (opt == "-h") {
			printf("Usage:\n-h - help\n-o - write own records until q is pressed\n-s - show file before sorting and after\n-r - show file after every run\n-f FILENAME - run own test file");
			return 1;
		}
	}
	
	//ownRec = 1;
	Tape input(fileName, ios::binary | ios::in | ios::out );
	Tape t1(tmp1Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t2(tmp2Path, ios::binary | ios::in | ios::out | ios::trunc);
	cout << "\n\n" << fileName;
	int nOfPhases = 0;
	bool sorted = false; //warunek koñcowy
	if (begEnd) {
		printf("Input\n");
		input.printTape();
		printf("\n");
	}
	while (true) {

		sorted = distribute(&input, &t1, &t2);
		if (sorted) {
			break; ///je¿eli nie wykryto zmiany taœmy - koniec sortowania!
		}
		merge(&t1, &t2, &input);
		nOfPhases++;
		if (eachStep) {
			printf("Phase %d\n", nOfPhases);
			input.printTape();
			printf("\n");
		}
	}
	if (begEnd) {
		printf("Sorted\n");
		t1.printTape();
	}
	printf("\nRekordow N = %d\nLiczba pocz. serii = %d\nLiczba faz r = %d\nLiczba zapisow = %d\nLiczba odczytow = %d\nWielkosc bufora b = %d", nOfRecords - 1, r, nOfPhases, nOfWrites, nOfReads, BUFFSIZE);

	double lfaz = ceil(log2(nOfRecords/2));
	double opdysk = 4 * nOfRecords / BUFFSIZE * lfaz;
	printf("\nPrzypadek sredni\nTeor. liczba faz = %.1lf\nOcz. lic. op. dysk. = %.1lf\nRzeczywista = %d", lfaz,opdysk, nOfReads + nOfWrites);


	return 0;
}
Tape* switchTape(Tape* current, Tape* a, Tape* b) {
	return current == a ? b : a;
}

bool distribute(Tape* inputTape, Tape* t1, Tape* t2) {
	double field = 0.0, oldField = 0.0;
	Record rec;
	Tape* currentTape = t1; //swichtape
	bool sorted = true;
		//dystrybucja
	while (true) {
		rec = inputTape->readNext();
		if (firstRun)
			nOfRecords++;
		if (rec.isEmpty())
			break;
		field = rec.getField();
		if (field >= oldField)
			oldField = field;
		else {
			sorted = false;
			oldField = field;
			currentTape = switchTape(currentTape, t1, t2);
			if (firstRun) {
				r++;
			}
		}
		currentTape->writeRecord(rec);
	}
	if (firstRun)
		r++;
	firstRun = 0;
	//zrzuæ resztê
	t1->writeToFile();
	t2->writeToFile();

	t1->clearBuffer();
	t2->clearBuffer();

	inputTape->clearFile();
	return sorted;
}

void merge(Tape* t1, Tape* t2, Tape* dest) {
	bool serie1 = true, serie2 = true;
	double field1 = 0.0, field2 = 0.0, oldField1 = 0.0, oldField2 = 0.0;
	Record rec1, rec2;
	rec1 = t1->readNext();
	rec2 = t2->readNext();
	while (true) {
		//scalaj seriami
		field1 = field2 = oldField1 = oldField2 = 0.0;
		serie1 = serie2 = true;

		if (rec1.isEmpty() && rec2.isEmpty())
			break;

		//seria
		while (!(rec1.isEmpty() || rec2.isEmpty())) {
			if (rec1.getField() <= rec2.getField()) {
				oldField1 = rec1.getField();
				dest->writeRecord(rec1);
				rec1 = t1->readNext();

				if (rec1.getField() < oldField1) {
					serie1 = false;
					break;
				}

			}
			else {
				oldField2 = rec2.getField();
				dest->writeRecord(rec2);

				rec2 = t2->readNext();

				if (rec2.getField() < oldField2) {
					serie2 = false;
					break;
				}

			}
		}
		//uzupe³nij do serii
		if (!serie2) {
			while (serie1) {
				dest->writeRecord(rec1);
				oldField1 = rec1.getField();
				rec1 = t1->readNext();
				if (rec1.getField() < oldField1) {
					serie1 = false;
					break;
				}
			}
		}
		else if (!serie1) {
			while (serie2) {
				oldField2 = rec2.getField();
				dest->writeRecord(rec2);
				rec2 = t2->readNext();
				if (rec2.getField() < oldField2) {
					serie2 = false;
					break;
				}
			}
		}
		//koniec któregoœ z plików
		else {

			if (rec1.isEmpty()) {
				while (!rec2.isEmpty()) {
					dest->writeRecord(rec2);
					rec2 = t2->readNext();
				}
			}
			else {
				while (!rec1.isEmpty()) {
					dest->writeRecord(rec1);
					rec1 = t1->readNext();
				}
			}
		}
	}
	dest->writeToFile();
	dest->clearBuffer();
	t1->clearFile();
	t2->clearFile();
}