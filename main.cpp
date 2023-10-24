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
	/**
	* Dopóki jest seria - zapisuj na jednym z plików
	* Jeœli jest nowa seria - zmieñ plik
	* Scalaj
	* Powtórz (ale do kiedy?) -> gdy zostanie jedna taœma, nie bêdzie ju¿ zmiany serii
	*
	* Argumenty do maina
	* help -h
	* Logika typu wpisz w³¹sne rekordy -o
	* wyœwietl przed i po sortowaniu -s
	* wyœwietl po ka¿dym etapie sortowania -r
	*
	* nazwa pliku testowego na koñcu -f
	*/
	///zmieñ posortowanego na input
	//usuñ tmpki

	/**Problemy
	* Wczytywanie na taœmê blisko koñca pliku
	* czyszczenie bufora !!!plików tmp przed zapisem
	*/

	bool ownRec = false, eachStep = false, begEnd = false;
	string fileName = "";
	
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
	
	Tape input(fileName, ios::binary | ios::in | ios::out | (ownRec ? 16 : 0));
	Tape t1(tmp1Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t2(tmp2Path, ios::binary | ios::in | ios::out | ios::trunc);
	
	if (ownRec) {
		fileName = "temp";
		char cmd;
		int a, b, h;
		printf("\nWrite r X X X to put record\nWrite 'q' to end\n");
		while (true) {
			cin >> cmd;
			if (cmd == 'q') {
				break;
			}
			if (cmd == 'r') {
				cin >> a >> b >> h;
				input.writeRecord(Record(a, b, h));
			}		
		}
		input.writeToFile();
	}
	

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

	printf("\nRekordow N = %d\nLiczba pocz. serii = %d\nLiczba faz r = %d\nLiczba zapisow = %d\nLiczba odczytow = %d\nWielkosc bufora b = %d",nOfRecords - 1,r, nOfPhases, nOfWrites, nOfReads, BUFFSIZE);
	
	double lfaz = ceil(log2(r));
	double opdysk = 4 * nOfRecords / BUFFSIZE * lfaz;
	printf("\nPrzypadek œredni\nMax liczba faz = %.1lf\nOcz. lic. op. dysk. = %.1lf\nRzeczywista = %d", lfaz,opdysk, nOfReads + nOfWrites);


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
		if (field >= oldField) {
			oldField = field;
		}
		else {

			sorted = false;
			oldField = field;
			currentTape = switchTape(currentTape, t1, t2);
			if (firstRun) {
				r++;
			}

		}
		if (firstRun) {
			r++;
		}
		currentTape->writeRecord(rec);
#if _DEBUG
		printf("\nZapisano %.1lf na %c", rec.getField(), (currentTape == t1 ? 'A' : 'B'));
#endif
	}
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

		if (rec1.isEmpty() && rec2.isEmpty()) {
			break;
		}

		//seria
		while (!(rec1.isEmpty() || rec2.isEmpty())) {
			if (rec1.getField() <= rec2.getField()) {
				oldField1 = rec1.getField();

				dest->writeRecord(rec1);
#if _DEBUG
				printf("Zapisano %lf \n", rec1.getField());
#endif
				rec1 = t1->readNext();

				if (rec1.getField() < oldField1) {
					serie1 = false;
					break;
				}

			}
			else {
				oldField2 = rec2.getField();
				dest->writeRecord(rec2);
#if _DEBUG
				printf("Zapisano %lf \n", rec2.getField());
#endif
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
#if _DEBUG
				printf("Dopisano %lf \n", rec1.getField());
#endif
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
#if _DEBUG
				printf("Dopisano %lf \n", rec2.getField());
#endif
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
#if _DEBUG
					printf("Na koniec Zapisano %lf \n", rec2.getField());
#endif
					rec2 = t2->readNext();
				}
			}
			else {
				while (!rec1.isEmpty()) {
					dest->writeRecord(rec1);
#if _DEBUG
					printf("Na koniec Zapisano %lf \n", rec1.getField());
#endif
					rec1 = t1->readNext();
				}
			}
		}
	}
#if _DEBUG
	dest->printBuffer();
#endif
	dest->writeToFile();
#if _DEBUG
	dest->printTape();
#endif
	dest->clearBuffer();
	t1->clearFile();
	t2->clearFile();
}



/*
* seekp
* seekg
* D:\PG\5sys\Struktury baz danych\Projekt\record_gen>python record_gen.py

https://cplusplus.com/reference/istream/istream/seekg/
https://cplusplus.com/reference/fstream/fstream/
https://stackoverflow.com/questions/55598861/stdfstream-read-block-of-data-from-file-and-write-data-back-to-file-until-eof
*/