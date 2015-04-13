#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/base64.h"
using namespace std;


void test() {

	cout << base64::encode("abc1234") << endl;

    assert(base64::encode("abc1234") == "YWJjMTIzNA==");
    assert(base64::decode("YWJjMTIzNA==") == "abc1234");
    assert(base64::decode("YWJjMTIz") == "abc123");
    cout << "ok." << endl;

}

int main(int argc, char * argv[])
{
	test();
	return 0;
}
