#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Record.h"
using namespace std;

#define BUFFSIZE 512// 16 64 256 512

extern int nOfReads;
extern int nOfWrites;

class Tape {
private:
	fstream *file;
	string filename;
	ios_base::openmode flags;
	uint32_t r_ptr;
	uint32_t w_ptr;
	uint32_t r_idx;
	uint32_t w_idx;
	Record buffer[BUFFSIZE] = { Record(0,0,0) };
public:
	void writeToBuff();
	void writeToFile();
	void forceWriteRecord(Record rec);
	Tape(string filename, ios_base::openmode flags);
	Record readNext();
	void writeRecord(Record rec);
	void resetCursor();
	void resetBufferPtr();
	void clearBuffer();
	void clearFile();
	void printTape();
	void printRecords();
	void printBuffer();
	~Tape();
};

