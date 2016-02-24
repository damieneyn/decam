#include "decam/dueye.h"

void CVuEye::InitOpenCVuEye()
{

	int rColor;

	(ColorMode == MONO) ? rColor = 1 : rColor = 4;
	switch(ColorMode)
	{
        default:
        case MONO:
            cvChannel = 1 ;
            break;
        case BGR32:
            cvChannel = 4;
            break;
        case RGB32:
            cvChannel = 3;
            break;
    }

	//Init buffer opencv/uEye memory
	iBuffer=cvCreateImage(getCvSize(m_nImg),8,cvChannel);
	is_AllocImageMem(m_hCam,m_nImg.width,m_nImg.height,m_nBitsPerPixel,&m_cvImageMemory,&m_lMemoryId);

	cout << "- Image descriptor ----------------"<<endl;
	cout << "- Size : " << m_nImg.width << "x" << m_nImg.height<<endl;
	cout << "- Color : " << rColor << endl;
	cout << "- Channels : "<<cvChannel << endl;
	cout << "- Bits per pixel :" <<m_nBitsPerPixel<<endl;
	cout << "- End Image descriptor ------------"<<endl<<endl;
/*
	//pointer to where the image is stored
	iBuffer->nSize=112;
	iBuffer->ID=0;
	iBuffer->nChannels=rColor;
	iBuffer->alphaChannel=0;
	iBuffer->depth=8;
	iBuffer->dataOrder=0;
	iBuffer->origin=0;
*/
	//rColor?iBuffer->align=3:
	iBuffer->align=1;
	iBuffer->width=m_nImg.width;//sInfo.nMaxWidth;
	iBuffer->height=m_nImg.height;//sInfo.nMaxHeight;
	iBuffer->roi=NULL;
	iBuffer->maskROI=NULL;
	iBuffer->imageId=NULL;
	iBuffer->tileInfo=NULL;
	iBuffer->imageSize=rColor*m_nImg.width*m_nImg.height;//sInfo.nMaxWidth*sInfo.nMaxHeight;
	iBuffer->widthStep=4*m_nImg.width;

	//On link les buffer
	iBuffer->imageDataOrigin = (char*)m_cvImageMemory;
	iBuffer->imageData = (char*)m_cvImageMemory;

	//On attend de remplir le buffer à l'initialisation- LENT
	is_FreezeVideo(m_hCam, IS_WAIT);
}

void CVuEye::GrabFrame()
{
	CuEye::GrabFrame();
	CuEye::getLastRingBuffer(m_cvImageMemory);
}

CVuEye::~CVuEye()
{
	if( m_cvImageMemory )is_FreeImageMem( m_hCam, m_cvImageMemory, m_lMemoryId );
	m_cvImageMemory = NULL;
	//iBuffer->imageDataOrigin=NULL;
	//iBuffer->imageData=NULL;
	//cvReleaseImage(&iBuffer);
}

void CVuEye::saveCvImage(const char* begin, int var, const char *ext)
{
	imwrite(getPath(begin,var,ext), this->getMat());

	// deprecated
	//cvSaveImage(path.c_str(),&iBuffer);
}

// deprecated
void CVuEye::saveCvImage(const char* begin, int var, const char *ext, IplImage *inputImage)
{
	cvSaveImage(getPath(begin,var,ext).c_str(),inputImage);
}

void CVuEye::saveCvImage(const char* begin, int var, const char *ext, Mat &inputImage)
{
	imwrite(getPath(begin,var,ext), inputImage);
}

int CVuEye::getCvLastRingBuffer()
{
	return CuEye::getLastRingBuffer(m_cvImageMemory);
}
#if defined(WIN32)
void CVuEye::pushFrame()
{
	CuEye::pushFrame(m_cvImageMemory);
}
#endif

Mat CVuEye::getMat()
{
	return cv::cvarrToMat(getCvImage(), false);
}

