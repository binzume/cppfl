#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/http.h"
using namespace std;

void test()
{

	net::HttpClient client;
	auto res = client.get("http://www.binzume.net/");

	assert(res.status == 200);
	assert(res.content != "");
	assert(res.getHeader("Content-Type").find("text/html") == 0);


	// Not found.
	auto res2 = client.get("http://www.binzume.net/aqswdefrt5gy_not_exists");
	assert(res2.status == 404);

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}

