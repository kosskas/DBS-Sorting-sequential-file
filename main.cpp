#include <iostream>
#include <string>
#include "Tape.h"
#include <math.h>

#define tmp1Path "t1"
#define tmp2Path "t2"

using namespace std;

//rec -1 bo liczony takze pusty
int r = 0,nOfRecords = 0, firstRun = 1;

Tape* switchTape(Tape* current, Tape* a, Tape* b);
void distribute(Tape* input, Tape* t1, Tape* t2);
bool merge(Tape* t1, Tape* t2, Tape* dest);

void getInputToFile();
int main(int argc, char** argv) {
	bool ownRec = false, eachStep = 0, begEnd = 0;
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
			getInputToFile();
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
	
	Tape input(fileName, ios::binary | ios::in | ios::out );
	Tape t1(tmp1Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t2(tmp2Path, ios::binary | ios::in | ios::out | ios::trunc);

	printf("\n\n");
	int nOfPhases = 0;
	
	if (begEnd) {
		printf("Input\n");
		input.printTape();
		printf("\n");
	}
	bool sorted = false; //warunek koñcowy
	while (true) {
		distribute(&input, &t1, &t2);

		sorted = merge(&t1, &t2, &input);

		nOfPhases++;
		if (sorted)
			break; ///koniec sortowania!

		if (eachStep) {
			printf("Phase %d\n", nOfPhases);
			input.printTape();
			printf("\n");
		}
	}
	if (begEnd) {
		printf("Sorted\n");
		input.printTape();
	}
	nOfRecords -= 1; //bo liczony tak¿e pusty
	printf("\nRecords N = %d\nInitial series = %d\nPhases r = %d\nWrites = %d\nReads = %d\nBuffer size b = %d", nOfRecords, r, nOfPhases, nOfWrites, nOfReads, BUFFSIZE);

	//double lfaz = ceil(log2(nOfRecords /2));
	double lfaz = ceil(log2(r));
	double opdysk = 4 * nOfRecords / BUFFSIZE * lfaz;
	printf("\nAvg case\nExpected phases = %.1lf\nExpected r/w = %.1lf\nReal r/w = %d", lfaz,opdysk, nOfReads + nOfWrites);


	return 0;
}
Tape* switchTape(Tape* current, Tape* a, Tape* b) {
	return current == a ? b : a;
}

void distribute(Tape* inputTape, Tape* t1, Tape* t2) {
	double field = 0.0, oldField = 0.0;
	Record rec;
	Tape* currentTape = t1; //swichtape
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
}

bool merge(Tape* t1, Tape* t2, Tape* dest) {
	bool serie1 = true, serie2 = true;
	bool sorted = true;
	double oldField1 = 0.0, oldField2 = 0.0;;
	Record rec1, rec2;

	rec1 = t1->readNext();
	rec2 = t2->readNext();
	while (true) {
		//scalaj seriami
		oldField1 = oldField2 = 0.0;
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
					if (!rec1.isEmpty())
						sorted = false;
					break;
				}
			}
			else {
				oldField2 = rec2.getField();
				dest->writeRecord(rec2);
				rec2 = t2->readNext();
				if (rec2.getField() < oldField2) {
					serie2 = false;
					if (!rec2.isEmpty())
						sorted = false;
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
					if (!rec1.isEmpty())
						sorted = false;
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
					if(!rec2.isEmpty())
						sorted = false;
					break;
				}
			}
		}
		//koniec któregoœ z plików
		else {
			if (rec1.isEmpty()) {
				while (!rec2.isEmpty()) {
					oldField2 = rec2.getField();
					dest->writeRecord(rec2);
					rec2 = t2->readNext();
					if (rec2.getField() < oldField2 && !rec2.isEmpty()) {
						sorted = false;
					}
				}
			}
			else {
				while (!rec1.isEmpty()) {
					oldField1 = rec1.getField();
					dest->writeRecord(rec1);
					rec1 = t1->readNext();				
					if (rec1.getField() < oldField1 && !rec1.isEmpty()) {
						sorted = false;
					}
				}
			}
		}
	}

	dest->writeToFile();
	dest->clearBuffer();
	t1->clearFile();
	t2->clearFile();

	return sorted;
}
void getInputToFile() {
	int n, a, b, h;
	printf("Enter number of records in file.\n");
	cin >> n;

	Tape input("temp", ios::binary | ios::in | ios::out | ios::trunc);
	printf("Records has 3 values - (a, b, h)\n");
	while (n--) {
		cin >> a >> b >> h;
		input.forceWriteRecord(Record(a, b, h));
		printf("\tSaved Record(%d,%d,%d)\n", a, b, h);
	}
}