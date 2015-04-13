#ifndef _IMAGE_EX_H
#define _IMAGE_EX_H
#include "image.h"
#include <math.h>

#ifndef MessageBox // FIXME
#pragma pack (1)
struct BITMAPFILEHEADER{
	short    bfType;
	long   bfSize;
	short    bfReserved1;
	short    bfReserved2;
	long   bfOffBits;
};
struct BITMAPINFOHEADER{
	long  biSize; 
	long   biWidth; 
	long   biHeight; 
	short   biPlanes; 
	short   biBitCount; 
	long  biCompression; 
	long  biSizeImage; 
	long   biXPelsPerMeter; 
	long   biYPelsPerMeter; 
	long  biClrUsed; 
	long  biClrImportant; 
}; 
#pragma pack ()
#endif



class ImageFilter2 {
public:
	template <class T>
	static void flip(T& img) {
		char *tbuf = new char[img.rowbytes];
		int y;
		for (y=0;y<img.height/2;y++) {
			memcpy(tbuf,img.buf+img.rowbytes*y,img.rowbytes);
			memcpy(img.buf+img.rowbytes*y,img.buf+img.rowbytes*(img.height-y-1),img.rowbytes);
			memcpy(img.buf+img.rowbytes*(img.height-y-1),tbuf,img.rowbytes);
		}
		delete [] tbuf;
	}
	template <typename T>
	static void flip(T* img) {
		flip(*img);
	}
};

template <class T>
class ImageFilter :public T{
public:
	ImageFilter(){}
	ImageFilter(char *fname){
		load(fname);
	}
	void chTone(int hh,int ss,int vv) {
		int x,y;
		int Ch,Cs,Cv,Cd;
		int r,g,b;
		int h,p,q,t;
		int m;
		unsigned char *img=buf;
		for(y=0; y<height; y++){
			for(x=0; x<width*3; x+=3){
				if(img[x]<img[x+1]){
					Cv=img[x+1];
					Cd=img[x];
					m=1;
				} else {
					Cv=img[x];
					Cd=img[x+1];
					m=2;
				}
				if(Cv<img[x+2]){Cv=img[x+2];m=0;}
				if(Cd>img[x+2]) Cd=img[x+2];
				if(Cv>0){
					Cs=(Cv-Cd)*1024/Cv;
				} else {
					Cs=0;
				}
				if(Cs!=0){
					b=(Cv-img[x  ])*1024 / (Cv-Cd);
					g=(Cv-img[x+1])*1024 / (Cv-Cd);
					r=(Cv-img[x+2])*1024 / (Cv-Cd);
					if(m==0){
						Ch=(b-g);
					}else if(m==1){
						Ch=(r-b+2048);
					}else{
						Ch=(g-r+4096);
					}
				}else{
					Ch=0;
				}
	
				Ch+=hh;
				Cs=Cs*ss>>10;
				if(Cs>1024) Cs=1024;
				Cv=Cv*vv>>10;
				if(Cv>255) Cv=255;
	
				if(Cs==0){
					r=Cv;
					g=Cv;
					b=Cv;
				}else{
					//if(Ch<0) Ch+=1024*6;
					Ch=(Ch+1024*6)%(1024*6);
					h=Ch>>10;
					//if(h>5)h-=5;
					p=Cv*(1024-Cs) >>10;
					q=Cv*(1024*1024-Cs*(     Ch-h*1024)) >>20;
					t=Cv*(1024*1024-Cs*(1024-Ch+h*1024)) >>20;
					switch(h){
						case 0: r=Cv;g=t; b=p; break;
						case 1: r=q; g=Cv;b=p; break;
						case 2: r=p; g=Cv;b=t; break;
						case 3: r=p; g=q; b=Cv;break;
						case 4: r=t; g=p; b=Cv;break;
						case 5: r=Cv;g=p; b=q; break;
					}
				}
				img[x  ]=b;
				img[x+1]=g;
				img[x+2]=r;
			}
			img+= rowbytes;
		}
	}
	void changeBrt(int bb,int cc,int vv){
		//コントラスト・明るさ
		if (vv<=0) return;
		int x,y;
		unsigned char *img=buf;
		int ttable[256];
		int i;
		for (i=0; i<256; i++) {
			ttable[i]=i+bb;
			//ttable[i]=(int)pow((double)ttable[i],1024.0/(double)vv);
			ttable[i]=(int)(pow((double)ttable[i]/255,1024.0/(double)vv)*255);
			ttable[i]=(ttable[i]-128)*cc/1024+128;
			if (ttable[i]>255) ttable[i]=255;
			if (ttable[i]<0) ttable[i]=0;
		}
		for(y=0; y<height; y++){
			for(x=0; x<rowbytes; x+=4){ // 32	ビットづつ
				img[x  ]=ttable[img[x  ]];
				img[x+1]=ttable[img[x+1]];
				img[x+2]=ttable[img[x+2]];
				img[x+3]=ttable[img[x+3]];
			}
			img+= rowbytes;
		}
	}
	void flip() {
		char *tbuf = new char[rowbytes];
		int y;
		for (y=0;y<height/2;y++) {
			memcpy(tbuf,buf+rowbytes*y,rowbytes);
			memcpy(buf+rowbytes*y,buf+rowbytes*(height-y-1),rowbytes);
			memcpy(buf+rowbytes*(height-y-1),tbuf,rowbytes);
		}
		delete [] buf;
	}
	Image* rotate(int deg) {
		if (deg==0) return NULL;
		Image *img=new Image;
		int h,w;
		int x,y;
		w=width;
		h=height;
	
		if (deg&1) { // 2/PI*n
			img->create(h,w,depth);
			unsigned char *img1=buf;
			unsigned char *img2=img->buf;
			int bpl1=rowbytes;
			int bpl2=img->rowbytes;
			if (deg<0) { // rotate left
				img2+=bpl2*3-3;
				for(y=0;y<h;y++) {
					for(x=0;x<w;x++) {
						memcpy(img2+bpl2*x,img1+x*3,3);
					}
					img1+=bpl1;
					img2-=3;
				}
			} else { // rotate right
				for(y=0;y<h;y++) {
					for(x=0;x<w;x++) {
						memcpy(img2+bpl2*(w-x-1),img1+x*3,3);
					}
					img1+=bpl1;
					img2+=3;
				}
			}
		} else { // 2/PI*n:flip
			img->create(w,h,depth);
			unsigned char *img1=buf;
			unsigned char *img2=img->buf;
			int bpl1=rowbytes;
			int bpl2=img->rowbytes;
			img2+=bpl2*h-3;
			for(y=0;y<h;y++) {
				for(x=0;x<w;x++) {
					memcpy(img2-x*3,img1+x*3,3);
				}
				img1+=bpl1;
				img2-=bpl2;
			}
		}

		copy(img);
		return img;
		
	}
	void copy(Image *src) {
		if (width!=src->width || height!=src->height)
			create(src->width,src->height,src->depth);
		unsigned char *img1=buf;
		unsigned char *img2=src->buf;
		int y;
		for(y=0;y<height;y++) {
			memcpy(img1,img2,rowbytes);
			img1+=rowbytes;
			img2+=rowbytes;
		}
	}

	int load(char *fname){
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;
		FILE *fp = fopen(fname,"rb");
		if (!fp) return 0;
		fread(&bf,sizeof(BITMAPFILEHEADER),1,fp);
		if (bf.bfType!=0x4d42) return 0;
		fread(&bi,sizeof(BITMAPINFOHEADER),1,fp);
		create(bi.biWidth,bi.biHeight,bi.biBitCount);
		fread(buf,rowbytes*height,1,fp);
		fclose(fp);
		return 1;
	}
	int save(char *fname){
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;
		memset(&bf,0,sizeof(bf));
		memset(&bi,0,sizeof(bi));
		FILE *fp = fopen(fname,"wb");
		if (!fp) return 0;
		bf.bfType=0x4d42;
		bf.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+rowbytes*height;
		bf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
		fwrite(&bf,sizeof(BITMAPFILEHEADER),1,fp);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = depth;
		fwrite(&bi,sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(buf,rowbytes,height,fp);
		fclose(fp);
		return 1;
	}

};

typedef ImageFilter<Bitmap> ImageEx;

#endif
