#ifndef _IMAGE_ITERATOR_H
#define _IMAGE_ITERATOR_H

template<typename T>
class line_iteratorT{
	//typedef int T;
public:
	T x,y;
private:
	T i,e;
	T d1,d2;
	T sx,sy;
	bool f;
public:
	inline void _line_iteratorT(const T &x0,const T &y0,const T &x1,const T &y1){
		T w = ( x1 > x0 ) ? x1 - x : x0 - x1;
		sx = ( x1 > x0 ) ? 1 : -1;
		T h = ( y1 > y0 ) ? y1 - y : y0 - y1;
		sy = ( y1 > y0 ) ? 1 : -1;
		f= (w >= h);

		if( f ) {
			d1=w*2;
			d2=h*2;
		} else {
			d1=h*2;
			d2=w*2;
		}

		i=0;
		e = d1;

	}
	line_iteratorT(const T &x0,const T &y0,const T &x1,const T &y1):x(x0),y(y0){
		_line_iteratorT(x0,y0,x1,y1);
	}
	line_iteratorT(const Point &p0,const Point &p2):x(p0.x),y(p0.y){
		_line_iteratorT(p0.x,p0.y,p1.x,p1.y);
	}
	line_iteratorT& operator++(){
		i += 2;
		e -= d2;
		if( f ) {
			x += sx;
			if( e < 0 ) {
				y += sy;
				e += d1;
			}
		} else {
			y += sy;
			if( e < 0 ) {
				x += sx;
				e += d1;
			}
		}
		return *this;
	}
	inline operator Point() const{return Point(x,y);}
	inline operator bool() const{return i<=d1;}
	inline Point operator *() const{return Point(x,y);}
};
typedef line_iteratorT<int> line_iterator;

template<typename T>
class rect_iteratorT {
	T w,h,sx;
public:
	T x,y;
	rect_iteratorT(const T &_x,const T &_y,const T &_w,const T &_h)
		:x(_x),y(_y),w(_w),h(_h),sx(_x){}
	rect_iteratorT& operator++(){
		x++;
		if (x>=sx+w) {
			y++;
			x=sx;
			h--;
		}
		return *this;
	}
	inline operator Point() const{return Point(x,y);}
	inline operator bool() const{return h>0;}
	inline Point operator *() const{return Point(x,y);}
};
typedef rect_iteratorT<int> rect_iterator;


template<typename T>
class wh_iteratorT {
	T w,h;
public:
	T x,y;
	wh_iteratorT(const T &_w,const T &_h):w(_w),h(_h){x=0;y=0;}
	wh_iteratorT(const Size &sz):w(sz.w),h(sz.h){x=0;y=0;}
	wh_iteratorT& operator++(){
		x++;
		if (x>=w) {
			y++;
			x=0;
		}
		return *this;
	}
	inline operator bool() const{return y<h;}
	inline operator Point() const{return Point(x,y);}
};
typedef wh_iteratorT<int> wh_iterator;

template<typename T1>
class edge_iteratorT {
public:
	int sx,sy;
	unsigned long c;
	int n;
	T1 &img;
	int count;
public:
	int x,y;
	edge_iteratorT(const T1 &_img,const int &_x,const int &_y):img(const_cast<T1&>(_img)),x(_x),y(_y){
		init();
	}
	edge_iteratorT(const T1 &_img,const Point &_p):img(const_cast<T1&>(_img)),x(_p.x),y(_p.y){
		init();
	}
	void init(){
		sx=x;sy=y;
		n=1;
		c=img.get(x,y);
		count=0;
		static const int dx[]={-1,-1,-1,0,1,1,1,0};
		static const int dy[]={-1,0,1,1,1,0,-1,-1};

		int cnt=0;
		while(img.get(x+dx[n],y+dy[n])==c){
			n=(n+1)&7;
			if(cnt++>=8) return;
		}
	}
	edge_iteratorT& operator++(){
		static const int dx[]={-1,-1,-1,0,1,1,1,0};
		static const int dy[]={-1,0,1,1,1,0,-1,-1};
		static const int nn[]={6,0,0,2,2,4,4,6};
		int cnt=0;
		while(img.get(x+dx[n],y+dy[n])!=c){
			n=(n+1)&7;
			//if(cnt++>=8) break;
		}
		x+=dx[n];y+=dy[n];
		//cout << x << "," << y << " ("<< (int)img(x,y).r << "(" << c << endl;
		//Sleep(100);
		n=nn[n];
		count++;
		return *this;
	}
	inline operator bool() const{return (count==0)||(x!=sx || y!=sy);}
	inline operator Point() const{return Point(x,y);}
	inline PIXEL& operator *() const{return img(x,y);}
};


template<typename T1>
class scan_iteratorT {
	int w,h;
	T1 &img;
public:
	int x,y;
	scan_iteratorT(const T1 &_img):w(_img.width),h(_img.height),img(const_cast<T1&>(_img)){x=0;y=0;}
	scan_iteratorT& operator++(){
		x++;
		if (x>=w) {
			y++;
			x=0;
		}
		return *this;
	}
	inline bool is_edge(){
		return x==0 || y==0 || x==w-1 || y==h-1;
	}
	inline operator bool() const{return y<h;}
	inline operator Point() const{return Point(x,y);}
	inline PIXEL& operator *() const{return img(x,y);}
};

typedef edge_iteratorT<Bitmap> edge_iterator;
typedef edge_iteratorT<BitmapN<unsigned char> > edge_iteratorB;
typedef edge_iteratorT<BitmapN<unsigned short> > edge_iteratorS;
typedef edge_iteratorT<BitmapN<unsigned long> > edge_iteratorL;
typedef scan_iteratorT<Bitmap> scan_iterator;
typedef scan_iteratorT<BitmapN<unsigned char> > scan_iteratorB;
typedef scan_iteratorT<BitmapN<unsigned short> > scan_iteratorS;
typedef scan_iteratorT<BitmapN<unsigned long> > scan_iteratorL;

#endif

