#ifndef _BASE_H
#define _BASE_H



struct Point{
	int x,y;
	Point(){};
	Point(int x_,int y_){x=x_;y=y_;}
	bool operator==(const Point &p) {
		return (p.x==x && p.y==y);
	}
	bool operator!=(const Point &p) {
		return !(p.x==x && p.y==y);
	}
#ifdef _COMPLEX_
	template<typename T>
	Point(std::complex<T> p){x=(int)p.real();y=(int)p.imag();}
	template<typename T>
	operator std::complex<T>(){return std::complex<T>((T)x,(T)y);}
#endif
#ifdef _IOSTREAM_
	friend
	std::ostream& operator<< (std::ostream &os,Point &p) {os << "(" <<p.x<<","<<p.y<< ")";return os;}
#endif
};

struct Size{
	Size(){};
	Size(int w_,int h_){w=w_;h=h_;}
	union {
		struct {
			int w,h;
		};
		struct {
			int width,height;
		};
	};
};

struct Rect{
	Rect(){};
	Rect(int _x,int _y,int w_,int h_){x=_x;y=_y;w=w_;h=h_;}
	int x,y;
	union {
		struct {
			int w,h;
		};
		struct {
			int width,height;
		};
	};
};

struct Color{
	Color(){};
	Color(int t) : r(t), g(t>>8), b(t>>16), a(t>>24){}
	Color(int rr,int gg,int bb){r=rr;g=gg;b=bb;a=0;}
	Color(int rr,int gg,int bb,int aa){r=rr;g=gg;b=bb;a=aa;}
	union{
		unsigned long rgba;
		struct{
			unsigned char g,b,r,a;
		};
		struct{
			unsigned int _rgb:24;
			unsigned int _padding:8;
		};
		struct{
			unsigned char g,b,r;
		} __rgb;
		unsigned long rgb;
	};
	bool operator==(const Color &c) {
		return c.rgba==rgba;
	}
	bool operator!=(const Color &c) {
		return c.rgba!=rgba;
	}

	static const int black = 0x000000;
	static const int white = 0xffffff;
	static const int red = 0x0000ff;
	static const int green = 0x00ff00;
	static const int blue = 0x0000ff;
	static const int yellow = 0x00ffff;
	static const int magenta = 0xff00ff;
	static const int cyan = 0xffff00;

	// HTML3.2
	static const int lime = 0x00FF00;
	static const int aqua = 0xffff00;
	static const int fuchsia = 0xff00ff;
	static const int maroon = 0x000080;
	static const int olive = 0x008080;
	static const int green_ = 0x008000;
	static const int teal = 0x808000;
	static const int navy = 0x800000;
	static const int purple = 0x800080;


	static const int dimgray = 0x696969;
	static const int gray = 0x808080;
	static const int darkgray = 0xa9a9a9;
	static const int silver = 0xc0c0c0;
	static const int lightgray = 0xd3d3d3;

	static const int orange = 0x00a5ff;

	static const int darkblue = 0x8b0000;
	static const int mediumblue = 0xcd0000;
	static const int skyblue = 0xebce87;
	static const int lightblue = 0xe6d8ad;
	static const int aliceblue = 0xfff8f0;

	static const int darkgreen = 0x006400;
	static const int lightgreen = 0x90ee90;
	static const int seagreen = 0x578b2e;

	static const int darkred = 0x008b00;

	static const int pink = 0xcbc0ff;
	static const int lightpink = 0xc1b6ff;
	static const int hotpink = 0xb469ff;

	static const int orangered = 0x0045ff;

	
};

namespace COLOR{
	const static Color black(0,0,0);
	const static Color red(255,0,0);
	const static Color green(0,255,0);
	const static Color blue(0,0,255);
	const static Color yellow(255,255,0);
	const static Color magenta(255,0,255);
	const static Color cyan(0,255,255);
	const static Color white(255,255,255);
	const static Color gray(128,128,128);
};


static std::string operator+(const std::string &s1,int i) {
	char s2[10];
	sprintf(s2,"%d",i);
    return s1+s2;
}
static std::string& operator+=(std::string &s1,int i) {
	char s2[10];
	sprintf(s2,"%d",i);
	s1+=s2;
    return s1;
}

#endif
