#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/date.h"
using namespace std;

void test()
{
	cout << (string)Date::today() << " " << (string)Time::now() << endl;
	cout << (string)DateTime::now() << endl;
	Second s=Second::now();
	s++;

	assert((string)Date(2009, 6, 19) == "2009-06-19");
	assert((string)DateTime(Date(2009, 6, 19), Time(23, 59, 59)) == "2009-06-19 23:59:59");

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}

