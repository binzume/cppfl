#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../include/http.h"
using namespace std;

bool test()
{

	net::HttpClient client;
	auto res = client.get("http://www.binzume.net/");

	assert(res.status == 200);
	assert(res.content != "");
	assert(res.getHeader("Content-Type").find("text/html") == 0);


	// Not found.
	auto res2 = client.get("http://www.binzume.net/aqswdefrtgy_not_exists_url");
	assert(res2.status == 404);

	return true;
}

int main(int argc,char *argv[])
{
	if (test()) {
		cout << "[OK]" << endl;
	}
	return 0;
}
