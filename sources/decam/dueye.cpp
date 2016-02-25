#include "dueye.h"


CuEye::CuEye(bool AutoGain, bool AutoShutter, int nBuffer, colorType ColorMode, size sImg)
{
	m_Ret = IS_NO_SUCCESS;

    //CloseCam();

	// init camera (open next available camera)
	this->ColorMode = ColorMode;
	m_hCam = (HIDS) 0;
	m_Ret = is_InitCamera(&m_hCam, NULL);

	if( m_Ret == IS_SUCCESS )
	{
		double dEnable=1,
			   dDisable=0;

		// retrieve original image size
		m_Ret = is_GetSensorInfo( m_hCam, &sInfo );
		if( m_Ret != IS_SUCCESS )	printf("GetSensorInfo failed\n");
		else{
			cout << " Initialisation successfull" << endl;
			cout << " Sensor : " << sInfo.strSensorName << endl;
			cout << " W x H : " << sInfo.nMaxWidth << " x " << sInfo.nMaxHeight << endl;
			cout << " Pixel size : " << (double)sInfo.wPixelSize / 100. << "micrometres" << endl;
			cout << endl;
		}

		m_nSizeX = sInfo.nMaxWidth;
		m_nSizeY = sInfo.nMaxHeight;

#ifndef __LINUX__
		// setup the color depth to the current windows
		m_Ret = is_GetColorDepth(m_hCam,&m_nBitsPerPixel,&m_nColorMode);
		if( m_Ret != IS_SUCCESS )
		{
			printf("GetColorDepth failed\n");
		}
#else
		m_nBitsPerPixel = 32;
#endif
		m_nColorMode = ColorMode;
		m_Ret = is_SetColorMode(m_hCam, m_nColorMode);
		if( m_Ret != IS_SUCCESS )
		{
			printf("SetColorMode failed\n");
		}

		AutoGain?is_SetAutoParameter(m_hCam,IS_SET_ENABLE_AUTO_GAIN,&dEnable,0):is_SetAutoParameter(m_hCam,IS_SET_ENABLE_AUTO_GAIN,&dDisable,0);
		AutoShutter?is_SetAutoParameter(m_hCam,IS_SET_ENABLE_AUTO_SHUTTER,&dEnable,0):is_SetAutoParameter(m_hCam,IS_SET_ENABLE_AUTO_SHUTTER,&dDisable,0);

		//SINGLE FRAME
		/*// memory initialization
		is_AllocImageMem(m_hCam,m_nSizeX,m_nSizeY,m_nBitsPerPixel,&m_pcImageMemory,&m_lMemoryId);
		//set memory active
		is_SetImageMem( m_hCam, m_pcImageMemory,m_lMemoryId );
		*/


		//Resize de l'image
		if(sImg.width==0 || sImg.height==0)
		{
			sImg.width=m_nSizeX;
			sImg.height=m_nSizeY;
		}
		m_nImg.width = sImg.width;
		m_nImg.height = sImg.height;

		if( (m_nSizeX >= 2*sImg.width) && (m_nSizeY >= 2*sImg.height) )
		{
			is_SetBinning(m_hCam,IS_BINNING_2X_VERTICAL|IS_BINNING_2X_HORIZONTAL);

			//maj des info caméra
			m_nImg.width = sInfo.nMaxWidth/2;
			m_nImg.height = sInfo.nMaxHeight/2;
		}

		if( (m_nSizeX >= 4*sImg.width) && (m_nSizeY >= 4*sImg.height) )
		{
			is_SetBinning(m_hCam,IS_BINNING_4X_VERTICAL|IS_BINNING_4X_HORIZONTAL);

			//maj des info caméra
			m_nImg.width = sInfo.nMaxWidth/4;
			m_nImg.height = sInfo.nMaxHeight/4;
		}

		//RING BUFFER
		m_nBuf = nBuffer;
		for( int i=0; i< m_nBuf  ; i++ )
		{

			// allocate buffer memory
			m_Ret = is_AllocImageMem(	m_hCam,
										m_nImg.width,
										m_nImg.height,
										m_nBitsPerPixel,
										&m_pcSeqImgMem[i],
										&m_lSeqMemId[i]);
			if( m_Ret != IS_SUCCESS )
			{
				break;  // it makes no sense to continue
			}

			// put memory into seq buffer
			m_Ret = is_AddToSequence(	m_hCam, m_pcSeqImgMem[i], m_lSeqMemId[i] );
			m_nSeqNumId[i] = i+1;   // store sequence buffer number Id
			if( m_Ret != IS_SUCCESS )
			{
				// free latest buffer
				is_FreeImageMem( m_hCam, m_pcSeqImgMem[i], m_lSeqMemId[i] );
				break;  // it makes no sense to continue
			}
		}

		//is_SetImageSize( m_hCam, sImg.width, sImg.height);						// deprecated use is_AOI() instead
		IS_RECT rectAOI;

		rectAOI.s32X = 0;
		rectAOI.s32Y = 0;
		rectAOI.s32Width = sImg.width;
		rectAOI.s32Height = sImg.height;

		is_AOI(m_hCam, IS_AOI_IMAGE_SET_AOI, (void*)&rectAOI, sizeof(rectAOI));


#ifndef __LINUX__
		//Image in RAM
		is_SetDisplayMode( m_hCam, IS_SET_DM_DIB);//WINDOWS ONLY

#endif

		//Image in Graphic memory
		//is_SetDisplayMode( m_hCam, IS_SET_DM_DIRECT3D);
		is_SetExternalTrigger(m_hCam,IS_SET_TRIGGER_HI_LO_SYNC);
	}else{
		printf("Erreur camera - verifier le branchement.\nLe programme va maintenant s'arreter.\n");
		getchar();
		exit(-1);
	}
}

void CuEye::StartLive()
{
	int nCurrentState = IO_LED_STATE_2;
	//led to green
	is_IO(m_hCam, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));

	is_SetExternalTrigger(m_hCam, IS_SET_TRIGGER_HI_LO_SYNC);;
	is_CaptureVideo(m_hCam, IS_DONT_WAIT);
}

void CuEye::StopLive()
{
	is_StopLiveVideo(m_hCam,  IS_FORCE_VIDEO_STOP);

	int nCurrentState = IO_LED_STATE_1;
	//led to orange
	is_IO(m_hCam, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));
}

void CuEye::GrabFrame()
{
	//Gérer le round buffer
	is_FreezeVideo(m_hCam, IS_DONT_WAIT);
	//is_SetIO (m_hCam, 0x01); // deprecated is_IO à la place
	//is_SetIO (m_hCam, 0x00);
}

CuEye::~CuEye()
{
	CloseCam();
}

void CuEye::CloseCam()
{

	if( m_hCam != 0 )
	{
		//SINGLE FRAME
		// Free the allocated buffer
		/*if( m_pcImageMemory )is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
		m_pcImageMemory = NULL;*/

		//ROUND BUFFER
		for(int i=0;i<m_nBuf;i++)
		{
			if( m_pcSeqImgMem[i] )is_FreeImageMem( m_hCam, m_pcSeqImgMem[i], m_lSeqMemId[i] );
			m_pcSeqImgMem[i] = NULL;
		}

		// Close camera
		is_ExitCamera( m_hCam );
        m_hCam = NULL;
	}

}

int CuEye::getLastRingBuffer(char *m_pImgDest)
{
	INT nNum;
	int i;
	char *pcMem, *pcMemLast;

	is_GetActSeqBuf(m_hCam, &nNum, &pcMem, &pcMemLast);
	for( i=0 ; i<m_nBuf ; i++)
	{
		if( pcMemLast == m_pcSeqImgMem[i] )
		break;
	}

	// lock buffer for processing
	m_Ret = is_LockSeqBuf( m_hCam,m_nSeqNumId[i], m_pcSeqImgMem[i] );

	//// start processing...................................

	// display buffer
	is_CopyImageMem(m_hCam,m_pcSeqImgMem[i],m_lSeqMemId[i],m_pImgDest);

	//// processing completed................................

	// unlock buffer
	is_UnlockSeqBuf( m_hCam, m_lSeqMemId[i], m_pcSeqImgMem[i] );

	return m_Ret;
}

void CuEye::SaveCurrentBufferedImage(char* path)
{
	INT nNum;
	int i;
	char *pcMem, *pcMemLast;

	is_GetActSeqBuf(m_hCam, &nNum, &pcMem, &pcMemLast);
	for( i=0 ; i<m_nBuf ; i++)
	{
		if( pcMemLast == m_pcSeqImgMem[i] )
		break;
	}

	//we save the last image acquired

	// lock buffer for processing
	m_Ret = is_LockSeqBuf( m_hCam,m_nSeqNumId[i], m_pcSeqImgMem[i] );

	//save the image
	//is_SaveImageMemEx(m_hCam,path,pcMemLast,m_lSeqMemId[i],IS_IMG_JPG,75);

	//convert char* to wchar_t*
	size_t size = strlen(path) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs(wa, path, size);

	IMAGE_FILE_PARAMS ImageFileParams;
	ImageFileParams.pwchFileName = wa;
	ImageFileParams.pnImageID = NULL;
	ImageFileParams.ppcImageMem = NULL;
	ImageFileParams.nQuality = 75;
	ImageFileParams.nFileType = IS_IMG_JPG;

	is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*) &ImageFileParams, sizeof(ImageFileParams));

	// unlock buffer
	is_UnlockSeqBuf( m_hCam, m_lSeqMemId[i], m_pcSeqImgMem[i] );
}

void CuEye::SaveImage(char *path)
{
	//convert char* to wchar_t*
	size_t size = strlen(path) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs(wa, path, size);

	IMAGE_FILE_PARAMS ImageFileParams;
	ImageFileParams.pwchFileName = wa;
	ImageFileParams.pnImageID = NULL;
	ImageFileParams.ppcImageMem = NULL;
	ImageFileParams.nQuality = 75;
	ImageFileParams.nFileType = IS_IMG_JPG;

	is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams, sizeof(ImageFileParams));
}


//#if defined(WIN32)
void CuEye::startAVISave(const std::string path,double fps)
{
	INT pnX, pnY, pnBits, pnPitch, nNum;
	char *pcMem, *pcMemLast;

	is_GetActSeqBuf(m_hCam, &nNum, &pcMem, &pcMemLast);

	is_InquireImageMem (m_hCam, pcMemLast, m_lMemoryId, &pnX,&pnY, &pnBits, &pnPitch);

	if (isavi_InitAVI(&pAviId, m_hCam))cout << "erreur IniAVI" << endl;

	pnPitch = ((ColorMode == colorType::MONO) ? pnPitch * 8 / 8 : pnPitch * 8 / 32) - m_nSizeX;

	isavi_SetImageSize(pAviId, m_nColorMode, m_nSizeX, m_nSizeY, 0, 0, pnPitch);

	isavi_SetImageQuality(pAviId,75);

	if (isavi_OpenAVI(pAviId, path.c_str())) cout << "erreur OpenAVI" << endl;

	isavi_SetFrameRate(pAviId,fps);

	if(isavi_StartAVI(pAviId)) cout<<"erreur StartAVI"<<endl;

	int nCurrentState = IO_LED_STATE_2;
	//led to green
	is_IO(m_hCam, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));
}



void CuEye::pushFrame(char *m_pImgDest)
{
	INT nNum;
	int i;
	char *pcMem, *pcMemLast;

	is_GetActSeqBuf(m_hCam, &nNum, &pcMem, &pcMemLast);
	for( i=0 ; i<m_nBuf ; i++)
	{
		if( pcMemLast == m_pcSeqImgMem[i] )
		break;
	}

	// lock buffer for processing
	m_Ret = is_LockSeqBuf( m_hCam,m_nSeqNumId[i], m_pcSeqImgMem[i] );

	//// start processing...................................

	// save avi buffer
	is_CopyImageMem(m_hCam,m_pcSeqImgMem[i],m_lSeqMemId[i],m_pImgDest);
	if(isavi_AddFrame(pAviId,pcMemLast))cout<<"Erreur addFrame"<<endl;

	//// processing completed................................

	// unlock buffer
	is_UnlockSeqBuf( m_hCam, m_lSeqMemId[i], m_pcSeqImgMem[i] );
}

void CuEye::stopAVISave()
{
	isavi_StopAVI(pAviId);

	isavi_CloseAVI(pAviId);
	isavi_ExitAVI(pAviId);

	int nCurrentState = IO_LED_STATE_1;
	//led to orange
	is_IO(m_hCam, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));
}
//#endif

/**************************************************************************************/
