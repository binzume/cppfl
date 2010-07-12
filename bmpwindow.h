#ifndef _WINDOW_H
#define _WINDOW_H

#define IMGWINDOW_CLASS "kwImageWindow"
#define WINDOW_CLASS "kwWindow"


template<typename T>
class PenEx : public DCPen {
	T *win;
	bool fupdate;
public:
	using Pen::color;
	using Pen::line;
	using Pen::circle;
	using Pen::circlef;
	using Pen::box;
	using Pen::boxf;
	using Pen::move;
	PenEx(){
		win = NULL;
		fupdate=false;
	}
//	void font(Font *font,bool ownflag=false){
//		SelectObject(hDC, font->hFont);
//		mfont=ownflag?font:NULL;
//	}

	PenEx(T *w):DCPen(w->hdc, w->dib->width, w->dib->height){
		win = w;
		fupdate=true;
	}
	PenEx(T &w):DCPen(w.dib->hdc, w.dib->width, w.dib->height){
		win = &w;
		fupdate=true;
	}
	void update(){
		if (win) win->update();
	}
	void update(int x,int y,int w,int h){
		if (win) win->update(x,y,w,h);
	}

	void update(bool f){ fupdate=f; }

	void text(const std::string &s) {
		DCPen::text(s);
		if (fupdate) update();
	}
	void print(const std::string &s) {
		DCPen::print(s);
		if (fupdate) update();
	}
	void line(int x,int y,int x2,int y2) {
		DCPen::line(x,y,x2,y2);
		if (fupdate) update();
	}
	void box(int x,int y,int w,int h) {
		DCPen::box(x,y,w,h);
		if (fupdate) update(x,y,w,h);
	}
	void boxf(int x,int y,int w,int h) {
		DCPen::boxf(x,y,w,h);
		if (fupdate) update(x,y,w,h);
	}
	void circle(int cx,int cy,int r) {
		DCPen::circle(cx,cy,r);
		if (fupdate) update(cx-r,cy-r,r*2+1,r*2+1);
	}
	void circlef(int cx,int cy,int r) {
		DCPen::circlef(cx,cy,r);
		if (fupdate) update(cx-r,cy-r,r*2+1,r*2+1);
	}
};

class BitmapWindow :public Form,public PenEx<BitmapWindow>{
	bool fitwindow;
	POINT pos;
public:
	HDC hdc;
	DIBitmap *dib;
	PenEx<BitmapWindow> *mpen;
	void fit(bool f) {fitwindow=f;}
	BitmapWindow(char *caption,int w,int h,bool fshow=true):Form(NULL) {
		pos.x=0;pos.y=0;
		fitwindow=false;
		WNDCLASSEX wc;
	
		if (!GetClassInfoEx(Application.hInstance,IMGWINDOW_CLASS,&wc)) {
			// ウインドウクラスを登録
			wc.cbSize		= sizeof(wc);
			wc.lpfnWndProc	= WindowProc1;
			wc.style		= CS_HREDRAW | CS_VREDRAW;
			wc.cbClsExtra	= 0;
			wc.cbWndExtra	= 0;
			wc.hInstance	= Application.hInstance;
			wc.hIcon		= NULL;
			wc.hCursor		= LoadCursor(NULL,IDC_ARROW);
			wc.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
			wc.hIconSm		= NULL;
			wc.lpszMenuName	= NULL;
			wc.lpszClassName = IMGWINDOW_CLASS;
			if (!RegisterClassEx(&wc)) return;
		}
		
		RECT rc={0,0,w,h};
		AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW|WS_CAPTION,FALSE);
		hWnd = CreateWindow(IMGWINDOW_CLASS,caption,
							WS_OVERLAPPEDWINDOW | WS_CAPTION, //WS_CHILD
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right-rc.left, rc.bottom-rc.top,
							NULL,
							(HMENU)NULL,
							Application.hInstance, 0);
	
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);

		dib = new DIBitmap(w,h);
		dib->create(w,h,24);
		dib->clearWhite();
		hdc=CreateCompatibleDC(NULL);
		SelectObject(hdc,dib->dib);
		
		fexist=true;
		if (fshow) show();
		
		mpen = new PenEx<BitmapWindow>(this);
	}
	~BitmapWindow() {
		if (fexist) {
			DestroyWindow(hWnd);
		}
		delete dib;
	}
	

	LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

		static PAINTSTRUCT ps;
		static POINT pt;

		switch(message)
		{
			case WM_CREATE:
				break;
			case WM_COMMAND:
				if (lParam) {
					Widget *w = (Widget*)GetWindowLongPtr((HWND)lParam ,GWLP_USERDATA);
					Event ev;
					ev.hWnd=hWnd;
					ev.message=message;
					ev.wParam=wParam;
					ev.lParam=lParam;
					ev.id = wParam;
					ev.target = w;
					if (w) {
						w->event(&ev);
					}
				}
				break;
			case WM_PAINT:
				{
					HDC hDC = BeginPaint (hWnd, &ps);
					RECT rt;
					int x=0,y=0;
					GetClientRect( hWnd, &rt );
					//FillRect(hDC,&rt,(HBRUSH)(COLOR_WINDOW + 1));
	
					if (fitwindow) { // ウインドウに合わせる
						int w,h;
						w = rt.right;
						h = rt.bottom;
						if (w*dib->height>h*dib->width) {
							h = h*rt.right/w;
						}
						if (w*dib->height<h*dib->width) {
							h = h*rt.right/w;
						}

						// アスペクト比
/*
						w = dib->width;
						h = dib->height;
						if (h > rt.bottom) {
							w = w*rt.bottom/h;
							h = rt.bottom;
						}
						if (w > rt.right) {
							h = h*rt.right/w;
							w = rt.right;
						}
						if (rt.right>w)
							x=(rt.right-w)/2;
						if (rt.bottom>h)
							y=(rt.bottom-h)/2;
*/
						SetStretchBltMode(hDC,STRETCH_HALFTONE);
						StretchBlt(hDC,x,y,w,h, hdc,0,0,dib->width,dib->height,SRCCOPY);
					} else {
						if(rt.right>dib->width){
							x=(rt.right-dib->width)>>1;
						}
						if(rt.bottom>dib->height){
							y=(rt.bottom-dib->height)>>1;
						}
						BitBlt(hDC,x,y,rt.right,rt.bottom,hdc,pos.x,pos.y,SRCCOPY);
					}
					EndPaint( hWnd, &ps );
				}
				break;
			case WM_LBUTTONDOWN:
				pt.x=LOWORD(lParam);
				pt.y=HIWORD(lParam);
				SetFocus(hWnd);
				SetCapture(hWnd);
				break;
			case WM_LBUTTONUP:
				ReleaseCapture();
				{
					Event ev;
					ev.hWnd=hWnd;
					ev.message=message;
					ev.wParam=wParam;
					ev.lParam=lParam;
					ev.target=this;
					ev.type=Event::CLICK;
					eventListener.call(ev);
				}
				break;
			//case WM_MOUSEMOVE:
			//	mouse.x=LOWORD(lParam);
			//	mouse.y=HIWORD(lParam);
			//	mousef=true;
			//	break;
			//case WM_MOUSELEAVE:
			//	mousef=false;
			//	break;
			case WM_DESTROY:
				PostQuitMessage(0);
				fexist=false;
				return  0;
		}
		return Form::WindowProc(hWnd,message,wParam,lParam);
	}
	void update(){
		Form::update();
	}
	void update(int x,int y,int w,int h){
		RECT rt;
		GetClientRect( hWnd, &rt );
		if(rt.right>dib->width){
			x+=(rt.right-dib->width)>>1;
		}
		if(rt.bottom>dib->height){
			y+=(rt.bottom-dib->height)>>1;
		}
		rt.left=x; rt.top=y;
		rt.right=x+w; rt.bottom=y+h;
		InvalidateRect(hWnd, &rt, FALSE);
//		UpdateWindow(hWnd);
	}
	DCPen *pen(){
		return new PenEx<BitmapWindow>(this);
	}

	void wait(){
		Application.run(*this);
	}

	using PenEx::color;
	using PenEx::line;
//	using Pen::circle;
//	using Pen::circlef;
	using PenEx::box;
	using PenEx::boxf;
	using PenEx::move;
	using PenEx::update;

	void text(const std::string &s) {
		Form::text(s);
	}

	void update(bool f) {
		mpen->update(f);
	}
	void puttext(const std::string &s) {
		mpen->text(s);
	}
	void print(const std::string &s) {
		mpen->print(s);
	}
	void line(int x,int y,int x2,int y2) {
		mpen->line(x,y,x2,y2);
	}
	void box(int x,int y,int w,int h) {
		mpen->box(x,y,w,h);
	}
	void boxf(int x,int y,int w,int h) {
		mpen->boxf(x,y,w,h);
	}
	void circle(int cx,int cy,int r) {
		mpen->circle(cx,cy,r);
	}
	void circlef(int cx,int cy,int r) {
		mpen->circlef(cx,cy,r);
	}
	void color(int r,int g,int b) {
		mpen->color(r,g,b);
	}

};

#endif
