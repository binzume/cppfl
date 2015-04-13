/**
 *   DirectShow Capture
 *
 */
#include <qedit.h>
#include <dshow.h>

#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ole32.lib" )

class Camera{
public:
	bool preview;

	IGraphBuilder *pGraph;
	ICaptureGraphBuilder2 *pBuilder;
	IBaseFilter *pSrc;
	IBaseFilter *pGrabFilter;
	ISampleGrabber *pSGrab;
	IMediaControl *pMediaControl;
	int width;
	int height;

	Camera(){
		preview = false;
		pGraph = NULL;
		pBuilder = NULL;
		pSGrab = NULL;
		pGrabFilter = NULL;
		pMediaControl = NULL;

		CoInitialize(NULL);
		CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (void **)&pGraph);

		CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
			IID_ICaptureGraphBuilder2, (void **)&pBuilder);
		pBuilder->SetFiltergraph(pGraph);

	}


	~Camera(){
		if (pGraph) pGraph->Release();
		if (pBuilder) pBuilder->Release();
		if (pSGrab) pSGrab->Release();
		if (pGrabFilter) pGrabFilter->Release();
		if (pMediaControl) pMediaControl->Release();

		CoUninitialize();
	}

	int init(){
		// サンプルグラバ
		CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		  IID_IBaseFilter, (LPVOID *)&pGrabFilter);
		pGrabFilter->QueryInterface(IID_ISampleGrabber, (void **)&pSGrab);

		// メディアタイプ
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video; // Sample Grabber の入力ピン(Capture Device の出力ピン)はUYVY
		mt.subtype = MEDIASUBTYPE_RGB24;     
		mt.formattype = FORMAT_VideoInfo;
		pSGrab->SetMediaType(&mt);

		// フィルタグラフに追加
		pGraph->AddFilter(pGrabFilter, L"Sample Grabber");

		// フィルタグラフ作成
		IPin *pSrcOut = Camera::getPin(pSrc, PINDIR_OUTPUT); // A
		IPin *pSGrabIN = Camera::getPin(pGrabFilter, PINDIR_INPUT);    // B
		IPin *pSGrabOut = Camera::getPin(pGrabFilter, PINDIR_OUTPUT);  // C
		
		if (pGraph->Connect(pSrcOut, pSGrabIN)!=S_OK) {
			return 0;
		}
	
	
		AM_MEDIA_TYPE am_media_type;
		
		pSGrab->GetConnectedMediaType(&am_media_type);
		VIDEOINFOHEADER *pVideoInfoHeader =
		 (VIDEOINFOHEADER *)am_media_type.pbFormat;
		printf("sample size = %d\n",
		 am_media_type.lSampleSize);
		 
		width = pVideoInfoHeader->bmiHeader.biWidth;
		height = pVideoInfoHeader->bmiHeader.biHeight;

		if (preview) {
			pGraph->Render(pSGrabOut);
		}
		return 1;
	}

	void start(){
		pSGrab->SetBufferSamples(TRUE); // GetCurrentBuffer を用いる際には必ずTRUE (FALSE では失敗する)
		pSGrab->SetOneShot(FALSE);
		pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
		pMediaControl->Run();
	}

	void stop(){
	}
	
	template <class T>
	bool cap(T *bmp) {
		long bufsize = bmp->rowbytes*bmp->height;
		
		if (pSGrab->GetCurrentBuffer(&bufsize, (long *)bmp->buf)!=S_OK) {
			return false;
		}
		return true;
	}

	static IBaseFilter* getSource() {
		ICreateDevEnum *pDevEnum = NULL;
		CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		  IID_ICreateDevEnum, (void **)&pDevEnum);
		
		IEnumMoniker *pClassEnum = NULL;
		if (pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0) != S_OK ) {
			return NULL;
		}

		ULONG cFetched;
		IMoniker *pMoniker = NULL;
		IBaseFilter *pSrc = NULL;
		if (pClassEnum->Next(1, &pMoniker, &cFetched) == S_OK){
			// とりあえず最初のやつを
			pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pSrc);
			pMoniker->Release();
		} else {
			return NULL;
		}
		pClassEnum->Release();
		pDevEnum->Release();
		return pSrc;
	}

	static IPin* getPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)
	{
	  BOOL       bFound = FALSE;
	  IEnumPins  *pEnum;
	  IPin       *pPin;
	
	  pFilter->EnumPins(&pEnum);
	  while(pEnum->Next(1, &pPin, 0) == S_OK)
	    {
	      PIN_DIRECTION PinDirThis;
	      pPin->QueryDirection(&PinDirThis);
	      if (bFound = (PinDir == PinDirThis)) // 引数で指定した方向のピンならbreak
	        break;
	      pPin->Release();
	    }
	  pEnum->Release();
	  return (bFound ? pPin : 0);
	}
};

