#include "../cppfl/all.h"

int main(int argc,char **argv) 
{
	EMF emf("test.emf",640,480);
	Pen &p=*emf.pen();
	
	p.clear();
	p.text("Hello, world!");
	p.line(20,20,400,400);
	p.color(255,0,0);
//	msgBox(STR emf.size().w);
	p.circlef(440,440,180);
	p.color(5,0,0);
	p.circle(10,10,200);

	return 0;
}

