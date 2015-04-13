#ifndef CPPFL_OPENGL_H
#define CPPFL_OPENGL_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment( lib , "opengl32.lib" )
#pragma comment( lib , "glu32.lib" )

#ifndef NOGLUT
#include <GL/glut.h>
#pragma comment( lib , "glut32.lib" )
#endif

class OpenGL {
public:
	HGLRC hRC;
	HDC hDC;
	OpenGL(){}
	bool init(HDC hdc){
		hDC=hdc;
		if( !SetupPixelFormat(hDC) ) return false;
		hRC = wglCreateContext(hDC);
		if (hRC == NULL) return false;
		if (wglMakeCurrent(hDC, hRC) == FALSE) return false;

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);	//画面をクリアする色を設定
		glClearDepth( 1.0f );					//色深度を設定
		glEnable( GL_DEPTH_TEST );				//描画計算におけるzバッファを有効にする
		return true;
 	}

	void init(HWND hWnd){
		HDC hDC = GetDC(hWnd);
		init(hDC);
		ReleaseDC(hWnd, hDC);
	}

 	void free(){
		wglDeleteContext(hRC);
 	}
 	
 	void setdc(HDC hdc){
 		hDC = hdc;
 		wglMakeCurrent(hDC, hRC);
 	}
 	void resetdc(){
 		wglMakeCurrent(hDC, 0);
 	}
 	void draw(){
	 	glFlush();
 		SwapBuffers(hDC);
 	}

	bool SetupPixelFormat(HDC hDC)
	{
	    PIXELFORMATDESCRIPTOR pfd = 
	    {
	        sizeof(PIXELFORMATDESCRIPTOR),
	        1,
	        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
	        PFD_TYPE_RGBA,
	        24,
	        0, 0, 0, 0, 0, 0,
	        0,
	        0,
	        0,
	        0,  0, 0, 0,
	        32,
	        0,
	        0,
	        PFD_MAIN_PLANE,
	        0,
	        0, 0, 0
	    };
    static PIXELFORMATDESCRIPTOR pfd2 = {
        sizeof(PIXELFORMATDESCRIPTOR),   //この構造体のサイズ
        1,                               //OpenGLバージョン
        PFD_DRAW_TO_BITMAP |             //ビットマップ設定
        PFD_SUPPORT_OPENGL |                    
        PFD_SUPPORT_GDI,
        PFD_TYPE_RGBA,                   //RGBAカラー
        24,                      //色数
        0, 0,                            //RGBAのビットとシフト設定
        0, 0,                            //G
        0, 0,                            //B
        0, 0,                            //A
        0,                               //アキュムレーションバッファ
        0, 0, 0, 0,                      //RGBAアキュムレーションバッファ
        24,                      //Zバッファ    
        0,                               //ステンシルバッファ
        0,                               //使用しない
        PFD_MAIN_PLANE,                  //レイヤータイプ
        0,                               //予約
        0, 0, 0                          //レイヤーマスクの設定・未使用
    };

		int pixelformat;
	
	    if((pixelformat = ChoosePixelFormat( hDC, &pfd2))==0){
			msgBox("ERR!");
	        return false;
	    }
	    //ピクセルフォーマット
	    if(!SetPixelFormat(hDC, pixelformat, &pfd2)){
			msgBox("ERR!");
	        return false;
	    }
	    return true;
	}
};


#endif
