#include <iostream>
#include <string>
#include "../cppfl/file.h"
using namespace std;

int main()
{
	Directory dir(".");
	string s;
	do {
		s=*dir;
		cout << s << endl;
	} while(s.size());
	dir.close();

	File f("File.cpp");
	string buf;
	f.load(buf);
	cout << buf << endl;

	return 0;
}

