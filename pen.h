#ifndef _PEN_H
#define _PEN_H

class Pen{
	friend class Image;
	friend class ResImage;
protected:
	int curx,cury;
	Color fgcolor;
	Color bgcolor;
	Pen(){}
	void init(){
		curx=cury=0;
		fgcolor=COLOR::black;
		bgcolor=COLOR::white;
	}
public:
	virtual ~Pen(){}
	virtual void move(int x,int y) {curx=x; cury=y;}
	virtual void color(int r,int g,int b) {}
	virtual void clear(const Color &c=COLOR::white) {}
	virtual void line(int x1,int y1,int x2,int y2) {}
	virtual void box(int x1,int y1,int w,int h) {}
	virtual void boxf(int x1,int y1,int w,int h) {}
	virtual void circle(int cx,int cy,int r){}
	virtual void circlef(int cx,int cy,int r){}
	virtual void text(const std::string &s) {}
	virtual void print(const std::string &s) {}
	virtual void font(Font *font,bool ownflag=false){}
	virtual Color& color() {return fgcolor;}
	void move(const Point &p) {move(p.x,p.y);}
	void circle(const Point &p,int r) {circle(p.x,p.y,r);}
	void circlef(const Point &p,int r) {circlef(p.x,p.y,r);}
	void color(const Color &c) { color(c.r,c.g,c.b); }
	void line(const Point &sp,const Point &ep) {line(sp.x,sp.y,ep.x,ep.y);}
	void lineto(const Point &ep) {line(curx,cury,ep.x,ep.y);}
	//void lines(const std::vector<Point> &p) {}
	virtual void release() {delete this;}
};

class DupPen : public Pen {
public:
	using Pen::color;
	using Pen::line;
	using Pen::circle;
	using Pen::circlef;
	using Pen::box;
	using Pen::boxf;
	using Pen::move;
	Pen *pen1,*pen2;
	DupPen(Pen *p1,Pen *p2){
		pen1=p1;pen2=p2;
	}
	DupPen(Pen &p1,Pen &p2){
		pen1=&p1;pen2=&p2;
	}
	void color(int r,int g,int b) {
		if (pen1) pen1->color(r,g,b);
		if (pen2) pen2->color(r,g,b);
	}
	void clear(const Color &c=COLOR::white) {
		if (pen1) pen1->clear(c);
		if (pen2) pen2->clear(c);
	}
	void line(int x1,int y1,int x2,int y2) {
		if (pen1) pen1->line(x1,y1,x2,y2);
		if (pen2) pen2->line(x1,y1,x2,y2);
	}
	void box(int x1,int y1,int w,int h) {
		if (pen1) pen1->box(x1,y1,w,h);
		if (pen2) pen2->box(x1,y1,w,h);
	}
	void boxf(int x1,int y1,int w,int h) {
		if (pen1) pen1->boxf(x1,y1,w,h);
		if (pen2) pen2->boxf(x1,y1,w,h);
	}
	void circle(int cx,int cy,int r){
		if (pen1) pen1->circle(cx,cy,r);
		if (pen2) pen2->circle(cx,cy,r);
	}
	void circlef(int cx,int cy,int r){
		if (pen1) pen1->circlef(cx,cy,r);
		if (pen2) pen2->circlef(cx,cy,r);
	}
	void print(const std::string &s) {
		if (pen1) pen1->print(s);
		if (pen2) pen2->print(s);
	}
	void text(const std::string &s) {
		if (pen1) pen1->text(s);
		if (pen2) pen2->text(s);
	}
	void font(Font *font,bool ownflag=false){
	}
	virtual ~DupPen(){
		if (pen1) pen1->release();
		if (pen2) pen2->release();
	}
};


template<typename T>
class BitmapPenT : public Pen {
protected:
	class Image *target;
	T *img;
public:
	using Pen::color;
	using Pen::line;
	using Pen::circle;
	using Pen::circlef;
	using Pen::box;
	using Pen::boxf;
	using Pen::move;
	BitmapPenT(){
		target=NULL;
		img=NULL;
		init();
	}
	BitmapPenT(T *b){
		img=b;
		target=(Image*)b;
		init();
	}
	void color(int r,int g,int b) {
		fgcolor.b=b;
		fgcolor.g=g;
		fgcolor.r=r;
	}

	void clear(const Color &c) {
		curx=0; cury=0;
		PIXEL pix(c);
		for (int x=0;x<img->width;x++) *(PIXEL*)(img->buf+x*img->bpp)=pix; // first line
		for (int y=1;y<img->height;y++) {
			int p=y*img->rowbytes/4;
			for (int x=0;x<img->rowbytes/4;x++) {
				*((long*)img->buf+p+x)=*((long*)img->buf+x);
			}
		}
	}
	void line(int x1,int y1,int x2,int y2) {
		if (!img) return;
		int t,x,y;
		if (x1>x2) {t=x1;x1=x2;x2=t;}
		if (y1>y2) {t=y1;y1=y2;y2=t;}
		int w=x2-x1, h=y2-y1;
	
		if (w < h) {
			for (y=0;y<=h;y++) {
				x=y*w/h;
				img->pixel(x1+x,y1+y)->rgb=fgcolor.rgb;
			}
		} else {
			for (x=0;x<=w;x++) {
				y=x*h/w;
				img->pixel(x1+x,y1+y)->rgb=fgcolor.rgb;
			}
		}
	}
	void box(int x1,int y1,int w,int h) {
		if (!img) return;
		int x2=x1+w-1 , y2=y1+h-1;
		int x;
	
		for (x=x1;x<=x2;x++) {
			img->pixel(x,y1)->rgb=fgcolor.rgb;
			img->pixel(x,y2)->rgb=fgcolor.rgb;
		}
		for (;y1<y2;y1++) {
			img->pixel(x1,y1)->rgb=fgcolor.rgb;
			img->pixel(x2,y1)->rgb=fgcolor.rgb;
		}
	}
	void boxf(int x1,int y1,int w,int h) {
		if (!img) return;
		int x2=x1+w-1 , y2=y1+h-1;
		int x,y;
	
		for (y=y1;y<y2;y++) {
			for (x=x1;x<x2;x++) {
				img->pixel(x,y)->rgb=fgcolor.rgb;
			}
		}
	}
	void circle(int cx,int cy,int r){
		int dx=r,dy=0,t=r;
		while (dx >= dy) {
			img->pixel(cx+dx,cy+dy)->rgb=fgcolor.rgb;
			img->pixel(cx+dx,cy-dy)->rgb=fgcolor.rgb;
			img->pixel(cx-dx,cy+dy)->rgb=fgcolor.rgb;
			img->pixel(cx-dx,cy-dy)->rgb=fgcolor.rgb;
			img->pixel(cx+dy,cy+dx)->rgb=fgcolor.rgb;
			img->pixel(cx+dy,cy-dx)->rgb=fgcolor.rgb;
			img->pixel(cx-dy,cy+dx)->rgb=fgcolor.rgb;
			img->pixel(cx-dy,cy-dx)->rgb=fgcolor.rgb;
		
			t -= 2*dy + 1;
			if (t < 0) { dx--; t += 2*dx;}
			dy++;
		}
	}
	void circlef(int cx,int cy,int r){
		int dx=r,dy=0,t=r;
		while (dx >= dy) {
			line(cx-dx,cy-dy,cx+dx,cy-dy);
			line(cx-dx,cy+dy,cx+dx,cy+dy);
	
			line(cx-dy,cy-dx,cx-dy,cy+dx);
			line(cx+dy,cy-dx,cx+dy,cy+dx);
		
			t -= 2*dy + 1;
			if (t < 0) { dx--; t += 2*dx;}
			dy++;
		}
	}

};

class DCPen : public Pen{
protected:
	HDC hDC;
	Font m_font;
	Size m_size;
public:
	using Pen::color;
	using Pen::line;
	using Pen::circle;
	using Pen::circlef;
	using Pen::box;
	using Pen::boxf;
	using Pen::move;
	bool hdcown;
	DCPen(){
		hDC=NULL;
		hdcown=false;
		init();
	}
	template<typename T>
	DCPen(T &b) {
		m_size=Size(b.width,b.height);
		hDC=CreateCompatibleDC(NULL);
		SelectObject(hDC,b.dib);
		hdcown=true;
		init();
	}
	DCPen(HDC hdc,const Size &size) : m_size(size){
		hDC=hdc;
		hdcown=false;
		SetBkMode(hDC,TRANSPARENT);
		init();
	}

	DCPen(HDC hdc,int width=-1, int height=-1) : m_size(width,height){
		if (width<0) {
			m_size = Size(GetDeviceCaps(hDC, HORZRES),GetDeviceCaps(hDC, VERTRES));
		}
		hDC=hdc;
		hdcown=false;
		SetBkMode(hDC,TRANSPARENT);
		init();
	}


	~DCPen(){
		if (hdcown) DeleteDC(hDC);
	}
	void font(Font *font,bool ownflag=false){
		m_font.hFont=font->hFont;
		m_font.ownflag=ownflag;
		SelectObject(hDC, m_font.hFont);
	}
	Font &font() {
		return m_font;
	}

	void color(int r,int g,int b) {
		fgcolor.b=b;
		fgcolor.g=g;
		fgcolor.r=r;
		LOGPEN p;
		p.lopnStyle = PS_SOLID;
		p.lopnWidth.x = 1;
		p.lopnColor =RGB(r,g,b);
		DeleteObject(SelectObject(hDC , CreatePenIndirect(&p)));
		DeleteObject(SelectObject(hDC , CreateSolidBrush(RGB(r,g,b))));
	}
	void color(const Color &c){color(c.r,c.g,c.b);}
	void clear(const Color &c){
		bgcolor.r=c.r;
		bgcolor.g=c.g;
		bgcolor.b=c.b;
		clear();
	}

	void clear(){
		Color c=fgcolor;
		color(bgcolor.r,bgcolor.g,bgcolor.b);
		boxf(0 , 0 , m_size.width,m_size.height);
		color(c.r,c.g,c.b);
		curx=cury=0;
	}
	void text(const std::string &s) {
		SetTextColor(hDC,RGB(fgcolor.r,fgcolor.g,fgcolor.b));
		TextOut(hDC,curx,cury,s.c_str(),(int)s.length());
		curx=0;
		cury+=16;
	}
	void print(const std::string &s) {
		SetTextColor(hDC,RGB(fgcolor.r,fgcolor.g,fgcolor.b));
		SIZE sz;
		int len=(int)s.length();
		int pos=0,l;
		while(pos<len) {
			l=(int)s.find('\n',pos);
			if (l==std::string::npos) {
				break;
			}
			if(l>pos)TextOut(hDC,curx,cury,s.c_str()+pos,l-pos);
			GetTextExtentPoint32(hDC,"A",1,&sz);
			pos=l+1;
			cury+=sz.cy;
			curx=0;
		}
		if (len>pos)TextOut(hDC,curx,cury,s.c_str()+pos,len-pos);
		GetTextExtentPoint32(hDC,s.c_str()+pos,len-pos,&sz);
		curx+=sz.cx;
		if (curx>=m_size.width) {
			cury+=sz.cy;
			curx=0;
		}
	}
	void line(int x,int y,int x2,int y2) {
		MoveToEx(hDC,x,y , NULL);
		LineTo(hDC,x2,y2);
	}
	void lines(const Point *points,int n) {
		Polyline(hDC , (POINT*)points , n);
	}
	void box(int x,int y,int w,int h) {
		HGDIOBJ hb=SelectObject(hDC, GetStockObject(NULL_BRUSH));
		Rectangle(hDC , x , y , x+w , y+h);
		DeleteObject(SelectObject(hDC,hb));
	}
	void boxf(int x,int y,int w,int h) {
		Rectangle(hDC , x , y , x+w , y+h);
	}
	void circle(int cx,int cy,int r) {
		HGDIOBJ hb=SelectObject(hDC, GetStockObject(NULL_BRUSH));
		Ellipse(hDC,cx-r,cy-r,cx+r,cy+r);
		DeleteObject(SelectObject(hDC,hb));
	}
	void circlef(int cx,int cy,int r) {
		Ellipse(hDC,cx-r,cy-r,cx+r,cy+r);
	}
	void draw(Point p,DCPen *src,Rect rc) {
		BitBlt(hDC,p.x,p.y,rc.w,rc.h,src->hDC,rc.x,rc.y,SRCCOPY);
	}
	void draw(Point p,HDC hdc,const Rect &rc) {
		BitBlt(hDC,p.x,p.y,rc.w,rc.h,hdc,rc.x,rc.y,SRCCOPY);
	}
	const Size& size(){return m_size;};
	operator HDC(){return hDC;}
	void release(){
		DeleteObject(SelectObject(hDC, GetStockObject(NULL_BRUSH)));
		DeleteObject(SelectObject(hDC, GetStockObject(NULL_PEN)));
		Pen::release();
	}
};

#endif
