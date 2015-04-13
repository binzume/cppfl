#include "base.h"

namespace kwlib{
HINSTANCE CApplication::hInstance=NULL;
bool CApplication::initflag=false;

int CApplication::run(class Form &f){
		if (!f.isexist()) return 0;
		f.show();
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (!f.isexist()) break;
		}
		return (msg.wParam);
}
void CApplication::doEvents(){
	MSG msg;
	while (PeekMessage (&msg,NULL,0,0,PM_NOREMOVE)) {
		if (!GetMessage (&msg,NULL,0,0))
			return ;//msg.wParam ;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
void CApplication::wait(long w){
	unsigned long t=GetTickCount();
	while(GetTickCount()<t+w) {
		doEvents();
		Sleep(1);
	}
}

CApplication Application;

}
