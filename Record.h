#pragma once
class Record {
//private:
public:
	int a, b, h;
//public:
	Record();
	Record(int a, int b, int h);
	double getField();
	bool isEmpty();
	Record empty();
};

