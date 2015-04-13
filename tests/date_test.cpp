#include <iostream>
#include "../include/date.h"
using namespace std;

void test()
{
	cout << (string)Date::today() << " " << (string)Time::now() << endl;
	cout << (string)DateTime::now() << endl;
	Second s=Second::now();
	s++;

	cout << "ok." << endl;
}

int main(int argc,int argv[])
{
	test();
	return 0;
}

