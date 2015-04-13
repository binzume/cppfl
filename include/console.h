#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <windows.h>
#include <conio.h>
#include <stdarg.h>
#include <string>

class Console
{
public:
	Console(){
		FreeConsole();
		AllocConsole();
	}

	~Console(){
		FreeConsole();
	}
	void printf(const char *fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		_cprintf(fmt, ap);
		va_end(ap);
	}
	void print(const std::string &s){
		_cprintf(s.c_str());
	}
};

#endif
