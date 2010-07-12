#ifndef _IMAGE_H
#define _IMAGE_H

#define RGB24(r,g,b) ((b)|(g)<<8|(r)<<16)
#define HSV32(h,s,v) ((h)<<16|(s)<<8|(v))

#ifndef _PEN_H
typedef void Pen;
typedef void DCPen;
typedef void ImgPen;
#endif



#pragma pack (1)
union PIXEL{
	PIXEL(){};
	PIXEL(const Color &c){r=c.r;g=c.g;b=c.b;a=c.a;};
	struct {
		unsigned char b,g,r,a;
	};
	struct{
		unsigned long rgb:24;
		unsigned long _padding:8;
	};
	struct {
		unsigned char v,s;
		short h;
	};
	unsigned long value;
	unsigned char rgba[4];
	int getv() {return (r*5 + g*9 + b*2)/16;}
};
#pragma pack ()



class Image{
public:
	long width,height;
	Image() {
		width = height = 0;
	}
	virtual ~Image(){
		free();
	}
	Size size() const {return Size(width,height);}
	virtual void free() {}
	virtual int create(int w,int h,int d) {return 0;}
	virtual class Pen* pen() {return NULL;}
};


class Bitmap : public Image{
	PIXEL dummy; // static?
protected:
	unsigned long bitmask;
	bool buf_alloc;
public:
	int rowbytes;
	int depth;
	int bpp;
	unsigned char *buf;

	Bitmap() {
		buf_alloc=false;
		buf = NULL;
		depth = 0;
	}
	Bitmap(int w,int h,int d=24) {
		buf_alloc=false;
		create(w,h,d);
	}
	Bitmap(void *pbuf,int w,int h,int d=24) {
		buf_alloc=false;
		set_header(w,h,d);
		buf = (unsigned char*)pbuf;
	}
	virtual ~Bitmap(){
		free();
	}

	int create(int w,int h,int d=24) { // d=8,16,24,32
		free();
		set_header(w,h,d);
		buf = new unsigned char[rowbytes*height+4];
		buf_alloc=true;
		return 0;
	}
	int create_noalloc(int w,int h,int d) { // d=8,16,24,32
		free();
		set_header(w,h,d);
		return 0;
	}
	void free() {
		if (buf_alloc) delete [] buf;
		buf_alloc=false;
	}

	int set_header(int w,int h,int d) {
		width = w;
		height = h;
		depth = d;
		bpp = d>>3;
		bitmask = (depth==32)?0xffffffff:((1<<depth)-1);
		rowbytes = (w*bpp+3)&~3;
		return 0;
	}
	PIXEL *pixel(int x,int y) {
		if (x<0 || y<0 || x>=width || y>=height) return &dummy;
		return (PIXEL*)(buf+rowbytes*y+bpp*x);
	}
	PIXEL *pixel(const Point &p) {return pixel(p.x,p.y);}
	const PIXEL *pixel(int x,int y) const{
		if (x<0 || y<0 || x>=width || y>=height) return &dummy;
		return (PIXEL*)(buf+rowbytes*y+bpp*x);
	}
	const PIXEL *pixel(const Point &p) const{return pixel(p.x,p.y);}
	unsigned long get(int x,int y) const {
		return *(long*)(buf+rowbytes*y+x*bpp)&bitmask;
	}
	void set(int x,int y,unsigned long v) {
		memcpy(buf+rowbytes*y+bpp*x,&v,bpp);
		//*(long*)(buf+rowbytes*y+bpp*x)&=~bitmask;
		//*(long*)(buf+rowbytes*y+bpp*x)|=v&bitmask;
	}
	void set(const Point &p,unsigned long v) {
		memcpy(buf+rowbytes*p.y+bpp*p.x,&v,bpp);
		//*(long*)(buf+rowbytes*p.y+bpp*p.x)&=~bitmask;
		//*(long*)(buf+rowbytes*p.y+bpp*p.x)|=v&bitmask;
	}
	void clear(const PIXEL &c) {
		for (int x=0;x<width;x++) *(PIXEL*)(buf+x*bpp)=c; // first line
		for (int y=1;y<height;y++) {
			int p=y*rowbytes/4;
			for (int x=0;x<rowbytes/4;x++) {
				*((long*)buf+p+x)=*((long*)buf+x);
			}
		}
	}
	void clear() {
		for (int i=0;i<rowbytes*height/4;i++) *((long*)buf+i)=0x00000000;
	}
	void clearWhite() {
		for (int i=0;i<rowbytes*height/4;i++) *((long*)buf+i)=0xffffffff;
	}
	
	void clone(Bitmap &b){
		unsigned char *buf2=b.buf;
		create(b.width,b.height,b.depth);
		for (int i=0;i<rowbytes*height/4;i++) *((long*)buf+i)=*((long*)buf2+i);
	}

	void drawto(Bitmap &b,int px=0,int py=0){
		int sx=0,sy=0;
		if (px<0) {sx=-px;px=0;}
		if (py<0) {sy=-py;py=0;}
		if (b.bpp == bpp) {
			int w=width, h=height;
			unsigned char *dst=b.buf, *src=buf;
			dst+=b.rowbytes*py + bpp*px;
			src+=rowbytes*sy + bpp*sx;
			for (int y=0; y<h; y++) {
				memcpy(dst,src,w*bpp);
				dst+=b.rowbytes;
				src+=rowbytes;
			}
		} else {
			for(int y=0;y+sy<height && py+y<b.height;y++) {
				for(int x=0;x+sx<width && px+x<b.width;x++) {
					b.set(px+x,py+y,get(x+sx,y+sy));
				}
			}
		}
	}
	void draw(Bitmap &b,int px=0,int py=0){
		drawto(b,px,py);
	}
	void draw(Bitmap &b,const Point &p){
		drawto(b,p.x,p.y);
	}

	Pen *pen(){
#ifdef _PEN_H
		return new BitmapPenT<Bitmap>(this);
#else
		return NULL;
#endif
	}
	Bitmap& operator=(Bitmap &a) {
		free();
		memcpy(this,&a,sizeof(a)); // *this = a;
		a.buf_alloc=false;
		return *this;
	}
	const PIXEL& operator()(int x,int y) const{
		return *(PIXEL*)(buf+rowbytes*y+bpp*x);
	}
	PIXEL& operator()(int x,int y) {
		return *(PIXEL*)(buf+rowbytes*y+bpp*x);
	}
	const PIXEL& operator[](const Point &p) const{
		return *(PIXEL*)(buf+rowbytes*p.y+bpp*p.x);
	}
	PIXEL& operator[](const Point &p) {
		return *(PIXEL*)(buf+rowbytes*p.y+bpp*p.x);
	}
};
//typedef BitmapPenT<Bitmap> ImgPen;




#pragma pack(push,1)
class PixelRGB{
public:
	unsigned char r,g,b;
	PixelRGB(){}
	PixelRGB(unsigned long v){set(v);}
	PixelRGB(const Color &c){r=c.r;g=c.g;b=c.b;}
	operator unsigned long(){return get();}
	inline unsigned long get(){
		return r | (g<<8) | (b<<16);
	}
	inline void set(unsigned long v){
		r = v&0xff;
		g = (v>>8)&0xff;
		b = (v>>16)&0xff;
	}
};
union PixelRGBA{
public:
	PixelRGBA(){};
	PixelRGBA(unsigned long v){set(v);}
	PixelRGBA(const Color &c){r=c.r;g=c.g;b=c.b;a=c.a;};
	operator unsigned long(){return get();}
	struct {
		unsigned char b,g,r,a;
	};
	struct{
		unsigned long rgb:24;
		unsigned long _padding:8;
	};
	struct {
		unsigned char v,s;
		short h;
	};
	unsigned long value;
	unsigned char rgba[4];
	int getv() {return (r*5 + g*9 + b*2)/16;}
	inline void set(unsigned long v){value=v;}
	inline unsigned long get(){return value;}
};

struct  PixelRGB555{
	PixelRGB555(){}
	PixelRGB555(unsigned short v){set(v);}
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


#pragma pack(pop)


template <typename T>
class BitmapN : public Bitmap {
	int create(int,int,int){return 0;}
	typedef T pixel_type;
public:
	BitmapN(){}
	BitmapN(int w,int h) : Bitmap(w,h,sizeof(T)*8) {}
	BitmapN(const Size &sz) : Bitmap(sz.w,sz.h,sizeof(T)*8) {}
	BitmapN(void *pbuf,int w,int h) : Bitmap(pbuf,w,h,sizeof(T)*8)  {
	}

	T& operator()(int x,int y) {
		return ((T*)(buf+rowbytes*y))[x];
	}
	const T& operator()(int x,int y) const{
		return ((T*)(buf+rowbytes*y))[x];
	}
	const PIXEL& operator[](const Point &p) const{
		return *(PIXEL*)&(((T*)(buf+rowbytes*p.y))[p.x]);
	}
	PIXEL& operator[](const Point &p) {
		return *(PIXEL*)&(((T*)(buf+rowbytes*p.y))[p.x]);
	}
	int create(int w,int h){return Bitmap::create(w,h,sizeof(T)*8);}

	unsigned long get(int x,int y) const {
		return ((T*)(buf+rowbytes*y))[x];
	}
	unsigned long get(Point p) const {
		return get(p.x,p.y);
	}
	void set(int x,int y,T v) {
		((T*)(buf+rowbytes*y))[x]=v;
	}
	void set(const Point &p,T v) {
		return set(p.x,p.y,v);
	}

	using Bitmap::drawto;
	template <class T2>
	void drawto(BitmapN<T2> &b,int px=0,int py=0){
		int sx=0,sy=0;
		if (px<0) {sx=-px;px=0;}
		if (py<0) {sy=-py;py=0;}
		for(int y=0;y+sy<height && py+y<b.height;y++) {
			for(int x=0;x+sx<width && px+x<b.width;x++) {
				b.set(px+x,py+y,get(x+sx,y+sy));
			}
		}
	}

};

typedef BitmapN<unsigned char> Bitmap8;
typedef BitmapN<unsigned short> Bitmap16;
typedef BitmapN<unsigned long> Bitmap32;
typedef BitmapN<PixelRGB> BitmapRGB;
typedef BitmapN<PixelRGBA> BitmapRGBA;
typedef BitmapN<PixelRGB555> BitmapRGB555;

// hh:F‘Š 0`1536 R:0 Y:300 G:512 B:1024
// ss:Ê“x 0`255
// vv:–¾“x 0`255
static inline PIXEL rgb2hsv(const PIXEL *p)
{
	int m,d,min,max;

	if(p->b<p->g){
		max=p->g; min=p->b;
		d = p->b - p->r;
		m = 1;
	} else {
		max=p->b; min=p->g;
		d = p->r - p->g;
		m = 2;
	}
	if(max<p->r){
		max=p->r;
		d = p->g - p->b;
		m = 0;
	}
	if(min>p->r) min=p->r;
	PIXEL c;
	c.v = max;
	c.s = max-min;
	//Cs = (max>0) ? (max-min)*255/max : 0;
	c.h = (c.s!=0) ? d*256/(max-min) + m*512 : 0;

	return c;
}



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
