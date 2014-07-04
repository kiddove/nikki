// Clustering.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"

// open cv can not open envi tif~!!!
//const char* filename = "E:\\Files\\segment\\image_1.tif";
//const char* filename = "E:\\IMG_0847.JPG";
//const char* filename = "E:\\Code\\Github\\nikki\\GLCM\\s41.tif";
//const char* filename = "E:\\Files\\segment\\8bit_3bands_test_11.png";
const char* filename = "E:\\Files\\segment\\16bit_4bands_imagery_101.png";
using namespace cv;
using namespace std;

/* Pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported.  */

int _tmain(int argc, _TCHAR* argv[])
{
	Mat src = imread(filename, -1);
	// returns the pixel type
	/*
	#define CV_8U   0
	#define CV_8S   1
	#define CV_16U  2
	#define CV_16S  3
	#define CV_32S  4
	#define CV_32F  5
	#define CV_64F  6
	*/
	int idepth = src.depth();
	// pixel type && channels
	//eg : CV_16SC3 
	int itype = src.type();
	// band
	int ichannel = src.channels();

	//Mat samples(src.rows * src.cols, 3, CV_32F);

	if (src.data > 0)
	{
		//if (ichannel == 1)
		//{
		//	imshow("clustered image", src);
		//	waitKey(0);
		//	return 1;
		//}
		try
		{
			// do clustering
			Mat cluster(src.rows * src.cols, ichannel, idepth);

			for (int r =0; r < src.rows; r++)
			{
				for (int c = 0; c < src.cols; c++)
				{
					for (int b = 0; b < ichannel; b++)
					{
						//cluster.at<float>(r + c * src.rows, b) = src.at<Vec3b>(r, c)[b];
						cluster.at<float>(r + c * src.rows, 0) = src.at<unsigned short>(r + c * src.rows, 0);
					}
				}
			}

			int clustercount = 3;
			Mat labels;
			int attempts = 15;
			Mat centers;

			kmeans(cluster, clustercount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
			Mat new_image(src.size(), src.type());
			//Mat new_image(src.rows * src.cols, 1, CV_32F);
			for (int r = 0; r < src.rows; r++)
			{
				for (int c = 0; c < src.cols; c++)
				{
					int cluster_idx = labels.at<int>(r + c * src.rows, 0);
					for (int b = 0; b < ichannel; b++)
					{
						new_image.at<Vec3b>(r, c)[b] = (unsigned char)centers.at<float>(cluster_idx, b);
						//new_image.at<float>(cluster_idx, 0) = centers.at<float>(cluster_idx, 0);
					}
				}
			}
			imshow("clustered image", new_image);
		}
		catch (cv::Exception ce)
		{
			cout << ce.msg << endl;
		}
	}
	else
		cout << _T("open file failed.") << endl;
	waitKey(0);
	return 0;
}
