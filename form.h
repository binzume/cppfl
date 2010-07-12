#ifndef _FORM_H
#define _FORM_H
namespace kwlib{

#define FORMWINDOW_CLASS "kwuiFormWindow"

namespace MessageBox{
	static int msgBox(const std::string &s,const std::string &t=""){
		return ::MessageBox(NULL,s.c_str(),t.c_str(),MB_OK);
	}
	static int msgBox(const class Widget *w,const std::string &s,const std::string &t=""){
		return ::MessageBox(w->hWnd,s.c_str(),t.c_str(),MB_OK);
	}
};
typedef Event EventData;

template<typename T>
class MenuContainer{
public:
	HMENU hMenu;
	MenuContainer(){
		hMenu = CreatePopupMenu();
	}
	MenuContainer(HMENU m){
		hMenu = m;
	}
	~MenuContainer(){
		//if (hMenu) DestroyMenu(hMenu);
	}
	HMENU handle(){return hMenu;}
	MenuContainer& add(const std::string &s,int id) {
		if (!hMenu) CreatePopupMenu();
		AppendMenu(hMenu, MF_ENABLED | MF_STRING ,id, s.c_str());
		return *this;
	}
	MenuContainer& add(const std::string &s,const MenuContainer<T> &m){
		if (!hMenu) CreatePopupMenu();
		AppendMenu(hMenu, MF_ENABLED | MF_STRING | MF_POPUP , (UINT)m.hMenu, s.c_str());
		return *this;
	}



	MenuContainer& add(T &mi){
		if (!hMenu) CreatePopupMenu();
		if (mi.hMenu)
			AppendMenu(hMenu, mi.mii.fState | MF_POPUP, (UINT)mi.hMenu, mi.mii.dwTypeData);
		else
			InsertMenuItem(hMenu,mi.mii.wID,FALSE,&mi.mii);
	//		AppendMenu(hMenu, mi.flags , mi.mid, mi.mtext.c_str());
		mi.parent(this);
		return *this;
	}

	T& get(T &mi,int mid){
		char s[256];
		mi.mii.dwTypeData=s;
		mi.mii.cch=sizeof(s);
		GetMenuItemInfo(hMenu,mid,FALSE,&mi.mii);
		mi.parent(this);
		mi.text(s);
		return mi;
	}

	T get(int mid){T mi;return get(mi,mid);}

	operator HMENU(){return hMenu;};
};

typedef MenuContainer<class MenuItem> Menu;
class MenuItem : public Menu{
public:
	MENUITEMINFO mii;
	std::string mtext;
	MenuContainer *m_parent;
	MenuItem(const std::string &s="",int id=0) : Menu(NULL){
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STATE  | MIIM_ID | MIIM_TYPE;
		mii.fState=0;
		mii.wID=id;
		m_parent=NULL;
		text(s);
	}
	void text(const std::string &s){
		mtext=s;
		mii.fType = MFT_STRING;
		mii.cch = mtext.length();
		mii.dwTypeData = (char*)mtext.c_str();
		if (mii.cch==1 && mii.dwTypeData[0]=='-') {
			//mii.fMask|=MIIM_TYPE;
			mii.fType|=MFT_SEPARATOR;
		}
		update();
	}
	std::string& text(){return mtext;};
	void checked(bool f){
		mii.fState=f?mii.fState|MFS_CHECKED:mii.fState&~MFS_CHECKED;
		update();
	}
	bool checked(){
		return (mii.fState&MFS_CHECKED)!=0;
	}
	void enabled(bool f){
		mii.fState=f?mii.fState&~MFS_DISABLED:mii.fState|MFS_DISABLED;
		update();
	}
	bool enabled(){
		return (mii.fState&MFS_DISABLED)==0;
	}
	void defaultItem(bool f){
		mii.fState=f?mii.fState|MFS_DEFAULT:mii.fState&~MFS_DEFAULT;
		update();
	}
	bool defaultItem(){
		return (mii.fState&MFS_DEFAULT)!=0;
	}
	void update(){
		if (!m_parent) return;
		mii.cch = mtext.length();
		mii.dwTypeData = (char*)mtext.c_str();
		SetMenuItemInfo(m_parent->hMenu,mii.wID,FALSE,&mii);
	}
	void parent(MenuContainer *m) {m_parent=m;}
	MenuContainer& parent() {return *m_parent;}
	int menuID(){return mii.wID;}
	int index(){
		if (m_parent) return 0;
		return 0;
	}
};

typedef MenuContainer<MenuItem> Menu;

class MainMenu : public Menu{
public:
	MainMenu():Menu(NULL){
		hMenu = CreateMenu();
	}
	virtual int event(int id) {
		return 0;
	}
};

class Form :public Container{
protected:
	EventListeners eventListener;
	bool fexist;
	MainMenu m_menu;
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
			wc.hbrBackground= (HBRUSH)(COLOR_3DFACE+1);
			wc.hIconSm		= NULL;
			wc.lpszMenuName	= NULL;
			wc.lpszClassName = FORMWINDOW_CLASS;
			if (!RegisterClassEx(&wc)) return;
		}


		RECT rc={0,0,w,h};
		AdjustWindowRect(&rc,WS_CAPTION | WS_OVERLAPPEDWINDOW&~(WS_THICKFRAME|WS_MAXIMIZEBOX),0);
		hWnd = CreateWindow(FORMWINDOW_CLASS,caption,
							WS_CAPTION | WS_OVERLAPPEDWINDOW&~(WS_THICKFRAME|WS_MAXIMIZEBOX),
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
	Form(HWND hWnd_) {
		hWnd=hWnd_;
		fexist=false;
	}
	Form(const std::string &caption="",int w=320,int h=240,bool fshow=false) {
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
		Application.run(*this);
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

	void menu(MainMenu *m) {
		m_menu=*m;
		SetMenu(hWnd,m_menu.hMenu);
	}
	void menu(const Menu &m) {
		m_menu=*(MainMenu*)&m;
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


	template<typename FUNC>
	void onClick(FUNC f){
		eventListener.add(Event::CLICK, f);
	}
	template<typename FUNC,typename OBJ>
	void onClick(FUNC f, OBJ* w){
		eventListener.add(Event::CLICK, f,w);
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
		if (event(ev)) return 0;
		switch(message)
		{
			case WM_CREATE:
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
						w->event(&ev);
					}
				}
			case WM_LBUTTONUP:
				{
					ev.type=Event::CLICK;
					eventListener.call(ev);
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
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
};

}

#endif
