

#include <iostream>
#include "../cppfl/all.h"
#include "../cppfl/capture2.h"
using namespace std;

int main()
{
	Camera cam;
	
	IGraphBuilder *pGraph = cam.pGraph;
	
	IBaseFilter *pSrc = Camera::getSource();
	cam.pSrc = pSrc;
	if (pSrc == NULL) {
		cout << "video input device not found" << endl;
		return 0;
	}
	pGraph->AddFilter(pSrc, L"Video Capture");
	
	if (cam.init()==0) {
		cout << "connect error" << endl;
		return 0;
	}
	ISampleGrabber *pSGrab = cam.pSGrab;

	cam.start();
	Sleep(100);

	long *buffer = NULL;
	long bufsize = 0;
	// ƒTƒCƒYŽæ“¾
	do{
		pSGrab->GetCurrentBuffer(&bufsize, NULL);
		Sleep(1);
	}while(bufsize <= 0);

	buffer = (long *)malloc(sizeof(char)*bufsize);
	if(buffer==NULL){
		cerr << "can't allocate memory\n";
		return 1;
	}
	cerr << "bufsize : " << bufsize << endl;

	BitmapWindow bw("sample", 680, 480);
	Bitmap *img = new Bitmap(buffer,cam.width,cam.height);

	while (bw.isexist()) {
		if(!cam.cap(img)){
			cerr << "Capture error\n";
			break;
		}
		ImageFilter2::flip(*img);
		img->draw(*bw.dib);
		bw.update();

		wait(10);
	}

	return 0;
}


