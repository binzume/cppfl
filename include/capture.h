#ifndef _CAPTURE_H
#define _CAPTURE_H
#include <windows.h>
#include <vfw.h>
//#pragma link "vfw32.lib" // BCC
#pragma comment(lib, "vfw32.lib") // VC++


#ifndef CAPTURE_MAX_DEVS
#define CAPTURE_MAX_DEVS 8
#endif

typedef int (*CB_CAP)(Bitmap *,long);

class Capture{
	HWND hWndVideo;			// Video window
	int DeviceID;			// Capture device
	CB_CAP cbcap;
	long dat;
public:
	int devs;
	char DeviceName[CAPTURE_MAX_DEVS][100];	// Device name
	BITMAPINFO bmpInfo;		// Bitmap information of the video frame
	Bitmap *img;
	Capture(){
		devs=0;
		cbcap=NULL; img=NULL;
	}
	void cap(){
		capGrabFrameNoStop(hWndVideo);
	}
	void rate(int t){
		capPreviewRate(hWndVideo,t);
	}
	void start(){
		capPreview(hWndVideo,TRUE);
	}
	void setFrameCallback(CB_CAP f,long d) {cbcap=f;dat=d;};
	int init(Bitmap &dst,HWND hWnd=NULL){
		return init(hWnd,&dst);
	}
	int init(HWND hWnd,Bitmap *dst){
		char DeviceVersion[100];
		BOOL bDriverDescription;
		int i,j;
	
		// Create capture window
		hWndVideo = capCreateCaptureWindow("CapWindow",
			WS_CHILD , // | WS_VISIBLE
			0,0,  320,240,  hWnd,0);
		if(!hWndVideo) return 0;

		SetWindowLongPtr(hWndVideo,GWLP_USERDATA,(LONG_PTR)this);

		// Callbacks
		capSetCallbackOnFrame(hWndVideo, VFW_FrameCallback);
		capSetCallbackOnError(hWndVideo, VFW_ErrorCallback);
	
		// Device list
		j = 0;
		for(i=0; i<CAPTURE_MAX_DEVS; i++){
			bDriverDescription = capGetDriverDescription(
				i,
				(LPSTR)DeviceName[i], sizeof(DeviceName[i]),
				(LPSTR)DeviceVersion, sizeof(DeviceVersion)
				);
			if(!bDriverDescription) break;
			j++;
		}
		devs=j;
		img = dst;
		return devs;
	}
	
	int open(long id) {
		DWORD wSize;
		// Select device
		DeviceID = id;//DialogBox(hInst, MAKEINTRESOURCE(IDD_SELDEV), NULL, SelDevDlgProc);
		capDriverConnect(hWndVideo, DeviceID);
	
		// Picture format
		wSize = capGetVideoFormatSize(hWndVideo);
		if( !capGetVideoFormat(hWndVideo, &bmpInfo, wSize) ){
			capDriverDisconnect( hWndVideo );
			return -2;
		}
		if( bmpInfo.bmiHeader.biCompression != BI_RGB || bmpInfo.bmiHeader.biBitCount != 24){
			capDriverDisconnect( hWndVideo );
			::MessageBox(hWndVideo, "Sorry, only 24bit RGB. exitting...", "error", MB_OK);
			return -3;
		}

		if (img)img->create(bmpInfo.bmiHeader.biWidth,bmpInfo.bmiHeader.biHeight,24);

		// Overlay
		//capOverlay(hWndVideo, TRUE);
		// capPreviewRate( hWndCap, 1 );
		// capPreview( hWndCap, TRUE );
		return 0;
	}
	void end() {
		if (hWndVideo) {
			// release callbacks
			capSetCallbackOnError(hWndVideo, NULL);
			capSetCallbackOnFrame(hWndVideo, NULL);

			capDriverDisconnect( hWndVideo );
			
			// destroy the window
			DestroyWindow(hWndVideo);
			hWndVideo = NULL;
		}
	}
	static LRESULT PASCAL VFW_ErrorCallback(HWND hWnd, int nErrID, LPSTR lpErrorText) 
	{ 
	    return (LRESULT) TRUE; 
	} 
	static LRESULT PASCAL VFW_FrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr) 
	{
		Capture *_this = (Capture*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
		return _this->FrameCallback(lpVHdr);
	}
	LRESULT FrameCallback(LPVIDEOHDR lpVHdr) {
		if (img) {
			//memcpy(img->buf,lpVHdr->lpData, lpVHdr->dwBufferLength);
			LPBYTE p=lpVHdr->lpData;
			for (int y=img->height-1;y>=0;y--) {
				memcpy(img->buf+img->rowbytes*y,p, img->rowbytes);
				p+=img->rowbytes;
			}
			if (cbcap) cbcap(img,dat);
		} else {
			Bitmap img;
			img.create_noalloc(bmpInfo.bmiHeader.biWidth,bmpInfo.bmiHeader.biHeight,24);
			img.buf=(unsigned char*)lpVHdr->lpData;
			if (cbcap) cbcap(&img,dat);
			img.buf=NULL;
		}
		return (LRESULT)TRUE;
	}
};

#endif


