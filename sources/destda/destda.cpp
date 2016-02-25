#include "decam/dueye.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	CVuEye cam(true);

	
	/******************************/
	/* Sample single shot grabber */
	/******************************/
	
	/* OPENCV SAMPLE */
	/*
	IplImage *img = cam.getCvImage();
	do
	{
		cam.GrabFrame();
		//cam.SaveImage(L"toto.jpg");

		//cam.saveCvImage("test", 0, ".jpg");
		cvShowImage("image", img);
	} while (cvWaitKey(1) != 'q');

	cvDestroyWindow("image");
	*/

	/* OPENCV2 SAMPLE */

	/*Mat img = cam.getMat();
	do
	{
		cam.GrabFrame();
		imshow("Image", img);
		//imwrite("D:\\test.jpg", img);

	} while (waitKey(1) != 'q');
	*/

	/**************************/					
	/* Sample FreeRun grabber */
	/**************************/
	/*
	Mat img = cam.getMat();

	cam.StartLive();
	do{
		cam.getCvLastRingBuffer();
		imshow("Image", img);
	} while (waitKey(1) != 'q');
	cam.StopLive();
	*/

	/*******************************/
	/* Sample FreeRun OpenCV Video */
	/*******************************/
	//VideoWriter video;
	//video.open("D:\\Damien\\Acqui\\Test.avi", CV_FOURCC('F', 'M', 'P', '4'), 30, cam.getCvSize(), cam.getIsColor());
	cam.startAVISave("D:\\Damien\\Acqui\\Test.avi", 30);

	Mat img = cam.getMat();
	do
	{
		cam.GrabFrame();  
		//video << img;
		cam.pushFrame();
		imshow("Image", img);
		
	} while (waitKey(1) != 'q');
	
	VideoCapture readVideo;
	readVideo.open("D:\\Damien\\Acqui\\Test.avi");
	readVideo >> img;
	imwrite("D:\\Damien\\Acqui\\readVideo.jpg", img);

	return 0;
}