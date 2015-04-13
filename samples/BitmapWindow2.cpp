#include "../include/all.h"

int main(int argc,char **argv)
{

	BitmapWindow window("Sample app",640,480);
	Pen &p = *window.pen();
	p.print("Hello, world!");

	WBitmap bmp=Resources::LoadBitmap("test.bmp");
	bmp.draw(*((DCPen*)&p));

	p.color(Color::red);
	p.print(STR 123+"Ç†Ç†Ç†"+"Ç¢Ç¢Ç¢a");

	p.color(Color::green);
	p.box(20,32,100,100);
	p.boxf(100,120,200,230);
	p.line(0,40,200,100);
	p.color(0,255,255);
	p.circlef(300,200,100);
	
	window.update();

	int x=100,y=100,dx=1,dy=2;
	while (window.isexist()) {
		// å√Ç¢â~Çè¡Ç∑
		window.text(STR "mouse("+window.cursorPos().x+","+window.cursorPos().y+")");

		p.color(Color::white);
		p.circlef(x,y,10);
		// à⁄ìÆ
		x+=dx;
		y+=dy;
		if (y<10 || y>=window.dib->height-10) dy=-dy,y+=dy;
		if (x<10 || x>=window.dib->width-10) dx=-dx,x+=dx;
		// â~Çï`âÊ
		p.color(Color::red);
		p.circlef(x,y,10);
		if (System::keydown(VK_ESCAPE)) break;
		wait(10);
	}

	p.release();
	bmp.dispose();
	return 0;
}

