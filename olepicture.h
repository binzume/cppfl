#define OLEPICTURE_H

#include	<windows.h>
#include	<OLECTL.h>
#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )


bool oleLoad(DIBitmap &dib,const string &fname){

	IStream *iStream;
	IPicture *iPicture;
	long lWidth,lHeight;
	
	File file(fname);
	long size=file.size();
	if(size<=0) return false;

	HGLOBAL hGlobal = GlobalAlloc(GPTR, size);
	file.load((char*)hGlobal,size);

	if(CreateStreamOnHGlobal(hGlobal, TRUE, &iStream)!=S_OK){
		GlobalFree(hGlobal);
		return false;
	}

	if(OleLoadPicture(iStream, size, TRUE, IID_IPicture, (LPVOID*)&iPicture)!=S_OK){
		GlobalFree(hGlobal);
		return false;
	}
	iPicture->get_Width(&lWidth); //lpVtbl-> iPicture,
	iPicture->get_Height(&lHeight);
	GlobalFree(hGlobal);

	{
		HDC hdc=CreateDC("DISPLAY",NULL,NULL,NULL);
		int cxpi = GetDeviceCaps(hdc,LOGPIXELSX);
		int cypi = GetDeviceCaps(hdc,LOGPIXELSY);
		dib.create(MulDiv(lWidth,cxpi,2540),MulDiv(lHeight,cypi,2540),24);
		DeleteDC(hdc);
	}
	DCPen *p=dib.dcpen();
	int i=iPicture->Render(*p,0,0,dib.width,dib.height, 0, lHeight, lWidth, -lHeight, NULL);//•`‰æ /lpVtbl->
	p->release();

//	iStream->Release();
	iPicture->Release();	//ƒCƒ[ƒW‰ğ•ú

	return i==S_OK;

}
