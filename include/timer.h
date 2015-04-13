#ifndef _TIMER_H
#define _TIMER_H

namespace kwlib{

class Timer {
	TIMERPROC f;
	UINT_PTR hTimer;
	int m_interval;
public:
	Timer(TIMERPROC f_,int c){
		f=f_;
		m_interval=c;
		hTimer=NULL;
	}
	void start(){
		hTimer = SetTimer(NULL,0,m_interval,f);
	}
	void stop(){
		if (hTimer) KillTimer(NULL,hTimer);
		hTimer=NULL;
	}
	~Timer(){ stop();}
};


}

#endif

