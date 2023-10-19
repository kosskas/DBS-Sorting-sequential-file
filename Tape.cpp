#include "Tape.h"

Tape::Tape(string filename, ios_base::openmode flags){
	file = new fstream();
	file->open(filename, flags);
	r_ptr = 0;
	w_ptr = 0;
	r_idx = BUFFSIZE;
	w_idx = 0;
	end = false;
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
	if (file->read((char*)buffer, sizeof(buffer))) {
		r_ptr += sizeof(buffer);
	}
	else {
		clearBuffer();
		end = true;
	}


}

void Tape::writeToFile() {
	const char* serialRec = (const char*)buffer; //serializacja rekordów
	//for (int i = 0; i < sizeof(Record); i++) {
	//	printf("%02X ", serialRec[i]);
	//}
	file->seekp(w_ptr);
	file->write(serialRec, sizeof(Record)*w_idx );
	w_ptr += sizeof(Record) * w_idx;

}


void Tape::resetCursor() {
	r_ptr = 0;
	w_ptr = 0;
	end = false;
	file->clear();
	file->seekg(r_ptr, ios::beg);
	file->seekp(w_ptr, ios::beg);
}

bool Tape::isEnd(){
	return file->eof();
}

void Tape::clearBuffer() {	
	memset(buffer, 0, sizeof(buffer));
}

void Tape::printTape() {
	//zmieniæ na czytanie w³aœciwe
	/*
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
	*/
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
