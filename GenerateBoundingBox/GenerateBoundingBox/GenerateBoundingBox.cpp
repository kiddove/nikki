// GenerateBoundingBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImageReader.h"
#include <math.h>       /* atan */

#define PI 3.14159265

using namespace cv;
using namespace std;

cv::Point midpoint(cv::Point ptA, cv::Point ptB) 
{
	return cv::Point((int)((ptA.x + ptB.x) * 0.5), (int)((ptA.y + ptB.y) * 0.5));
}
// comparison function object
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
	double i = fabs( contourArea(cv::Mat(contour1)) );
	double j = fabs( contourArea(cv::Mat(contour2)) );
	return ( i < j );
}

bool compareRect ( cv::Rect rc1,cv::Rect rc2 ) {
	// x, y is top left
	if (rc1.x < rc2.x)
		return true;
	else if (rc1.x > rc2.x)
		return false;
	else
	{
		return rc1.y < rc2.y;
	}
}

bool isRectValid(Rect rc1, Rect rc2, float threshold)
{
	float angle1, angle2;
	angle1 = atan((float)rc1.width / (float)rc1.height) * 180 / (float)PI;
	angle2 = atan((float)rc2.width / (float)rc2.height) * 180 / (float)PI;

	return abs(angle1 - angle2) < threshold / (2.0);
}


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

	//cv::Mat src;
	//imgReader.m_data.convertTo(src, CV_8U);
	//cv::Mat src_gray, threshold_output;
	// need 3or4channel 
	//cv::cvtColor(src, src_gray, CV_BGR2GRAY);
	//cv::blur(imgReader.m_data, src_gray, Size(1, 1));

	std::vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	// 8uc1 32fc1
	//threshold(imgReader.m_data, imgReader.m_data, 100, 255, THRESH_BINARY);
	// need 8uc1, 32sc1
	//int iii = CV_32SC1;
	//cv::Mat helpframe2;
	//imgReader.m_data.convertTo(helpframe2, CV_32S);
	//iii = helpframe2.depth();
	findContours(imgReader.m_data, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	//vector<Point2f> center(contours.size());
	//vector<float> radius(contours.size());

	for (int i = 0; i < (int)contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(cv::Mat(contours_poly[i]));
		//minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	}

	Mat drawing = Mat::zeros(imgReader.m_data.size(), CV_8UC3);
	RNG rng(12345);
	for (int i = 0; i < (int)contours.size(); i++)
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		//rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 1, 8, 0 );
		//circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
	}

	// remove the big ones
	std::vector<Rect> calcRect;
	for (std::vector<Rect>::iterator iter = boundRect.begin(); iter != boundRect.end(); iter++)
	{
		Rect rc = *iter;
		bool bValid = true;
		for(int i = 0; i < (int)boundRect.size(); i++)
		{
			if (rc == boundRect[i])
				continue;
			if ((rc & boundRect[i]) == boundRect[i])		// intersenction is boundRect[i], so boundRect is inside rc, rc is invalid
			{
				bValid = false;
				break;
			}
		}

		if (bValid)
		{
			calcRect.push_back(rc);
		}
	}

	//for (int i = 0; i < (int)calcRect.size(); i++)
	//{
	//	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	//	//drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
	//	rectangle( drawing, calcRect[i].tl(), calcRect[i].br(), color, 1, 8, 0 );
	//	//circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
	//}

	float angle_threshold = 3.0;
	// sort rect
	std::sort(calcRect.begin(), calcRect.end(), compareRect);
	// calc
	std::vector<Rect> resultRect;
	for (int j = 0; j < (int)calcRect.size(); j++)
	{
		for(int i = 0; i < (int)calcRect.size(); i++)
		{
			if (calcRect[j] == calcRect[i])
			{
				continue;
			}

			if (!isRectValid(calcRect[j], calcRect[i], angle_threshold))
				continue;

			// merge together
			Rect rc_merge = calcRect[j] | calcRect[i];
			// if angle valid
			if (!isRectValid(calcRect[j], rc_merge, angle_threshold))
				continue;

			//cout << "lalala" << endl;
			//resultRect.push_back(rc_merge);
			calcRect[j] = rc_merge;
		}
		///
	}



	for (int i = 0; i < (int)calcRect.size(); i++)
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		//drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		rectangle( drawing, calcRect[i].tl(), calcRect[i].br(), color, 1, 8, 0 );
		//circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
	}














	/// Show in a window
	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", drawing );


	cv::imwrite("bound_2.tif", drawing);

	//namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
	//imshow( "Display window", imgReader.m_data );                   // Show our image inside it.

	waitKey(0);   

	return 0;
}

