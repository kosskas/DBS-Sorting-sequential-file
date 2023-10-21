#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Record.h"
using namespace std;

#define BUFFSIZE 10

class Tape {
private:
	fstream *file;
	string filename;
	ios_base::openmode flags;
	uint32_t r_ptr;
	uint32_t w_ptr;
	uint32_t r_idx;
	uint32_t w_idx;
	bool end;
	Record buffer[BUFFSIZE] = { Record(0,0,0) };
public:
	void writeToBuff();
	void writeToFile();

	//static int loperacjidyskowych = 0;
	Tape(string filename, ios_base::openmode flags);
	Record readNext();
	void writeRecord(Record rec);
	void resetCursor();
	bool isEndOfBuffer();
	void resetBufferPtr();
	void clearBuffer();
	void clearFile();
	void printTape();
	void printRecords();
	void printBuffer();
	void resetFlags();
	//
	~Tape();
};

