#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/file.h"
using namespace std;

void test()
{
	Directory dir(".");
	auto files = dir.entries();
	for (auto it = files.begin(); it != files.end(); ++it) {
		cout << *it << endl;
	}

	File f(__FILE__);
	string buf;
	f.load(buf);
	cout << "read " << buf.size() << " bytes." << endl;

	assert(f.size() > 0);
	assert(buf.size() == f.size());

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}

