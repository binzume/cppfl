#include <iostream>
#include "../include/http.h"
#include "../include/htmlrenderer.h"
#include "../include/all.h"

using namespace std;
using namespace net;

void dumpDomTree(HTML::Element *e,int d=0)
{
	for (int i=0;i<d;i++)
		cout << "  " ;
	cout << e->tagName ;
	if (e->tagName=="_text") {
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

class Screen1 : public HTML::Screen {
	BitmapWindow *win;
public:
	Screen1(BitmapWindow &w){height=w.size().height;width=w.size().width;win=&w;}
	void color(int c){win->color(c);}
	void boxf(int x,int y,int w,int h){win->boxf(x,y,w,h);}
	void line(int x,int y,int w,int h){win->line(x,y,w,h);}
	void draw_char(int c,int x,int y){win->move(x,y);char s[4]={0,0,0,0};s[0]=c;s[1]=c>>8; win->print(s);}
};

int main(int argc,char *argv[])
{
//	HttpClient hc;
//	hc.load("http://www.binzume.net/");

	string s =
		"<html>"
			"<head><title>ƒ^ƒCƒgƒ‹</title></head>"
			"<body>"
				"<h2 id='testid'>head</h2>"
				"<p>aaa<a href='aaa.html'>link‚ ‚ ‚ </a></p>"
				"sss<hr />aa"
			"</body>"
		"</html>";

	HTML::Document document;
	document.parse(s.c_str(),s.length());
	dumpDomTree(&document);

	HTML::Element *e = document.getElementById("testid");
	if (e) {
		cout << "testid: " << e->tagName << " " << e->getInnerText() << endl;
	}

	// draw test
	BitmapWindow win("brows",320,240);
	Screen1 scr(win);
	document.layout(scr.width);
	document.draw(&scr,0,-20);
	wait(win);

	return 0;
}
