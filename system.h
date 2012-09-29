#pragma once
#ifndef _SYSTEM_H
#define _SYSTEM_H

class Mouse {
public:
	int x,y;
	void update(){
		POINT p;
		GetCursorPos(&p);
		x=p.x; y=p.y;
	}
	static void move(int x,int y) {
		SetCursorPos(x,y);
	}
	static long pos(){
		POINT p;
		GetCursorPos(&p);
		return p.x+(p.y<<16);
	}
	static Point point(){
		POINT p;
		GetCursorPos(&p);
		return Point(p.x,p.y);
	}
	static void lclick(int x,int y){
		mouse_event(MOUSEEVENTF_LEFTDOWN,x,y,0,NULL);
		mouse_event(MOUSEEVENTF_LEFTUP,x,y,0,NULL);
	}
};

class System {
public:
	static Mouse mouse;
	static int keydown(int k) {
		int r = GetKeyState(k);
		return (r>>16)&1;
	}
	class KeyArray{
		public:
		int operator [](int k) {
			return keydown(k);
		}
	};
	
};

#ifndef NOIMP
Mouse System::mouse;
#endif

#ifdef _IMAGE_H
class Screen {
public:
	HDC hdc;
	int width,height;
	Screen() {
		hdc=CreateDC(TEXT("DISPLAY"),0,0,0);
		width=GetDeviceCaps(hdc , HORZRES);
		height=GetDeviceCaps(hdc , VERTRES);
	}
	~Screen() {
		DeleteDC(hdc);
	}
	void capture(DIBitmap &bmp, int x, int y, int w, int h) {
		BitBlt(bmp.getdc(),0,0,w,h,hdc,x,y,SRCCOPY);
	}
	void capture(DIBitmap &bmp, int x, int y) {
		BitBlt(bmp.getdc(),0,0,bmp.width,bmp.height,hdc,x,y,SRCCOPY);
	}
	DIBitmap* capture(int x, int y, int w, int h) {
		DIBitmap *bmp = new DIBitmap(w,h);
		capture(*bmp, x, y, w, h);
		return bmp;
	}
	static Size size(){
		RECT rect;
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
		Size sz;
		sz.width = rect.right - rect.left;
		sz.height = rect.bottom - rect.top;
		return sz;
	}
};
#endif
#endif
