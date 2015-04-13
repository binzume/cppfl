#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include "../include/htmlparser.h"
using namespace std;

ostream & operator <<(ostream &os,const wstring &ws){
	
	std::string s;
	os << Encode::stringEncode_SJIS(s,ws.begin(),ws.end());
	return os;
}

void dumpDomTree(HTML::Element *e,int d=0)
{
	for (int i=0;i<d;i++)
		cout << "  " ;
	cout << e->tagName ;
	if (e->nodeType==3) {
		cout << "[" << ((HTML::TextNode*)e)->text << "]";
	}
	cout << endl ;
	if (e->isContainer()) {
		HTML::Container *e2 = (HTML::Container*)e; 
		unsigned int i;
		for (i=0;i<e2->childNodes.size();i++) {
			dumpDomTree(e2->childNodes[i],d+1);
		}
	}
}

void test()
{
	string s =
		"<html>"
			"<head><title>タイトル</title></head>"
			"<body>"
				"<h2 id='testid'>head</h2>"
				"<p>aaa<a href='aaa.html' name=\"abcd\">linkあああ</a></p>"
				"sss<hr />aa"
			"</body>"
		"</html>";

	HTML::Document document;
	document.parse(s);
	dumpDomTree(&document);

	HTML::Element *e = document.getElementById("testid");
	if (e) {
		cout << "testid: " << e->tagName << " " << e->getInnerText() << endl;
	}

	HTML::NodeList list;
	if (document.getElementsByName(list,"abcd").size()){
		cout << "abcd: " << list[0]->tagName << " " << list[0]->getInnerText() << endl;
	}

	assert(e != NULL);
	assert(document.getElementsByName(list,"abcd").size() == 1);
	assert(list[0]->tagName == "a");

	cout << "ok." << endl;
}

int main(int argc,char *argv[])
{
	test();
	return 0;
}
