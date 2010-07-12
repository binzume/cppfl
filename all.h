#ifndef _INC_STDIO //VC++
#define _NO_CONSOLE
#endif

#include <string>
#include <vector>
#include <windows.h>
#include <commctrl.h>


#ifndef _WIN64
#undef SetWindowLongPtr
static __inline LONG_PTR SetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{ return SetWindowLong(hWnd, nIndex, (LONG)dwNewLong); }
#endif


namespace kwlib{
#include "base.h"
#include "file.h"
#include "thread.h"
}
#include "application.h"
#include "widget.h"
#include "form.h"
#include "resources.h"

#ifndef NOIMP
#include "application_cpp.h"
#endif

namespace kwlib{
#define _USEDIB

#include "pen.h"
#include "imageex.h"
#include "imagetool.h"
#include "bmpwindow.h"
#include "system.h"
};
#include "timer.h"


static inline int wait(kwlib::Form &f){
	return kwlib::Application.run(f);
}
static inline void wait(long w){
	kwlib::Application.wait(w);
}


#define STR std::string()+


// libs

#ifndef NOIMP

#if defined(_MSC_VER)
# pragma comment( lib, "gdi32.lib" )
# pragma comment( lib, "user32.lib" )
# pragma comment( lib, "comctl32.lib" )
# ifndef CPPFL_NO_MAIN
# ifdef _NO_CONSOLE
#  pragma comment(linker, "/subsystem:windows")
#  pragma comment(linker, "/entry:mainCRTStartup")
# else
#  pragma comment(linker, "/subsystem:console")
#  pragma comment(linker, "/entry:mainCRTStartup")
# endif
# endif
#elif defined(__BORLANDC__)
# pragma link "user32.lib"
# pragma link "gdi32.lib"
# pragma link "comctl32.lib"
#endif

#endif


// namespace

using std::string;
using namespace kwlib;
using namespace kwlib::MessageBox;
