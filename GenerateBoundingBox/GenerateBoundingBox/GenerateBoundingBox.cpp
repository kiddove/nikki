// GenerateBoundingBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImageReader.h"

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	ImageReader imgReader;
	imgReader.LoadFile("roadresult.tif");

	//cv::Mat img, image;
	//try {
	//image = cv::imread("roadresult.tif", CV_LOAD_IMAGE_UNCHANGED );
	//}
	//catch (cv::Exception e) {
	//	cout << e.err << std::endl ;
	//}

	//cv::cvtColor(image, img, CV_RGB2GRAY);

	//int iType = img.type();
	int iType = imgReader.m_data.depth();

	if(! imgReader.m_data.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	vector<vector<Point> > contours;
	vector<vector<int>> hierarchy;
	try {
		findContours(imgReader.m_data, contours, CV_RETR_CCOMP,  CV_CHAIN_APPROX_SIMPLE);
	}
	catch (cv::Exception e) {
		cout << e.err << std::endl ;
	}

	cv::imwrite("bound.tif", imgReader.m_data);

	namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window", imgReader.m_data );                   // Show our image inside it.

	waitKey(0);   

	return 0;
}

