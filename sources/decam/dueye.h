/*
* Interface uEye/OpenCv
*
* CuEye gère l'interface avec uEye
* uEyeOpenCv gère l'interface OpenCv<->uEye
* Damien Eynard 2010
* damien.eyn'at'gmail'dot'com
*
* Update 19/11/2010
* v1.2 : prise en charge de l'enregistrement avi
*
* Update 17/01/2016
* v2.0 : prise en charge OpenCV 3.1, uEye 4.72
*
* Update 13/02/2016
* v2.1 : prise en charge CMake 2.8
*
*/

#include <uEye.h>
#if defined(WIN32)
    #include <uEye_tools.h>
#endif // defined
#include <stdio.h>
#include <iostream>

#include <cv.h>
#include <highgui.h>

#ifndef CLASS_UEYE
#define CLASS_UEYE

//#define __LINUX__
//#define DRIVER_DLL_NAME "libueye_api.so"

#define MAX_SEQ_BUFFERS 300
#define DEF_GAIN 1
#define DEF_SHUTTER 1
#define DEF_COLOR MONO//BGR32
#define DEF_SIZE getSize(0,0)
#define DEF_NBUFFER 10
#define DEF_FPS 25

#if defined(WIN32)
    #define DLL_EXPORT __declspec( dllexport )
#else
    #define DLL_EXPORT
#endif

using namespace cv;

enum colorType{
	MONO = IS_CM_MONO8,
	RGB32 = IS_CM_RGBA8_PACKED,
	BGR32 = IS_CM_BGRA8_PACKED
};

struct size{
	int width;
	int height;
};

DLL_EXPORT inline  size getSize(int x, int y)
{
    size p;

	p.width = x;
	p.height = y;

    return p;
}

DLL_EXPORT inline CvSize getCvSize(size sI){ return cvSize(sI.width, sI.height); }

using namespace std;

class DLL_EXPORT CuEye{

protected:
	// uEye varibles
	HIDS	m_hCam;				// handle to camera
	HWND	m_hWndDisplay;		// handle to diplay window
	INT		m_nColorMode;		// Y8/RGB16/RGB24/REG32
	INT		m_nBitsPerPixel;	// number of bits needed store one pixel
	INT		m_nSizeX;			// width of image
	INT		m_nSizeY;			// height of image
	INT		m_nPosX;			// left offset of image
	INT		m_nPosY;			// right offset of image
	SENSORINFO sInfo;

	// memory needed for live display while using DIB
	INT		m_Ret;				// camera error return
	INT		m_lMemoryId;		// camera memory - buffer ID
	char*	m_pcImageMemory;	// camera memory - pointer to buffer
	char*	m_pcImageMemory2;	// camera memory - pointer to buffer
	SENSORINFO m_sInfo;			// sensor information struct
    INT     m_nRenderMode;		// render  mode
    INT     m_nFlipHor;			// horizontal flip flag
    INT     m_nFlipVert;		// vertical flip flag
	void *pMemVoid;
	size	m_nImg;
	colorType	ColorMode;
	bool	m_nLive;


	//Ring buffer sequence
	INT		m_nBuf;
	INT		m_lSeqMemId[MAX_SEQ_BUFFERS];	// camera memory - buffer ID
	char*	m_pcSeqImgMem[MAX_SEQ_BUFFERS];	// camera memory - pointer to buffer
	int		m_nSeqNumId[MAX_SEQ_BUFFERS];	// varibale to hold the number of the sequence buffer Id

	//AVI pointer
	INT pAviId;

	void CloseCam();

public:
	//void InitCam(bool color=0,bool live=0);
	CuEye(bool AutoGain=DEF_GAIN,
		  bool AutoShutter=DEF_SHUTTER,
		  int nBuffer=DEF_NBUFFER,
		  colorType ColorMode=DEF_COLOR,
		  size=DEF_SIZE);
	virtual ~CuEye();

	HIDS *GetCamIDS(){return &m_hCam;};
	void GrabFrame();
	void StopLive();
	void StartLive();
	int getLastRingBuffer(char *);
	void SaveCurrentBufferedImage(char*);
	void SaveImage(char*);

#if defined(WIN32)
	void startAVISave(std::string path,double fps);
	void pushFrame(char *);
	void stopAVISave();
#endif
};

class DLL_EXPORT CVuEye :public CuEye
{
private:
	char*	m_cvImageMemory;	// camera memory - pointer to buffer
	IplImage *iBuffer = nullptr;//cvCreateImage(cvSize(1, 1), IPL_DEPTH_8U, 1);
	Mat *iMat = nullptr;
	void InitOpenCVuEye();

public:

	CVuEye(size sI = DEF_SIZE,
				colorType ColorMode=DEF_COLOR,
				bool AutoGain=DEF_GAIN,
				bool AutoShutter=DEF_SHUTTER,
				int nBuffer=DEF_NBUFFER):
				CuEye(AutoGain, AutoShutter, nBuffer, ColorMode, sI)
				{InitOpenCVuEye(); };

/*	CVuEye(colorType ColorMode=DEF_COLOR):
				CuEye(DEF_GAIN, DEF_SHUTTER, DEF_NBUFFER, ColorMode, DEF_SIZE)
				{InitOpenCVuEye();};

	CVuEye(size sI=DEF_SIZE,colorType ColorMode=DEF_COLOR):
				CuEye(DEF_GAIN, DEF_SHUTTER, DEF_NBUFFER, ColorMode, sI)
				{InitOpenCVuEye();};
*/
	~CVuEye();

	IplImage* getCvImage(){return iBuffer;};
	Mat getMat();

	//Single shot
	void GrabFrame();
	void saveCvImage(const char* begin = "Img_", int = 0, const char* ext = ".bmp");
	static void saveCvImage(const char* begin, int var, const char *ext, IplImage *inputImage);

	//Live
	int getCvLastRingBuffer();

	//Avi
#if defined(WIN32)
	void pushFrame();
#endif
	bool isColor(){return (ColorMode==colorType::RGB32)?true:false;};

};

#endif
