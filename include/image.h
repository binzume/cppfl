#ifndef _IMAGE_H
#define _IMAGE_H

#include "bitmap.h"

class Drawable {
protected:
	HDC m_hDC;
	Size sz;
public:
	HDC handle(){return m_hDC;}
	operator HDC(){return m_hDC;}
	const Size& size(){return sz;}
	Drawable(){m_hDC=NULL;}
	Drawable(HDC hDC){m_hDC=hDC;}
	Drawable(HDC hDC,const Size &size){m_hDC=hDC;sz=size;}
	virtual void draw(Drawable &img,int x=0,int y=0){
		BitBlt(img.m_hDC,x,y,sz.w,sz.h,m_hDC,0,0,SRCCOPY);
	}
	virtual void dispose(){DeleteDC(m_hDC);}
	virtual ~Drawable(){}
};


class BMP : public Drawable{
	HBITMAP hOrgBmp;
public:
	BMP(HBITMAP hBitmap){
		m_hDC=CreateCompatibleDC(NULL);
		hOrgBmp=(HBITMAP)SelectObject(m_hDC , hBitmap);
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		sz=Size(bmp.bmWidth,bmp.bmHeight);

	}
	virtual void dispose(){
		SelectObject(m_hDC , hOrgBmp);
		DeleteDC(m_hDC);
	}
};

class MaskedBMP : public Drawable{
	HBITMAP hOrgBmp;
	HBITMAP hMaskBmp;
	HDC hMaskDC;
public:
	MaskedBMP(HBITMAP hBitmap){
		m_hDC=CreateCompatibleDC(NULL);
		hOrgBmp=(HBITMAP)SelectObject(m_hDC , hBitmap);
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		sz=Size(bmp.bmWidth,bmp.bmHeight);

		hMaskDC = CreateCompatibleDC(m_hDC);
		hMaskBmp = CreateBitmap(sz.w,sz.h, 1, 1, NULL);
		SelectObject(hMaskDC, hMaskBmp);
	}
	MaskedBMP(HBITMAP hBitmap,const Color& c){
		m_hDC=CreateCompatibleDC(NULL);
		hOrgBmp=(HBITMAP)SelectObject(m_hDC , hBitmap);
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		sz=Size(bmp.bmWidth,bmp.bmHeight);

		hMaskDC = CreateCompatibleDC(m_hDC);
		hMaskBmp = CreateBitmap(sz.w,sz.h, 1, 1, NULL);
		SelectObject(hMaskDC, hMaskBmp);

		setTrans(c);
	}
	void setTrans(const Color &c){
		SetBkColor(m_hDC,RGB(c.r,c.g,c.b));
		BitBlt(hMaskDC, 0,0, sz.w,sz.h, m_hDC, 0,0, SRCCOPY);
		BitBlt(m_hDC, 0,0, sz.w,sz.h, hMaskDC ,0,0 ,0x220326);
	}
	virtual void draw(Drawable &img,int x=0,int y=0){
		SetTextColor(img.handle(),0);
		//BitBlt(img.handle(),x,y,size().w,size().h,hMaskDC,0,0,SRCAND);
		BitBlt(img.handle(),x,y,size().w,size().h,hMaskDC,0,0,SRCAND);
		BitBlt(img.handle(),x,y,size().w,size().h,m_hDC,0,0,SRCINVERT);
	}
	void dispose(){
		SelectObject(m_hDC , hOrgBmp);
		DeleteDC(m_hDC);
	}
	~MaskedBMP(){
		DeleteDC(hMaskDC);
		DeleteObject(hMaskBmp);
	}
};

class DIBitmap : public Bitmap{
public:
	HDC hdc;
	HBITMAP	dib;			// bitmap handle
	BITMAPINFOHEADER bmih;	// infoheader
	DIBitmap() {}
	DIBitmap(int w,int h,int d=24) {create(w,h,d);}
	virtual ~DIBitmap() {free();}
	DIBitmap(const DIBitmap &dst){
		*this=dst;
		const_cast<DIBitmap&>(dst).buf=NULL;
	}
	DIBitmap(const std::string &fname){
		load(fname.c_str());
	}
	DIBitmap &DIBitmap::operator=(const DIBitmap &o){
		memcpy(this,&o,sizeof(DIBitmap));
		const_cast<DIBitmap&>(o).buf=NULL;
		return *this;
	}
	DIBitmap(HBITMAP hBitmap){
		BITMAP bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		HDC hBmpDC=CreateCompatibleDC(NULL);
		HBITMAP hOrgBmp=(HBITMAP)SelectObject(hBmpDC , hBitmap);

		create(bmp.bmWidth,bmp.bmHeight,24);
		hdc=CreateCompatibleDC(NULL);
		SelectObject(hdc,dib);
		BitBlt(hdc,0,0,bmp.bmWidth,bmp.bmHeight,hBmpDC,0,0,SRCCOPY);
		DeleteDC(hdc);

		SelectObject(hBmpDC , hOrgBmp);
		DeleteDC(hBmpDC);
		hdc = NULL;

	}

	int create(int w,int h,int d=24) {
		free();
		width = w;
		height = h;
		depth = d;
		bpp = d>>3;
		bitmask = (depth==32)?0xffffffff:((1<<depth)-1);
		rowbytes = (w*bpp+3)&~3;

		memset(&bmih,0,sizeof(bmih));
		bmih.biSize=sizeof(BITMAPINFOHEADER);
		bmih.biWidth=w;
		bmih.biHeight=-h;
		bmih.biPlanes=1;
		bmih.biBitCount=d;
		bmih.biSizeImage=w*h*d>>3;
	
		dib=CreateDIBSection(NULL,(BITMAPINFO*)&bmih,DIB_RGB_COLORS,(void**)&buf,NULL,0);
//		hdc=CreateCompatibleDC(NULL);
//		SelectObject(hdc,dib);
		hdc = NULL;
		return 1;
	}
	void free() {
		if (buf) {
			releasedc();
			DeleteObject(dib);
			buf=NULL;
		}
	}

	HDC getdc(){
		if (hdc == NULL) {
			hdc=CreateCompatibleDC(NULL);
			SelectObject(hdc,dib);
		}
		return hdc;
	}
	void releasedc(){
		DeleteDC(hdc);
		hdc = NULL;
	}

	int load(const char *fname){
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;
		FILE *fp = fopen(fname,"rb");
		if (!fp) return 0;
		fread(&bf,sizeof(BITMAPFILEHEADER),1,fp);
		if (bf.bfType!=0x4d42) return 0;
		fread(&bi,sizeof(BITMAPINFOHEADER),1,fp);
		create(bi.biWidth,bi.biHeight,bi.biBitCount);
		//fread(dib->buf,dib->rowbytes,dib->height,fp);
		int y;
		for (y=height-1;y>=0;y--) {
			fread(buf+rowbytes*y,rowbytes,1,fp);
		}
		fclose(fp);
		return 1;
	}
	int save(const char *fname){
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;
		memset(&bf,0,sizeof(bf));
		memset(&bi,0,sizeof(bi));
		FILE *fp = fopen(fname,"wb");
		if (!fp) return 0;
		bf.bfType=0x4d42;
		bf.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+rowbytes*height;
		bf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
		fwrite(&bf,sizeof(BITMAPFILEHEADER),1,fp);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = depth;
		fwrite(&bi,sizeof(BITMAPINFOHEADER),1,fp);
		for (int y=height-1;y>=0;y--) {
			fwrite(buf+rowbytes*y,rowbytes,1,fp);
		}
		fclose(fp);
		return 1;
	}
	Pen *pen(){
		return (Pen*)dcpen();
	}
	DCPen *dcpen(){
#ifdef _PEN_H
		return new DCPen(*this);
		//return new DCPen(hdc,Size(width,height));
#else
		return NULL;
#endif
	}
	operator HBITMAP(){
		return dib;
	}
};


class EMF : public Image {
public:
	HDC hdc;
	EMF(char *f,int w,int h){
		HDC hDC2 = CreateDC("DISPLAY",NULL,NULL,NULL);
		RECT rc={0,0,
			w*100*GetDeviceCaps(hDC2,HORZSIZE)/GetDeviceCaps(hDC2,HORZRES),
			h*100*GetDeviceCaps(hDC2,VERTSIZE)/GetDeviceCaps(hDC2,VERTRES)};
		hdc=CreateEnhMetaFile(hDC2,f,&rc,NULL);
		DeleteDC(hDC2);
		width=w;
		height=h;
	}
	EMF(char *f){
		hdc=CreateEnhMetaFile(NULL,f,NULL,NULL);
	}
	~EMF(){
		close();
	}
	void close(){
		if (hdc) {
			HENHMETAFILE hm=CloseEnhMetaFile(hdc);
			hdc=NULL;
			DeleteEnhMetaFile(hm);
		}
	}
	Pen *pen(){
		return (Pen*)dcpen();
	}
	DCPen *dcpen(){
#ifdef _PEN_H
		return new DCPen(hdc,Size(width,height));
#else
		return NULL;
#endif
	}
};


#endif
