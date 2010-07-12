#ifndef _THREAD_H
#define _THREAD_H

#ifdef _WIN32
#	include <windows.h>
#	include <process.h>
	typedef uintptr_t pthread_t;
	typedef CRITICAL_SECTION pthread_mutex_t; 
//#	define pthread_create(thread_id,nul,func,prm) ((*(thread_id)=_beginthread(func,0,prm))==-1)
#	define pthread_create(thread_id,nul,func,prm) ((*(thread_id)=_beginthreadex(NULL,0,func,prm,0,NULL))==0)
#	define pthread_join(id,ret) {WaitForSingleObject((HANDLE)id,INFINITE);CloseHandle((HANDLE)id);}
#	define pthread_exit(ret) _endthreadex(ret)
#	define pthread_detach(thread_id) CloseHandle((HANDLE)thread_id);
#	define pthread_mutex_init(mutex,nul) InitializeCriticalSection(mutex)
#	define pthread_mutex_destroy(mutex) DeleteCriticalSection(mutex)
#	define pthread_mutex_lock(mutex) EnterCriticalSection(mutex)
#	define pthread_mutex_trylock(mutex) (TryEnterCriticalSection(mutex)==0)
#	define pthread_mutex_unlock(mutex) LeaveCriticalSection(mutex)
#	define THREAD_FUNCTION unsigned int __stdcall
#	define THREAD_RETURN return 0
#else
#	include <pthread.h>
#	define THREAD_FUNCTION void*
#	define THREAD_RETURN return NULL
#	define Sleep(x) usleep((x)*1000)
#endif


#define THREAD_EXEC 0x1 // thread running
#define THREAD_TERM 0x2
#define THREAD_ERROR 0x4
#define THREAD_ABORT 0x8
#define THREAD_FATAL 0x100 // fatal error!


class Thread{
	int ret;
	int ref_count;
protected:
	pthread_t m_id;
//	long m_id;
	long stat;
	long ctrl;
public:
	Thread(){
		ctrl=0;
		stat=0;
		ref_count=1;
	}
	~Thread(){
		ctrl=1;
		pthread_detach(m_id);
	}
	void start(){
		if (stat&THREAD_EXEC) return;
		stat=0;
		ref_count++;
		//m_id=_beginthread(ThreadRun,0,this);
		pthread_create(&m_id,NULL,ThreadRun,this);
	}
	void cancelpoint(){
		if (ctrl) {
			stat ^= THREAD_EXEC | THREAD_TERM; // reset:EXEC set:TERM
			finish();
			release();
			pthread_exit(0);
		}
	}
	pthread_t tid(){return m_id;}
	void join(){pthread_join(m_id,NULL);} // join thread.
	void detach(){pthread_detach(m_id);}
	void wait(){join();} // wait for thread term.
	void cancel(){
		ctrl=1;
		join();
	}
	bool isend(){return (stat&THREAD_TERM)!=0;}
	void release(){
		ref_count--;
		if (ref_count<=0) delete this;
	}
	static THREAD_FUNCTION ThreadRun(void *t) {
		Thread *thread=(Thread*)t;
		thread->stat |= THREAD_EXEC ; // set:EXEC
		thread->ret=thread->task();
		thread->stat ^= THREAD_EXEC | THREAD_TERM; // reset:EXEC set:TERM
		thread->finish();
		thread->release();
		THREAD_RETURN;
	}
	virtual int task(){
		return -1;
	}
	virtual void finish(){
	}
#ifdef _WIN32
	void suspend(){SuspendThread((HANDLE)m_id);}
	void resume(){ResumeThread((HANDLE)m_id);}
	void abort(){ // Dont use!!
		TerminateThread((HANDLE)m_id,-1);
		pthread_detach(m_id);
	}
	void sleep(int t){suspend();Sleep(t);resume();}
#endif
};


class ThreadFunc : public Thread{
	typedef int (*THREADFUNC)(class Thread*);
	THREADFUNC m_pfunc;
public:
	ThreadFunc(THREADFUNC func){
		m_pfunc=func;
	}
	virtual int task(){
		return m_pfunc?m_pfunc(this):-1;
	}
};

class Mutex {
public:
	pthread_mutex_t mutex;
	Mutex(){
		pthread_mutex_init(&mutex,NULL);
	}
	~Mutex(){
		pthread_mutex_destroy(&mutex);
	}
	void lock(){
		pthread_mutex_lock(&mutex);
	}
	bool trylock(){
		return pthread_mutex_trylock(&mutex)==0;
	}
	void unlock(){
		pthread_mutex_unlock(&mutex);
	}
};

class MLock {
	MLock(){}
	Mutex *mutex;
public:
	MLock(Mutex& m):mutex(&m){mutex->lock();}
	~MLock(){mutex->unlock();}
};

#endif
