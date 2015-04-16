#include <iostream>
#include "../include/all.h"
#define NOGLUT // draw to bitmap buffer...
#include "../include/opengl.h"

using namespace std;

#define PI 3.14159265358979

// ‰ñ“]—p
double rot=0;
int kf=1;

void draw_sphere(double r)
{
	int n = 20; // •ªŠ„”(2‚Ì”{”)
	int i,j;
	double x,y;
	double z1,z2,nz1,nz2,s1,s2;

/*
	nz1 = cos(1*PI*2/n);
	z1 = nz1*r;
	s1 = sin(1*PI*2/n);
	// ã
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0,0,1);
	glVertex3d(0,0,r);
	for (i=0;i<=n;i++) {
			x=cos(i*PI*2/n)*s1, y=sin(i*PI*2/n)*s1;
			glNormal3d(x,y,nz1);
			glVertex3d(x*r,y*r,z1);
	}
	glEnd();

	// ‰º
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0,0,-1);
	glVertex3d(0,0,-r);
	for (i=0;i<=n;i++) {
			x=cos(i*PI*2/n)*s1, y=sin(i*PI*2/n)*s1;
			glNormal3d(x,y,-nz1);
			glVertex3d(x*r,y*r,-z1);
	}
	glEnd();

	// ’†
//*/
	for (j=0;j<n/2;j++) {
		s1 = sin(j*PI*2/n);
		s2 = sin((j+1)*PI*2/n);
		nz1 = cos(j*PI*2/n);
		nz2 = cos((j+1)*PI*2/n);
		z1=nz1*r; z2=nz2*r;
		glBegin(GL_QUAD_STRIP);
		for (i=0;i<=n;i++) {
					x=cos(i*PI*2/n), y=sin(i*PI*2/n);
					glNormal3d(x*s1,y*s1,nz1);
					glVertex3d(x*r*s1,y*r*s1,z1);
					glNormal3d(x*s2,y*s2,nz2);
					glVertex3d(x*r*s2,y*r*s2,z2);
		}
		glEnd();
	}

}


void draw_cylinder(double r,double p1,double p2)
{
	int n = 20; // •ªŠ„”
	double x,y,xx,yy;
	int i;
	glBegin(GL_QUAD_STRIP);
	for (i=0;i<=n;i++) {
		x = cos(i*2*PI / n);
		y = sin(i*2*PI / n);
		xx=x*r , yy=y*r;
		glNormal3d(x,y,0);
		glVertex3d(xx,yy,p2);
		glVertex3d(xx,yy,p1);

	}
	glEnd();
}


void OnDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslated(-0.0,0.0,-3.0);

	glRotated(rot, 0.0, 1.0, 0.0); // ‰ñ‚·

	glPushMatrix();
	glTranslated(-0.6,0.0,0.0);
	draw_sphere(0.5); // ”¼Œa0.5‚Ì‹…
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.2,0.0,0.0);
	draw_sphere(0.7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.6,0.0,0.0);
	glRotated(90, 0.0, 1.0, 0.0);
	draw_cylinder(0.2,0,2.0);
	glPopMatrix();

}


int main(){
	BitmapWindow win("test",640,480);
	DIBitmap gldib(640,480);

	OpenGL gl;
	if (!gl.init(gldib.getdc())){
		cerr << "ERROR: glInit()" << endl;
	}

	// OpenGL‰Šú‰»
	GLfloat light_position[] = {-1.0, 1.0, 1.0, 0.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};

	glClearColor(0.0, 1.0, 1.0, 0.0);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	int w = gldib.width, h = gldib.height;
	glViewport(0, 0, (GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5, 0.5, -0.5*h/w, 0.5*h/w,0.5,20.0);
	glMatrixMode(GL_MODELVIEW);

	while(win.isexist()) {
		wait(20);
		OnDraw();
		gl.draw();
		gldib.drawto(*win.dib);
		win.update();
		rot+=1.0;
	}

	wait(win);
	return 0;
}
