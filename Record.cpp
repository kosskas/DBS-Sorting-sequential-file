#include "Record.h"

Record::Record() {
	this->a = 0;
	this->b = 0;
	this->h = 0;
}

Record::Record(int a, int b, int h) {
	this->a = a;
	this->b = b;
	this->h = h;
}
/*
Record::Record() {
	this->a = 0;
}
Record::Record(int a, int b, int c) {
	this->a = a;

}*/
double Record::getField()
{
	return (double)(0.5 * (a + b) * h);
	//return a;
}

bool Record::isEmpty()
{
	return !(a || b || h);
	//return !(a);
}

