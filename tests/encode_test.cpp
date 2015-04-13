
#include <iostream>
#include <string>
#include <vector>
#include "../include/encode.h"


using namespace std;
using namespace Encode;


void test() {
	string s = "ABC‚ ‚¢‚¤123Š¿Žšaa";

	string t=encode(s,SJIS,UTF8);
	cout << t << endl;
	
	cout << "SJIS: ";
	for (SJIS_IteratorS it(s.begin(),s.end());!it.eof(); )
		cout << it.shift() << " ";
	cout << endl;

	vector<int> a;
	cout << "UTF-8: ";
	for (UTF8_IteratorS it(t.begin(),t.end());it!=t.end();++it ) {
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
	
	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}
