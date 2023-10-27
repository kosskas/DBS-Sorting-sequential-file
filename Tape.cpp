#include "Tape.h"

int nOfReads = 0;
int nOfWrites = 0;

Tape::Tape(string filename, ios_base::openmode flags) : filename(filename), flags(flags){
	file = new fstream();
	file->open(filename, flags);
	r_ptr = 0;
	w_ptr = 0;
	r_idx = BUFFSIZE;
	w_idx = 0;
}

Record Tape::readNext() {
	if (r_idx > BUFFSIZE - 1) {
		writeToBuff();
		r_idx = 0;
	}
	return buffer[r_idx++];
}

void Tape::writeRecord(Record rec) {
	if (w_idx > BUFFSIZE - 1) {
		writeToFile();
		w_idx = 0;
	}
	buffer[w_idx++] = rec;
}

void Tape::writeToBuff() {
	//readFromFile
	file->seekg(r_ptr);
	streamsize bytesRead = file->read((char*)buffer, sizeof(buffer)).gcount();
	if (bytesRead < sizeof(buffer)) {
		memset((char*)buffer + bytesRead, 0, sizeof(buffer) - bytesRead); //jeœli przeczytano mniej ni¿ ca³¹ stronê, wyzeruj dalsze
	}
#if _DEBUG
	printf("\n\tprzeczytano %ld\n", bytesRead);
#endif
	r_ptr += file->gcount();
	nOfReads++;

}

void Tape::writeToFile() {
#if _DEBUG
	streampos startPos = file->tellp();
#endif
	const char* serialRec = (const char*)buffer; //serializacja rekordów
	file->seekp(w_ptr);
	file->write(serialRec, sizeof(Record) * w_idx);
	w_ptr += sizeof(Record) * w_idx;

#if _DEBUG
	streamsize bytesWritten = file->tellp() - startPos;
	printf("\n\tZapisano %ld bajtow\n", bytesWritten);
#endif
	nOfWrites++;
}

void Tape::forceWriteRecord(Record rec) {
	const char* serialRec = (const char*)&rec; //serializacja rekordu
	file->seekp(w_ptr);
	file->write(serialRec, sizeof(Record));
	w_ptr += sizeof(Record);
}


void Tape::resetCursor() {
	r_ptr = 0;
	w_ptr = 0;
	file->clear();
	file->seekg(r_ptr, ios::beg);
	file->seekp(w_ptr, ios::beg);
}

void Tape::clearBuffer() {	
	memset(buffer, 0, sizeof(buffer));
	resetBufferPtr();
}

void Tape::clearFile() {
	file->close();
	file->open(filename, flags | ios::trunc);
	resetCursor();
	clearBuffer();
	//resetBufferPtr();
}

void Tape::printTape() {
	//czytanie niebuforowane
	
	Record rec(0, 0, 0);
	r_ptr = 0;
	do {
		file->seekg(r_ptr);
		if (file->read((char*)&rec, sizeof(Record))) {
			r_ptr += sizeof(Record);
		}
		else {
			break;
		}
		printf("%.1lf ", rec.getField());
	}
	while (true);
	resetCursor();
	
	/*
	//czytanie buforowane
	r_ptr = 0;
	resetBufferPtr();

	Record rec(0, 0, 0);
	while (true) {
		rec = readNext();
		if (rec.isEmpty()) 
			break;
		printf("%.1lf ", rec.getField());
	}
	resetCursor();
	clearBuffer();
	resetBufferPtr();
	*/
}

void Tape::printRecords() {
	r_ptr = 0;
	resetBufferPtr();
	Record rec(0, 0, 0);
	while (true) {
		rec = readNext();
		if (rec.isEmpty())
			break;
		printf("(%d %d %d) ", rec.a, rec.b, rec.h);
	}
	printf("\n");
	resetCursor();
	clearBuffer();
	//resetBufferPtr();
}

void Tape::printBuffer()
{
	for (int i = 0; i < BUFFSIZE; i++) {
		printf("%.1lf ", buffer[i].getField());
	}
	printf("widx=%d\n", w_idx);
}

void Tape::resetBufferPtr() {
	r_idx = BUFFSIZE;
	w_idx = 0;
}

Tape::~Tape() {
	file->close();
	delete file;
}
