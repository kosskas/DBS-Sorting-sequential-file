#pragma once
struct Record {
	int a, b, h;
	Record();
	Record(int a, int b, int h);
	double getField();
	bool isEmpty();
};