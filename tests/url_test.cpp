#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/url.h"
using namespace std;

void test()
{

	net::Url url("http://www.binzume.net:80/path#frag");

	cout << url.str() << endl;

	assert(url.valid);
	assert(url.scheme == "http");
	assert(url.host == "www.binzume.net");
	assert(url.port == 80);
	assert(url.path == "/path");
	assert(url.fragment == "frag"); // TODO: "flag" or "#frag"
	assert(url.str() == "http://www.binzume.net:80/path#frag");

	net::Url url2("http//aaa");
	assert(url2.valid == false);

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}

