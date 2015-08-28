#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/base64.h"
using namespace std;


bool test()
{
    assert(base64::encode("abc1234") == "YWJjMTIzNA==");
    assert(base64::decode("YWJjMTIzNA==") == "abc1234");
    assert(base64::decode("YWJjMTIz") == "abc123");

	return true;
}

int main(int argc,char *argv[])
{
	if (test()) {
		cout << "[OK]" << endl;
	}
	return 0;
}
