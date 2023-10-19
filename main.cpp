#include <iostream>
#include "Tape.h"

#define inputPath "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\rekordy11"
#define tmp1Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t1"
#define tmp2Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t2"
#define tmp3Path "D:\\PG\\5sys\\Struktury baz danych\\Projekt\\record_gen\\t3"
using namespace std;


Tape* switchTape(Tape* current, Tape* a, Tape* b) {
	return current == a ? b : a;
}

int main() {
/**
* Dopóki jest seria - zapisuj na jednym z plików
* Jeœli jest nowa seria - zmieñ plik
* Scalaj
* Powtórz (ale do kiedy?) -> gdy zostanie jedna taœma, nie bêdzie ju¿ zmiany serii
*/
	Tape input(inputPath, ios::binary | ios::in | ios::out);
	Tape t1(tmp1Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t2(tmp2Path, ios::binary | ios::in | ios::out | ios::trunc);
	Tape t3(tmp3Path, ios::binary | ios::in | ios::out | ios::trunc);

	double field1 = 0.0 ,field2  = 0.0, oldField1 = 0.0, oldField2 = 0.0;
	bool changeOfSerie = false;
	bool singleTape = true; //warunek koñcowy

	Record rec1, rec2;
	Tape* inputTape = &input;
	Tape* currentTape = &t1; //swichtape

#if _DEBUG
	input.printTape();
#endif

	while (true) {
		t1.resetCursor();
		t2.resetCursor();
		singleTape = true;


		//dystrybucja
		while (true) {
			rec1 = inputTape->readNext();
			if (inputTape->isEnd())
				break;
			field1 = rec1.getField();
			if (field1 >= oldField1) {
				oldField1 = field1;
			}
			else {

				singleTape = false;
				oldField1 = field1;
				currentTape = switchTape(currentTape, &t1, &t2);
			}
			currentTape->writeRecord(rec1);
#if _DEBUG
			printf("\nZapisano %.1lf na %c", rec1.getField(), (currentTape == &t1 ? 'A' : 'B'));
#endif
		}
		//zrzuæ resztê
		t1.writeToFile();
		t2.writeToFile();
		printf("\nT1\n");
		//t1.printBuffer();
		t1.printTape();
		printf("\nT2\n");
		t2.printTape();
		t1.resetCursor();
		t2.resetCursor();
		///je¿eli nie wykryto zmiany taœmy - koniec sortowania! (przepisz ponownie na stary plik?? zmieñ nazwê
		if (singleTape)
			break;
		//scalanie


		bool serie1 = true, serie2 = true;
#if _DEBUG
		break;
#endif
		//sprawdz ktory plik jest eof a który nie

		inputTape = &t3;
	}
#if _DEBUG

	//t2.printBuffer();
#endif


	return 0;
}
/*
* seekp
* seekg
* D:\PG\5sys\Struktury baz danych\Projekt\record_gen>python record_gen.py

https://cplusplus.com/reference/istream/istream/seekg/
https://cplusplus.com/reference/fstream/fstream/
https://stackoverflow.com/questions/55598861/stdfstream-read-block-of-data-from-file-and-write-data-back-to-file-until-eof


	//for (Record rec = t1.readNext(); !rec.isEmpty(); rec = t1.readNext()) {
	//	t2.writeRecord(rec);
	//}
	//t1.resetCursor();
	//t2.resetCursor();

	//printf("T2\n");
	//while (!(rec = t2.readNext()).isEmpty()) {
	//	printf("%d %d %d\n", rec.a, rec.b, rec.h);
	//}
*/

/*


while (true) {
		t1.resetCursor();
		t2.resetCursor();
		singleTape = true;
		//dystrybucja
		while (!inputTape->isEnd()) {
			rec1 = inputTape->readNext();
			field1 = rec1.getField();
			if (field1 >= oldField1) {
				oldField1 = field1;
			}
			else {

				singleTape = false;
				oldField1 = field1;
				currentTape = switchTape(currentTape, &t1, &t2);
			}

			currentTape->writeRecord(rec1);
#if _DEBUG
			printf("\nZapisano %.1lf na %c", rec1.getField(), (currentTape == &t1 ? 'A' : 'B'));
#endif
		}

		t1.resetCursor();
		t2.resetCursor();
		///je¿eli nie wykryto zmiany taœmy - koniec sortowania! (przepisz ponownie na stary plik?? zmieñ nazwê
		if (singleTape)
			break;

#if _DEBUG
		printf("\nT1\n");
		while (!t1.isEnd()) {
			rec1 = t1.readNext();
			printf("%.1lf ", rec1.getField());
		}
		printf("\nT2\n");
		while (!t2.isEnd()) {
			rec2 = t2.readNext();
			printf("%.1lf ", rec2.getField());
		}
		t1.resetCursor();
		t2.resetCursor();
#endif
		//scalanie
		bool serie1 = true, serie2 = true;
#if _DEBUG
		break;
#endif
		//sprawdz ktory plik jest eof a który nie

		inputTape = &t3;
	}
*/