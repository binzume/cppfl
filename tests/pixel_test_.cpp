#include <string>
#include <iostream>
#include "../include/all.h"
using namespace std;

struct  PixelRGB1555{
	PixelRGB1555(){}
	PixelRGB1555(unsigned short v){set(v);}
	typedef unsigned char element_type;
	union {
		unsigned short value;
		struct {
		public:
			element_type r:5;
			element_type g:5;
			element_type b:5;
			element_type a:1;
		};
	};
	unsigned char R(){ return (r<<5) | (r>>2); }
	unsigned char G(){ return (g<<5) | (g>>2); }
	unsigned char B(){ return (b<<5) | (b>>2); }
	unsigned char A(){ return a*255;}
	inline void set(unsigned short v){value=v;}
	inline unsigned short get(){return value;}
};



int main(int argc,char *argv[])
{

	BitmapRGB bmp(1024,768);
	BitmapRGB b = bmp.clip(16,20,100,100);

	PixelRGB1555 a(0xffff);
	a.r=10;
	cout << (int)a.G() << endl;
	cout << (int)a.g << endl;
	cout << sizeof(long long) << endl;

	PixelRGB pix[10];
	pix[1]=0x00ffff;
	cout << sizeof(pix) << pix[1] << endl;
	return 0;
}
