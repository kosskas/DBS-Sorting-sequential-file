#include <iostream>
#include <stdio.h>
#include "Tape.h"

//#define inputPath "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\rekordy"
#define inputPath "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\rekordy2"
#define tmp1Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t1"
#define tmp2Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t2"
#define tmp3Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t3"
using namespace std;



Tape* switchTape(Tape* current, Tape* a, Tape* b);
bool distribute(Tape* input, Tape* t1, Tape* t2);
void merge(Tape* t1, Tape* t2, Tape* dest);
int main() {
/**
* Dopóki jest seria - zapisuj na jednym z plików
* Jeœli jest nowa seria - zmieñ plik
* Scalaj
* Powtórz (ale do kiedy?) -> gdy zostanie jedna taœma, nie bêdzie ju¿ zmiany serii
* 
* Logika typu wpisz w³¹sne rekordy
*/

/**Problemy
* Wczytywanie na taœmê blisko koñca pliku
* czyszczenie bufora !!!plików tmp przed zapisem
*/
	Tape input(inputPath, ios::binary | ios::in | ios::out);
	Tape t1(tmp1Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t2(tmp2Path, ios::binary | ios::in | ios::out | ios::trunc);


	int nOfPhases = 0;

	bool sorted = false; //warunek koñcowy
	printf("Wejscie \n");
	while (true) { //jak tu bedzie sorted to output w INPUT?
		input.printTape();
		printf("\n");
		sorted = distribute(&input, &t1, &t2);


		/*
		printf("\nT1\n");
		t1.printTape();
		printf("\nT2\n");
		t2.printTape(); 
		*/
		if (sorted) {
			break; ///je¿eli nie wykryto zmiany taœmy - koniec sortowania!
		}
		merge(&t1, &t2, &input);
		nOfPhases++;
	}

	printf("\nPosortowane \n");
	t1.printRecords();
	t1.printTape();

	printf("\nLiczba faz = %d\nLiczba zapisow = %d\nLiczba odczytow = %d", nOfPhases, nOfWrites, nOfReads);
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
		}
		currentTape->writeRecord(rec);
#if _DEBUG
		printf("\nZapisano %.1lf na %c", rec.getField(), (currentTape == t1 ? 'A' : 'B'));
#endif
	}
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
		else {
			//koniec któregoœ z plików
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

	//printf("\nkoniec etapu scalania\n");
}



/*
* seekp
* seekg
* D:\PG\5sys\Struktury baz danych\Projekt\record_gen>python record_gen.py

https://cplusplus.com/reference/istream/istream/seekg/
https://cplusplus.com/reference/fstream/fstream/
https://stackoverflow.com/questions/55598861/stdfstream-read-block-of-data-from-file-and-write-data-back-to-file-until-eof
*/