#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/url.h"
using namespace std;

bool test()
{

	net::Url url("http://www.binzume.net:80/path?hoge=1#frag");

	assert(url.valid);
	assert(url.scheme == "http");
	assert(url.host == "www.binzume.net");
	assert(url.port == 80);
	assert(url.path == "/path");
	assert(url.query == "hoge=1");
	assert(url.fragment == "frag"); // TODO: "#frag"
	assert(url.str() == "http://www.binzume.net:80/path?hoge=1#frag");

	net::Url url2("http//aaa");
	assert(url2.valid == false);

	return true;
}

int main(int argc,char *argv[])
{
	if (test()) {
		cout << "[OK]" << endl;
	}
	return 0;
}
