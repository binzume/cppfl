#ifndef _SPLASH_H
#define _SPLASH_H
#include <map>

namespace kwlib{


class SplashWindow :public Form{
private:
	void create(const char *caption,int w,int h,bool fshow){

		WNDCLASSEX wc;
		if (!GetClassInfoEx(Application.hInstance,FORMWINDOW_CLASS,&wc)) {
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
			wc.lpszClassName = FORMWINDOW_CLASS;
			if (!RegisterClassEx(&wc)) return;
		}
// | WS_EX_LAYERED | WS_EX_TRANSPARENT

		RECT rc={0,0,w,h};
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW&~(WS_THICKFRAME|WS_MAXIMIZEBOX),0);
		hWnd = CreateWindowEx(WS_EX_TOOLWINDOW  | WS_EX_LAYERED| WS_EX_TRANSPARENT , FORMWINDOW_CLASS,
							caption,
							WS_VISIBLE | WS_POPUP,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right-rc.left, rc.bottom-rc.top,
							NULL,
							(HMENU)NULL,
							Application.hInstance, 0);
	
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);

		fexist=true;
		if (fshow) show();
	}
public:
	SplashWindow(HWND hWnd_) {
		hWnd=hWnd_;
		fexist=false;
	}
	SplashWindow(const std::string &caption="",int w=320,int h=240,bool fshow=false) : Form(NULL){
		fexist=false;
		create(caption.c_str(),w,h,fshow);
		wrect.w=w; wrect.h=h;
	}
	void icon(HICON hIco) {
		SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIco );
		SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIco );
	}

	void show(bool f=true) {
		ShowWindow(hWnd,f?SW_SHOW:SW_HIDE);
		UpdateWindow(hWnd);
	}
	void hide() { show(false); }
	void showDialog() {
		//Application.run(*this);
	}
	void size(int w,int h) {
		RECT rc={0,0,w,h};
		AdjustWindowRect(&rc,GetWindowLong(hWnd,GWL_STYLE),0);
		SetWindowPos(hWnd,NULL,0,0,rc.right-rc.left, rc.bottom-rc.top,SWP_NOZORDER|SWP_NOMOVE);
	}
	void size(const Size &sz) {
		size(sz.w,sz.h);
	}
	void fixsize(bool f=true){
		long l=GetWindowLong(hWnd,GWL_STYLE);
		if (f) {
			l&=~ (WS_THICKFRAME|WS_MAXIMIZEBOX);
		} else {
			l|= (WS_THICKFRAME|WS_MAXIMIZEBOX);
		}
		SetWindowLong(hWnd,GWL_STYLE,l);
		RECT rc;
		GetClientRect(hWnd,&rc);
		size(rc.right,rc.bottom);
	}

	void font(Font &fnt){
		m_font.hFont=fnt.hFont;
		m_font.ownflag=fnt.ownflag;
		fnt.ownflag=false;
	}

	void menu(MainMenu *m) {
		m_menu=*m;
		SetMenu(hWnd,m_menu.hMenu);
	}
	void menu(const Menu &m) {
		m_menu=*(MainMenu*)&m;
		if (m.handler()) {
			eventListener.addref(Event::MENU, *m.handler());
		}
		SetMenu(hWnd,m_menu.hMenu);
	}

	template<typename FUNC>
	void onMenu(FUNC f){
		eventListener.add(Event::MENU, f);
	}
	template<typename FUNC,typename OBJ>
	void onMenu(FUNC f, OBJ* w){
		eventListener.add(Event::MENU, f,w);
	}

	void onMenu(MenuHandler &f,int){
		eventListener.addref(Event::MENU, f);
	}

	template<typename FUNC>
	void onClick(FUNC f){
		eventListener.add(Event::CLICK, f);
	}
	template<typename FUNC,typename OBJ>
	void onClick(FUNC f, OBJ* w){
		eventListener.add(Event::CLICK, f,w);
	}

	template<typename FUNC>
	void onPaint(FUNC f){
		eventListener.add(Event::PAINT, f);
	}
	template<typename FUNC,typename OBJ>
	void onPaint(FUNC f, OBJ* w){
		eventListener.add(Event::PAINT, f,w);
	}
	

	template<typename FUNC>
	void onEvent(FUNC f){
		eventListener.add(Event::ANYTHING, f);
	}
	template<typename FUNC,typename OBJ>
	void onEvent(FUNC f, OBJ* w){
		eventListener.add(Event::ANYTHING, f,w);
	}


	void colorkey(Color c) {
		SetLayeredWindowAttributes(hWnd, c.rgb, 0, LWA_COLORKEY);
	}


	MainMenu& menu() {
		return m_menu;
	}

	int msgBox(const std::string &s,const std::string &t=""){
		return MessageBox::msgBox(this,s.c_str(),t.c_str());
	}

	bool isexist() {
		return fexist;
	}
	virtual bool event(Event &ev){
		return false;
	}

	virtual LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
		Event ev;
		ev.hWnd=hWnd;
		ev.message=message;
		ev.wParam=wParam;
		ev.lParam=lParam;
		ev.target=this;
		ev.type=0;
		switch(message)
		{
			case WM_CREATE:
				break;
			case WM_PAINT:
				{
					ev.type=Event::PAINT;
					eventListener.call(ev);
				}
				break;
			case WM_COMMAND:
				if (!lParam) {
					ev.type=Event::MENU;
					ev.id = wParam;
					eventListener.call(ev);
				} else {
					Widget *w = (Widget*)GetWindowLongPtr((HWND)lParam ,GWLP_USERDATA);
					if (w) {
						ev.target = w;
						ev.type=Event::CLICK;
						ev.id = wParam;
						eventListener.call(ev);
					}
				}
				break;
			case WM_LBUTTONUP:
				{
					ev.type=Event::CLICK;
					eventListener.call(ev);
				}
				break;
			case WM_VSCROLL:
			case WM_HSCROLL:
				if ((HWND)lParam != hWnd) {
					Widget *w = (Widget*)GetWindowLongPtr((HWND)lParam ,GWLP_USERDATA);
					if (w) {
						ev.target = w;
						ev.type=Event::CHANGE;
						ev.id = wParam;
						w->event(&ev);
					}
				}
			break;
			case WM_SIZE:
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					onResize(rc.right,rc.bottom);
				}
				break;
			case WM_CLOSE:
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				fexist=false;
				return  0;
		}
		if (ev.type==0) {
			ev.type=Event::ANYTHING;
			eventListener.call(ev);
		}
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
};

}

#endif
