
#pragma comment( lib, "shell32.lib" )
// タスクトレイアイコン
class TrayIcon{
	HWND hWnd;
	NOTIFYICONDATA ticon;
//	static UINT s_uTaskbarRestart;
public:
	TrayIcon(HWND hWnd,HICON hIcon = NULL){
//		s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
		ticon.cbSize=sizeof(NOTIFYICONDATA);
		ticon.hIcon=hIcon;
		ticon.hWnd=hWnd;
		//ticon.uCallbackMessage=ICONMSG;
		ticon.uFlags= NIF_ICON | NIF_MESSAGE | NIF_TIP;
		ticon.uID=0;
		strcpy(ticon.szTip,"");
	}
	
	void setIcon(HICON hIcon){
		ticon.hIcon=hIcon;
	}

	void setMessage(unsigned long msg){
		ticon.uCallbackMessage = msg;
	}

	void setText(const std::string &s){
		strcpy( ticon.szTip, s.c_str());
	}
	
	void show(){
		Shell_NotifyIcon(NIM_ADD,&ticon);
	}

	void hide(){
		Shell_NotifyIcon(NIM_DELETE,&ticon);
	}
};
