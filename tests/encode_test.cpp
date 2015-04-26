#undef NDEBUG
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "../include/encode.h"

using namespace std;
using namespace Encode;


void test() {

	// "ABC‚ ‚¢‚¤123Š¿Žšaa"
	string sjis = "\x41\x42\x43\x82\xA0\x82\xA2\x82\xA4\x31\x32\x33\x8A\xBF\x8E\x9A\x61\x61"; // SJIS
	string utf8 = "\x41\x42\x43\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\x31\x32\x33\xE6\xBC\xA2\xE5\xAD\x97\x61\x61"; // utf-8

	// sjis -> utf-8
	string ret=encode(sjis, SJIS, UTF8);
	cout << ret << endl;
	assert(ret == utf8);

	// utf-8 -> sjis
	ret=encode(utf8, UTF8, SJIS);
	cout << ret << endl;
	assert(ret == sjis);

	// misc
	cout << "SJIS: ";
	for (SJIS_IteratorS it(sjis.begin(),sjis.end());!it.eof(); )
		cout << it.shift() << " ";
	cout << endl;

	vector<int> a;
	cout << "UTF-8: ";
	for (UTF8_IteratorS it(utf8.begin(),utf8.end());it!=utf8.end();++it ) {
		cout << *it << " ";
		a.push_back(*it);
	}
	cout << endl;

	string utf;
	stringEncode_UTF8(utf,a.begin(),a.end());
	cout << utf << endl;

	for (UTF8_IteratorS it(utf.begin(),utf.end());it!=utf.end();++it ) {
		cout << *it << " ";
	}
	cout << endl;

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}
