#ifndef _APPLICATION_H
#define _APPLICATION_H

namespace kwlib{

class CApplication{
public:
	static HINSTANCE hInstance;
	static bool initflag;
	CApplication(){
		init();
	}
	static void exit(int i=0){::exit(i);}
	static void init(){
		if (initflag) return;
		hInstance=GetModuleHandle(NULL);
		InitCommonControls();
		initflag=true;
	}
	static int run(class Form &f);
	static void wait(long w);
	static void doEvents();
};

extern CApplication Application;

}
#endif
