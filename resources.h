#ifndef _RESOURCES_H
#define _RESOURCES_H

namespace kwlib{

#include "pen.h"

class ResImage {
public:
	ResImage(){};
	int height();
	int width();
};

class WBitmap : public ResImage{
public:
	HBITMAP hBitmap;
	WBitmap(){}
	WBitmap(HBITMAP hBmp){
		hBitmap=hBmp;
	}
	WBitmap(int w,int h){
		HDC hDC=GetDC(NULL);
		hBitmap=CreateCompatibleBitmap(hDC,w,h);
		ReleaseDC(NULL,hDC);
	}
	HBITMAP handle(){
		return hBitmap;
	}
	int height(){
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		return bmp.bmHeight;
	}
	int width(){
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		return bmp.bmWidth;
	}
	Size size(){
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		Size sz(bmp.bmWidth,bmp.bmHeight);
		return sz;
	}
	void draw(HDC hDC,int x=0,int y=0){
		HDC hBmpDC = CreateCompatibleDC(hDC);
		Size sz = size();
		SelectObject(hBmpDC , hBitmap);
		BitBlt(hDC,x,y,sz.w,sz.h,hBmpDC,0,0,SRCCOPY);
		DeleteDC(hBmpDC);
	}
	void dispose(){
		if (hBitmap) DeleteObject(hBitmap);
		hBitmap=NULL;
	}
	Pen* pen(){
		HDC hDC=GetDC(NULL);
		HDC hBmpDC = CreateCompatibleDC(hDC);
		SelectObject(hBmpDC , hBitmap);
		DCPen *p=new DCPen(hBmpDC,size());
		p->hdcown=true;
		ReleaseDC(NULL,hDC);
		return (Pen*)p;
	}
	operator HBITMAP(){
		return hBitmap;
	}
};

class WIcon : public ResImage{
public:
	HICON hIcon;
	WIcon(HICON hIco){
		hIcon=hIco;
	}
	HICON handle(){
		return hIcon;
	}
	void dispose(){
		if (hIcon) DeleteObject(hIcon);
		hIcon=NULL;
	}
	operator HICON(){
		return hIcon;
	}
};

class Resources{
public:
	static WBitmap LoadBitmap(const std::string name){
		HBITMAP hBitmap;
		hBitmap = (HBITMAP)LoadImage( NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
		return WBitmap(hBitmap);
	}
	static WBitmap getBitmap(int id){
		return WBitmap((HBITMAP)LoadImage(Application.hInstance, (PCSTR)id, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR ));
	}
	static WIcon getIcon(int id){
		return WIcon((HICON)LoadImage(Application.hInstance, (PCSTR)id, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR ));
		//return WIcon((HICON)::LoadIcon(Application.hInstance,(PCSTR)id ));
	}
	static WIcon getIcon(const std::string name){
		return WIcon((HICON)LoadImage(Application.hInstance, name.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR ));
		//return WIcon((HICON)::LoadIcon(Application.hInstance, name.c_str() ));
	}
};

}

#endif
