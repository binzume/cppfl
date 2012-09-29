#ifndef _WIDGET_H
#define _WIDGET_H
namespace kwlib{

typedef void (*CBFUNC)(class Widget*w,int i);

class Font{
public:
	HFONT hFont;
	int height;
	bool ownflag;
	Font(){
		hFont=NULL;
		ownflag=false;
	}
	Font(const std::string &name,int h,int angle=0){
		height=h;
		ownflag=true;
		hFont=CreateFont(h,0,angle,0,FW_REGULAR,FALSE,FALSE,FALSE,
			SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY,FIXED_PITCH | FF_MODERN,name.c_str());
	}
	~Font(){
		if (ownflag) DeleteObject(hFont);
	}
};

struct DockStyle{enum{
		None = 0,
		Top = 1,
		Bottom = 2,
		Left = 4,
		Right = 8,
		V = 3,
		H = 12,
		Fill = 15
};};


class Event
{
public:
	class Widget *target;
	int id;
	int type;
	HWND hWnd;
	union{
		UINT message,msg;
	};
	union{
		WPARAM wParam,wp;
	};
	union{
		LPARAM lParam,lp;
	};
	const static int ANY = 0;
	const static int CLICK = 1;
	const static int MENU = 2;
	const static int CHANGE = 3;
	const static int PAINT = 4;
	const static int ANYTHING = 5;
	const static int EMAX = 6;
};
typedef bool (*EVENT_LISTENER)(Event &e);

class EventListenerFuncBase
{
public:
	virtual bool operator() (Event)=0;
};

template<typename F>
class EventListenerFunc : public EventListenerFuncBase {
public:
	F func;
	EventListenerFunc(F f): func(f){}
	bool operator() (Event e){
		return func(e);
	}
};

template<typename F,typename C>
class EventListenerFunc2 : public EventListenerFuncBase {
public:
	F func;
	C obj;
	EventListenerFunc2(F f,C o):func(f),obj(o){}
	bool operator() (Event e){
		return (obj->*func)(e);
	}
};

template<typename F>
class EventListenerFuncRef : public EventListenerFuncBase {
public:
	F &func;
	EventListenerFuncRef(F &f): func(f){}
	bool operator() (Event e){
		return func(e);
	}
};

class EventListeners
{
	// 数が増えたらmapにする
//	std::map<int,EventListenerFuncBase*> listeners;
	EventListenerFuncBase* listeners[Event::EMAX];
public:
	EventListeners(){
		for (int i=0;i<Event::EMAX; i++) listeners[i]=NULL;
	}
	~EventListeners(){
		for (int i=0;i<Event::EMAX; i++) if(listeners[i]) delete listeners[i];
	}
	template <typename T>
	void add(int event,T listener){
		if(listeners[event]) delete listeners[event];
		listeners[event] = new EventListenerFunc<T>(listener);
	}

	template <typename T>
	void addref(int event,T &listener){
		if(listeners[event]) delete listeners[event];
		listeners[event] = new EventListenerFuncRef<T>(listener);
	}


	template <typename T>
	void add(int event,T* listener){
		if(listeners[event]) delete listeners[event];
		if (listener!=NULL) {
			listeners[event] = new EventListenerFunc<T*>(listener);
		} else {
			listeners[event] = NULL;
		}
	}

	template <typename T,typename C>
	void add(int event,T listener,C obj){
		if(listeners[event]) delete listeners[event];
		listeners[event] = new EventListenerFunc2<T,C>(listener,obj);
	}
	void remove(int event){
		if(listeners[event]) delete listeners[event];
		listeners[event] = NULL;
	}
	bool call(Event &e){
		if (listeners[e.type])return (*listeners[e.type])(e);
		return false;
	}
};


class Widget{
protected:
	Font m_font;
public:
	Rect wrect;
	HWND hWnd;
	int malign;
	//enum ALIGN {
	static const int
		NONE = 0,
		FIT_TOP = 1,
		FIT_BOTTOM  = 2,
			FIT_V = 3,
		FIT_LEFT = 4,
		FIT_RIGHT = 8,
			FIT_H = 12,
			FIT = 15,
		CENTER_V = 16,
		CENTER_H = 32,
			CENTER = 48;
	//};
	virtual bool event(Event *e){return false;}
	Widget(){hWnd=NULL;malign=0;}
	virtual ~Widget(){
		if (m_font.hFont) {
			SendMessage(hWnd, WM_SETFONT, (WPARAM)NULL, TRUE);
		}
	}
	Point pointToClient(const Point &p) {
		POINT pt;
		pt.x=p.x; pt.y=p.y;
		ScreenToClient(hWnd,&pt);
		return Point(pt.x,pt.y);
	}
	Point cursorPos(){
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hWnd,&pt);
		return Point(pt.x,pt.y);
	}
	void align(int i){malign=i;}
	void dock(int i){malign=i;}
	int dock(){return malign;}
	void locate(HWND parent,int x,int y) {
		wrect.x=x; wrect.y=y;
		SetParent(hWnd,parent);
		location(x,y);
	}
	void parent(Widget &parent){
		SetParent(hWnd,parent.hWnd);
	}
	void parent(Widget *parent){
		SetParent(hWnd,parent->hWnd);
	}
	Widget& parent(){
		return *(Widget*)GetWindowLongPtr(GetParent(hWnd),GWLP_USERDATA);
	}
	void location(int x,int y){
		SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	void enable(bool f){
		EnableWindow(hWnd,f?TRUE:FALSE);
	}
	void text(const std::string &s){
		SetWindowText(hWnd,s.c_str());
	}
	std::string text(){
		char buf[1024];
		GetWindowText(hWnd,buf,sizeof(buf));
		return buf;
	}
	virtual void size(int w,int h){
		wrect.w=w; wrect.h=h;
		SetWindowPos(hWnd,NULL,0,0,w,h,SWP_NOMOVE|SWP_NOZORDER);
	}
	virtual Size size(){
		Size sz;
		sz.w=wrect.w;
		sz.h=wrect.h;
		return sz;
	}
	void size(Size &sz){
		size(sz.w,sz.h);
	}
	void font(Font *font, bool ownflag=false){
		SendMessage(hWnd, WM_SETFONT, (WPARAM)font->hFont, TRUE);
		m_font.hFont=font->hFont;
		m_font.ownflag=ownflag;
	}
	void font(Font &fnt){
		font(&fnt,fnt.ownflag);
		fnt.ownflag=false;
	}
	void font(const std::string &name, int height){
		font(&Font(name,height),true);
	}
	
	void topmost(bool f){
		SetWindowPos( hWnd, f?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE) );
	}
	
	void close(){
		DestroyWindow(hWnd);
	}
	int id(){
		return (int)GetMenu(hWnd);
	}

	virtual LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
		return DefWindowProc(hWnd,message,wParam,lParam);
	}

	static LRESULT CALLBACK WindowProc1(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
		Widget *_this = (Widget*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
		if (_this) return _this->WindowProc(hWnd,message,wParam,lParam);
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
	WNDPROC subclass(){
		return (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc1);
	}

	void update(int x,int y,int w,int h) {
		RECT r={x,y,x+w,y+h};
		InvalidateRect(hWnd, &r, FALSE);
		UpdateWindow(hWnd);
	}
	void update() {
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
	}

};

class Container: public Widget{
protected:
	int curx,cury;
	std::vector<Widget*> child;
public:
	Container(){
		curx=cury=0;
	}
	void pos(int x,int y){
		curx=x;
		cury=y;
	}
	void add(Widget *w,int x=-1,int y=-1) {
		if (x<0) x=curx;
		if (y<0) y=cury;
		w->locate(hWnd,x,y);
		RECT rc;
		GetClientRect(w->hWnd,&rc);
		curx=x;
		cury=y+rc.bottom;
		child.push_back(w);
	}
	void movepos(int dx,int dy) {
		curx+=dx; cury+=dy;
	}
	void add(Widget &w,int x=-1,int y=-1) {
		add(&w,x,y);
	}
	Widget* find(int id){
		HWND hwnd=GetDlgItem(hWnd,id);
		if (hwnd) {
			Widget* w=(Widget*)GetWindowLongPtr(hwnd ,GWLP_USERDATA);
			if (w) return w;
		}
		return NULL;
	}
	void layout() {
		RECT rc;
		GetWindowRect(hWnd,&rc);
		onResize(rc.right-rc.left,rc.bottom-rc.top);
	}
	void onResize(int w,int h) {
		int i,l=child.size(),a;
		for (i=0;i<l;i++) {
			a=child[i]->dock();
			Size s=child[i]->size();
			if (a) {
				if (a&FIT_RIGHT) {
					s.w=w-child[i]->wrect.x;
				}
				if (a&FIT_BOTTOM) {
					s.h=h-child[i]->wrect.y;
				}
				//if (a==(FIT_V|FIT_RIGHT)) w-=s.w;
				child[i]->size(s.w,s.h);
				if (a==(FIT_H|FIT_BOTTOM)) h-=child[i]->size().h;
			}
			else if (child[i]->wrect.x<0 || child[i]->wrect.y<0) {
				int x=child[i]->wrect.x, y=child[i]->wrect.y;
				if (x<0) x=w-x;
				if (y<0) y=h-y;
				child[i]->location(x,y);
			}
		}
	}
};

class WidgetTable: public Container{
	int ww;
	WNDPROC OrgProc;
	std::vector<int> cw;
public:
	WidgetTable(int w=2){
		ww=w;
		cw.resize(w);
		hWnd = CreateWindow(
				("STATIC"),"",WS_CHILD|WS_VISIBLE|SS_LEFT,
				0, 0,100, 100, HWND_MESSAGE,
				(HMENU)NULL,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		OrgProc = subclass();
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
		switch(message)
		{
			case WM_SIZE:
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					onResize(rc.right,rc.bottom);
				}
				break;
		}
		return CallWindowProc(OrgProc,hWnd,message,wParam,lParam);
	}
	void onResize(int w,int h) {
		int i,l=child.size(),a;
		int r=w,d=0;
		for (i=0;i<ww && i<l;i++) {
			if (cw[i]>0) {
				r-=cw[i];
			} else {
				d++;
			}
		}
		if (r<0) r=2;
		int x=0;
		for (i=0;i<l;i++) {
			a=child[i]->dock();
			if (a&FIT_H) {
				Size s=child[i]->size();
				if (cw[i%ww]>0)
					s.w=cw[i%ww];
				else
					s.w=r/d;
				child[i]->size(s.w,h);
			}
			child[i]->location(x,0);
			if (cw[i%ww]>0)
				x+=cw[i%ww];
			else
				x+=r/d;
		}
	}
	void cmode(int c,int m) {
		cw[c]=m;
	}
};


typedef void (Widget::*CBFUNC2)(int i);
typedef void (*CBFUNC3)(int i);

class Button: public Widget{
protected:
	EventListeners eventListener;
public:
	Button(HWND parent,const std::string &s="",int i=0,EVENT_LISTENER f=NULL){
		eventListener.add(Event::CLICK, f);
		hWnd = CreateWindow(
				("BUTTON"),s.c_str(),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 0,80, 24, parent,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}

	template<typename FUNC>
	Button(const std::string &s, FUNC f, int i=0){
		eventListener.add(Event::CLICK, f);
		hWnd = CreateWindow(
				("BUTTON"),s.c_str(),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}

	Button(const std::string &s="",int i=0,EVENT_LISTENER f=NULL){
		eventListener.add(Event::CLICK, f);
		hWnd = CreateWindow(
				("BUTTON"),s.c_str(),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}

	void bitmap(HBITMAP hBmp) {
		SendMessage( hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp );
	}
	HBITMAP bitmap() {
		return (HBITMAP)SendMessage( hWnd, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)0 );
	}


	template<typename FUNC>
	void onClick(FUNC f){
		eventListener.add(Event::CLICK, f);
	}
	template<typename FUNC,typename C>
	void onClick(FUNC f, C* w){
		eventListener.add(Event::CLICK, f,w);
	}
	bool event(Event *e) {
		e->type=Event::CLICK;
		return eventListener.call(*e);
	}
};
class TextBox: public Widget{
	CBFUNC cb;
public:
	TextBox(const std::string &s="",int i=0,bool ml=false,CBFUNC f=NULL){
		cb = f;
		hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
				("EDIT"),s.c_str(),WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL| WS_BORDER|(ml?ES_MULTILINE:0),
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	int integer() {
		return atoi(text().c_str());
	}
	bool event(Event *e) {
		if (cb) cb(this,e->id);
		return false;
	}
};

class Label: public Widget{
public:
	Label(const std::string &s="",int i=0){
		hWnd = CreateWindow(
				("STATIC"),s.c_str(),WS_CHILD|WS_VISIBLE|SS_LEFT,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
};

class CheckBox: public Widget{
public:
	CheckBox(const std::string &s="",int i=0){
		hWnd = CreateWindow(
				("BUTTON"),s.c_str(),WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	CheckBox(Widget &parent,const std::string &s="",int i=0){
		hWnd = CreateWindow(
				("BUTTON"),s.c_str(),WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
				0, 0,80, 24, parent.hWnd,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}

	void check(bool f){
		SendMessage(hWnd , BM_SETCHECK , f?BST_CHECKED:BST_UNCHECKED , 0);
	}
	bool check(){
		return SendMessage(hWnd , BM_GETCHECK , 0 , 0) == BST_CHECKED;
	}

};

class PictureBox: public Widget{
	EventListeners eventListener;
public:
	PictureBox(HBITMAP hBmp=NULL,int i=0){
		hWnd = CreateWindow(
				("STATIC"),"",WS_CHILD|WS_VISIBLE|SS_BITMAP,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		if (hBmp) bitmap(hBmp);
	}
	void bitmap(HBITMAP hBmp) {
		SendMessage( hWnd, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp );
	}
	HBITMAP bitmap() {
		return (HBITMAP)SendMessage( hWnd, STM_GETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)0 );
	}
	void update(){
		InvalidateRect(hWnd,NULL,FALSE);
	}


	template<typename FUNC>
	void onClick(FUNC f){
		long l=GetWindowLong(hWnd,GWL_STYLE);
		SetWindowLong(hWnd,GWL_STYLE,f|SS_NOTIFY);
		eventListener.add(Event::CLICK, f);
	}
	template<typename FUNC,typename C>
	void onClick(FUNC f, C* w){
		long l=GetWindowLong(hWnd,GWL_STYLE);
		SetWindowLong(hWnd,GWL_STYLE,f|SS_NOTIFY);
		eventListener.add(Event::CLICK, f,w);
	}
	bool event(Event *e) {
		e->type=Event::CLICK;
		return eventListener.call(*e);
	}
};

class ListBox: public Widget{
public:
	ListBox(const std::string &s="",int i=0){
		hWnd = CreateWindow( // LBS_NOTIFY
				("LISTBOX"),s.c_str(),WS_CHILD|WS_VISIBLE|(LBS_STANDARD& ~LBS_SORT),
				0, 0,80, 24*4, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	void add(std::string s) {
			SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)s.c_str());
	}
	int sel(){
		return SendMessage(hWnd,LB_GETCURSEL,0,0);
	}
};

class ListView: public Widget{
public:
	ListView(const std::string &s="",int i=0){
		hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
				WC_LISTVIEW,s.c_str(),WS_CHILD|WS_VISIBLE|LVS_REPORT| WS_BORDER,
				0, 0,80, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		DWORD style = ListView_GetExtendedListViewStyle(hWnd);
		style |= LVS_EX_FULLROWSELECT |LVS_EX_HEADERDRAGDROP ;
		ListView_SetExtendedListViewStyle(hWnd, style);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	void add(char **s,int n) {
		LVITEM item = {0};
		int i,k=0;
		for (i=0;i<n;i++) {
		item.mask = LVIF_TEXT;
		item.pszText = s[i];
		item.iItem = k;
		item.iSubItem = i;
			if (i==0) k=ListView_InsertItem(hWnd, &item);
			else ListView_SetItem(hWnd, &item);
		}
	}
	void clear(){
		ListView_DeleteAllItems(hWnd);
	}
	void addCol(LVCOLUMN *c){
		ListView_InsertColumn(hWnd, 0, c);
	}
	void addCol(const std::string &s){
		LVCOLUMN col;
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 100;
		col.pszText = (LPSTR)s.c_str();
		addCol(&col);
	}
	void addCol(char **s,int n) {
		LVCOLUMN col;
		int i;
		for (i=0;i<n;i++) {
			col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
			col.fmt = LVCFMT_LEFT;
			col.cx = 100;
			col.pszText = (LPSTR)s[i];
			ListView_InsertColumn(hWnd, i, &col);
		}
	}
	int sel(){
		return SendDlgItemMessage(hWnd,1,LB_GETCURSEL,0,0);
	}
};

class ComboBox: public Widget{
public:
	ComboBox(const std::string &s="",int i=0){
		hWnd = CreateWindow(
				("COMBOBOX"),s.c_str(),WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
				0, 0,80, 24*5, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	ComboBox(HWND parent,const std::string &s="",int i=0){
		hWnd = CreateWindow(
				("COMBOBOX"),s.c_str(),WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
				0, 0,80, 24*5, parent,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	void add(std::string s) {
		SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)s.c_str());
	}
	int sel(){
		return SendMessage(hWnd,CB_GETCURSEL,0,0);
	}
	void sel(int c){
		SendMessage(hWnd,CB_SETCURSEL,c,0);
	}
};

class Slider: public Widget{
	EventListeners eventListener;
public:
	const static bool HORIZONTAL = false;
	const static bool VERTICAL = true;
	Slider(bool vflag=false,int i=0){
		hWnd = CreateWindow(
				("msctls_trackbar32"),"",WS_CHILD|WS_VISIBLE|(vflag?TBS_VERT:TBS_HORZ),
				0, 0,160, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	Slider(HWND parent,bool vflag=false,int i=0){
		int w,h;
		if (vflag) {
			w=24; h=160;
		} else {
			w=160; h=24;
		}
		hWnd = CreateWindow(
				("msctls_trackbar32"),"",WS_CHILD|WS_VISIBLE|(vflag?TBS_VERT:TBS_HORZ),
				0, 0, w, h, parent,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		range(0,255);
	}
	Slider(Widget &parent,bool vflag=false,int i=0){
		int w,h;
		if (vflag) {
			w=24; h=160;
		} else {
			w=160; h=24;
		}
		hWnd = CreateWindow(
				("msctls_trackbar32"),"",WS_CHILD|WS_VISIBLE|(vflag?TBS_VERT:TBS_HORZ),
				0, 0, w, h, parent.hWnd,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		range(0,255);
	}
	void range(int min,int max){
		SendMessage(hWnd,TBM_SETRANGE,1,MAKELPARAM(min,max));
	}
	int pos(){
		return SendMessage(hWnd,TBM_GETPOS,0,0);
	}
	void pos(int c){
		SendMessage(hWnd,TBM_SETPOS,1,c);
	}


	template<typename FUNC>
	void onChange(FUNC f){
		eventListener.add(Event::CHANGE, f);
	}
	bool event(Event *e) {
		return eventListener.call(*e);
	}
};

class ProgressBar: public Widget{
public:
	ProgressBar(bool vflag=false,int i=0){
		hWnd = CreateWindow(
				("msctls_progress32"),"",WS_CHILD|WS_VISIBLE|(vflag?PBS_VERTICAL:0) | PBS_SMOOTH ,
				0, 0,160, 24, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	ProgressBar(HWND parent,bool vflag=false,int i=0){
		hWnd = CreateWindow(
				("msctls_progress32"),"",WS_CHILD|WS_VISIBLE|(vflag?PBS_VERTICAL:0) | PBS_SMOOTH ,
				0, 0,160, 24, parent,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	void range(int min,int max){
		SendMessage(hWnd,PBM_SETRANGE,0,MAKELPARAM(min,max));
	}
	int pos(){
		return SendMessage(hWnd,PBM_GETPOS,0,0);
	}
	void pos(int c){
		SendMessage(hWnd,PBM_SETPOS,c,0);
	}
	void step(int c){
		SendMessage(hWnd,PBM_SETSTEP,c,0);
	}
};

class Rebar: public Widget{
	REBARINFO rbi; 
public:
	Rebar(HWND parent=HWND_MESSAGE,int i=0){
		hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | RBS_BANDBORDERS | WS_CLIPCHILDREN | CCS_NODIVIDER,
				 0, 0, 200, 20, parent, (HMENU)(INT_PTR)i, GetModuleHandle(NULL), NULL); 
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		ZeroMemory(&rbi, sizeof(REBARINFO));
		rbi.cbSize = sizeof(REBARINFO);
		SendMessage(hWnd, RB_SETBARINFO, 0, (LPARAM)&rbi); 
		dock(DockStyle::H);
	}
	void add(Widget &w,const std::string &s="") {
		w.locate(hWnd,0,0);
		REBARBANDINFO rbBand;
		ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
		rbBand.fStyle = RBBS_CHILDEDGE;
		rbBand.cxMinChild = 2;
		rbBand.cyMinChild = 25;
		rbBand.lpText = (char*)s.c_str();
		rbBand.hwndChild = w.hWnd;
		rbBand.cx = 100;
		SendMessage(hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand); 
	}

	void parentresize(){
	}
};

class Spliter: public Widget{
	bool vflag;
	Widget *w1,*w2;
	WNDPROC OrgProc;
	int mpos;
	int m_mode;
	Size sz;
	int barwidth;
public:
	Spliter(int i=0,bool vf=true){
		sz.w=100; sz.h=100;
		hWnd = CreateWindow(
				("STATIC"),"",WS_CHILD|WS_VISIBLE|SS_LEFT,
				0, 0,sz.w, sz.h, HWND_MESSAGE,
				(HMENU)(INT_PTR)i,                      // メニューハンドル(ID)
				GetModuleHandle(NULL),NULL);
		OrgProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc1);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		vflag=vf;
		w1=w2=NULL;
		mpos=sz.w/2;
		barwidth=2;
		m_mode=0;
	}
	LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
		switch(message)
		{
			case WM_NCHITTEST:
				return HTCLIENT;
			case WM_SETCURSOR:
				break;
			case WM_LBUTTONDOWN:
				SetCursor(LoadCursor(NULL,vflag?IDC_SIZEWE:IDC_SIZENS));
				SetCapture(hWnd);
				break;
			case WM_LBUTTONUP:
				ReleaseCapture();
				break;
			case WM_MOUSEMOVE:
				SetCursor(LoadCursor(NULL,vflag?IDC_SIZEWE:IDC_SIZENS));
				if(wParam & MK_LBUTTON){
					int p=0;
					if (vflag) {
						p=LOWORD(lParam)-barwidth/2;
					} else {
						p=HIWORD(lParam)-barwidth/2;
					}
					if (p!=mpos) pos(p);
				}
				break;
		}

		return CallWindowProc(OrgProc,hWnd,message,wParam,lParam);
	}
	void add(Widget *w) {
		w->locate(hWnd,0,0);
		if (!w1) {
			w1=w;
			return;
		}
		if (!w2) {
			w2=w;
			pos(mpos);
			return;
		}
	}
	void mode(int m){
		m_mode=m;
		layout();
	}
	int pos() {
		return mpos;
	}
	void layout(){
		if (!w1 || !w2) return;
		if (m_mode==1) {
			w1->location(0,0);
			w1->size(sz.w,sz.h);
			w2->locate(hWnd,0,sz.h);
			return;
		} else if (m_mode==2) {
			w2->location(0,0);
			w2->size(sz.w,sz.h);
			w1->locate(hWnd,0,sz.h);
			return;
		}
		w1->location(0,0);
		if (vflag) {
			w1->size(mpos,sz.h);
			w2->location(mpos+barwidth,0);
			w2->size(sz.w-mpos-barwidth,sz.h);
		} else {
			w1->size(sz.w,mpos);
			w2->location(0,mpos+barwidth);
			w2->size(sz.w,sz.h-mpos-barwidth);
		}
	}
	void size(int w,int h) {
		Widget::size(w,h);
		int p=0;
		if (vflag)
			p = sz.w?w*mpos/sz.w:20;
		else
			p = sz.h?h*mpos/sz.h:20;
		sz.w=w;
		sz.h=h;
		pos(p);
		layout();
	}
	void pos(int p) {
		if (p<20) p=20;
		if (vflag) {
			if (p>sz.w-20) p=sz.w-20;
		} else {
			if (p>sz.h-20) p=sz.h-20;
		}
		if (p==mpos) return;
		mpos = p;
		layout();
		UpdateWindow(hWnd);
	}
};


class StatusBar: public Widget{
	int height;
public:
	StatusBar(char **str=NULL,int i=0){
		hWnd = CreateStatusWindow(
				WS_CHILD|WS_VISIBLE | SBARS_SIZEGRIP,
				"", HWND_MESSAGE,
				i);                      // メニューハンドル(ID)
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)this);
		dock(DockStyle::H | DockStyle::Bottom);
	}
	void parts(int widths[],int n) {
		SendMessage(hWnd, SB_SETPARTS, (WPARAM)n, (LPARAM)widths);
	}
	void text(int n,const std::string &s){
		SendMessage(hWnd, SB_SETTEXT, (WPARAM)n, (LPARAM)s.c_str());
	}
	void sizeGrip(bool f) {
		long l=GetWindowLong(hWnd,GWL_STYLE);
		SetWindowLong(hWnd,GWL_STYLE,f?l|SBARS_SIZEGRIP:l&~SBARS_SIZEGRIP);
	}
	Size size(){
		RECT rc;
		GetWindowRect(hWnd,&rc);
		return Size(rc.right,rc.bottom-rc.top);
	}
};



}


#endif

