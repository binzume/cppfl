#ifndef CPPFL_BITMAP_H
#define CPPFL_BITMAP_H

#define RGB24(r,g,b) ((b)|(g)<<8|(r)<<16)
#define RGBA32(r,g,b,a) ((b)|(g)<<8|(r)<<16|(a)<<24)
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
	// old
	void clear(const PIXEL &c) {
		for (int x=0;x<width;x++) *(PIXEL*)(buf+x*bpp)=c; // first line
		for (int y=1;y<height;y++) {
			int p=y*rowbytes/4;
			for (int x=0;x<rowbytes/4;x++) {
				*((long*)buf+p+x)=*((long*)buf+x);
			}
		}
	}
	void clear(const Color &c) {
		PIXEL pix(c);
		clear(pix);
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

#endif
