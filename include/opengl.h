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

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);	//��ʂ��N���A����F��ݒ�
		glClearDepth( 1.0f );					//�F�[�x��ݒ�
		glEnable( GL_DEPTH_TEST );				//�`��v�Z�ɂ�����z�o�b�t�@��L���ɂ���
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
        sizeof(PIXELFORMATDESCRIPTOR),   //���̍\���̂̃T�C�Y
        1,                               //OpenGL�o�[�W����
        PFD_DRAW_TO_BITMAP |             //�r�b�g�}�b�v�ݒ�
        PFD_SUPPORT_OPENGL |                    
        PFD_SUPPORT_GDI,
        PFD_TYPE_RGBA,                   //RGBA�J���[
        24,                      //�F��
        0, 0,                            //RGBA�̃r�b�g�ƃV�t�g�ݒ�
        0, 0,                            //G
        0, 0,                            //B
        0, 0,                            //A
        0,                               //�A�L�������[�V�����o�b�t�@
        0, 0, 0, 0,                      //RGBA�A�L�������[�V�����o�b�t�@
        24,                      //Z�o�b�t�@    
        0,                               //�X�e���V���o�b�t�@
        0,                               //�g�p���Ȃ�
        PFD_MAIN_PLANE,                  //���C���[�^�C�v
        0,                               //�\��
        0, 0, 0                          //���C���[�}�X�N�̐ݒ�E���g�p
    };

		int pixelformat;
	
	    if((pixelformat = ChoosePixelFormat( hDC, &pfd2))==0){
			msgBox("ERR!");
	        return false;
	    }
	    //�s�N�Z���t�H�[�}�b�g
	    if(!SetPixelFormat(hDC, pixelformat, &pfd2)){
			msgBox("ERR!");
	        return false;
	    }
	    return true;
	}
};


#endif
