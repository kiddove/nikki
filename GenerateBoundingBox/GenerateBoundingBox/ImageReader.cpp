// ImageReader.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "ImageReader.h"
#include "trace.h"
#include <vector>
#include <map>
#include <stack>
#include <set>

using namespace std;
using namespace cv;
ImageReader::ImageReader()
{
	GDALAllRegister();
}

ImageReader::~ImageReader()
{
}

void ImageReader::CloseFile()
{
	// cv::Mat deallocate?
	m_strFile.clear();
	if (m_pDataset != NULL)
	{
		GDALClose(m_pDataset);
		m_pDataset = NULL;
	}
}

int ImageReader::getOpenCVType(GDALDataType gType, int iChannel)
{
	switch (gType)
	{
	case GDT_UInt16:
		return CV_16UC(iChannel);
		break;
	case GDT_Int16:
		return CV_16SC(iChannel);
		break;
	case GDT_Float32: 
		return CV_32FC(iChannel);
		break;
	case GDT_Byte:
		return CV_8UC(iChannel);
		break;
	case GDT_Float64:
		return CV_64FC(iChannel);
	case GDT_Int32:
		return CV_32SC(iChannel);
	default:
		return -1;
	}
}
bool ImageReader::LoadFile(string strFile)
{
	// open file
	m_pDataset = (GDALDataset*)GDALOpen(strFile.c_str(), GA_ReadOnly);
	if (m_pDataset == NULL)
	{
		return false;
	}

	// readfile
	//band channel
	int iChannel = m_pDataset->GetRasterCount();

	// image width
	int iWidth = m_pDataset->GetRasterXSize();
	// image height
	int iHeight = m_pDataset->GetRasterYSize();
	// image type
	GDALRasterBand* pBand = m_pDataset->GetRasterBand(1);
	if (pBand == NULL)
		return false;
	GDALDataType gType = pBand->GetRasterDataType();
	int iType = getOpenCVType(gType, iChannel);
	//int bType = getOpenCVType(gType, GDT_Byte);
	int bType = getOpenCVType(GDT_Int32, 1);
	m_data.create(iHeight, iWidth, bType);
	//std::vector<Mat> tobeMerged;
	//cv::Mat mt;
	//mt.create(iHeight, iWidth, bType);
	//tobeMerged.assign(iChannel, mt);
	for (int i = 1; i <= iChannel; i++)
	{
		GDALRasterBand* poBand = m_pDataset->GetRasterBand(i);
		if (poBand != NULL)
		{
			int iCount = iWidth * iHeight * GDALGetDataTypeSize(gType) / 8;
			char *pafScanline = (char*)CPLMalloc(iCount);
			poBand->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pafScanline, iWidth, iHeight, gType, 0, 0);
			Mat mtt(iHeight, iWidth, iType, pafScanline);
			//int i1 = mtt.at<byte>(0, 0);
			//Scalar s = cv::mean(mtt);
			//TRACE(_T("band:%d--mean:%.3f\n"), i, s.val[0]);
			//tobeMerged[i-1] = mtt.clone();
			//tobeMerged[i-1] = mtt;
			
			//Mat fMat;
			//fMat.create(iHeight, iWidth, bType);
			//tobeMerged.push_back(mtt.clone());

			double* p = (double*)pafScanline;
			//TRACE(_T("band %d:\n"), i-1);
			for (int ii = 0; ii < iHeight; ii++)
			{
				for (int jj = 0; jj < iWidth; jj++)
				{
					//fMat.at<float>(ii, jj) = (float)mtt.at<unsigned short>(ii, jj);
					double dValue = mtt.at<double>(ii, jj);
					int fValue = (int)dValue;
					SetValue(m_data, ii, jj, i-1, fValue);
					//TRACE(_T("%.2f, "), fValue);
					p++;
				}
				//TRACE(_T("\n"));
			}
			//tobeMerged.push_back(fMat);
			CPLFree(pafScanline);
		}
	}

	//TRACE(_T("--------------------------------\n"));
	//cv::Mat img;
	//merge(tobeMerged, m_data);
	//m_data = img.clone();
	//try
	//{
	//	img.convertTo(m_data, getOpenCVType(GDT_Float32, iChannel));
	//}catch (cv::Exception e) {
	//	TRACE("error: %s\n", e.what());
	//	return false;
	//}
	//img.assignTo(m_data, getOpenCVType(GDT_Float32, iChannel));
	//imshow(_T("test"), m_data);
	m_strFile = strFile;

	////CV_LOAD_IMAGE_ANYDEPTH 
	//cv::Mat R = cv::imread(strFile, -1);
	////imshow(_T("test"), R);
	//int iit = R.type();
	//int iid = R.depth();
	//int iic = R.channels();
	//int iii = m_data.at<Vec<uchar, 1>>(0, 0)[0];

	//for (int kk = 0; kk < iChannel; kk++)
	//{
	//	TRACE(_T("band %d:\n"), kk);
	//	for (int ii = 0; ii < iHeight; ii++)
	//	{
	//		for (int jj = 0; jj < iWidth; jj++)
	//		{
	//			TRACE(_T("%d, "), m_data.at<Vec<uchar, 3>>(ii, jj)[kk]);
	//		}
	//		TRACE(_T("\n"));
	//	}
	//}
	//int ii = R.at<Vec<uchar, 1>>(0, 0)[0];
	return true;
}


//#include "stdafx.h"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//
//#include <cstdio>
//#include <iostream>
//
//using namespace cv;
//using namespace std;
//
//static void help()
//{
//    cout << "\nThis program demonstrates the famous watershed segmentation algorithm in OpenCV: watershed()\n"
//            "Usage:\n"
//            "./watershed [image_name -- default is fruits.jpg]\n" << endl;
//
//
//    cout << "Hot keys: \n"
//        "\tESC - quit the program\n"
//        "\tr - restore the original image\n"
//        "\tw or SPACE - run watershed segmentation algorithm\n"
//        "\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
//        "\t  (before that, roughly outline several markers on the image)\n";
//}
//Mat markerMask, img;
//Point prevPt(-1, -1);
//
//static void onMouse( int event, int x, int y, int flags, void* )
//{
//    if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
//        return;
//    if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
//        prevPt = Point(-1,-1);
//    else if( event == CV_EVENT_LBUTTONDOWN )
//        prevPt = Point(x,y);
//    else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
//    {
//        Point pt(x, y);
//        if( prevPt.x < 0 )
//            prevPt = pt;
//        line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
//        line( img, prevPt, pt, Scalar::all(255), 5, 8, 0 );
//        prevPt = pt;
//        imshow("image", img);
//    }
//}
//
//int main( int argc, char** argv )
//{
//    char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
//    Mat img0 = imread(filename, 1), imgGray;
//
//    if( img0.empty() )
//    {
//        cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
//        return 0;
//    }
//    help();
//    namedWindow( "image", 1 );
//
//    img0.copyTo(img);
//    cvtColor(img, markerMask, COLOR_BGR2GRAY);
//    cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);
//    markerMask = Scalar::all(0);
//    imshow( "image", img );
//    setMouseCallback( "image", onMouse, 0 );
//
//    for(;;)
//    {
//        int c = waitKey(0);
//
//        if( (char)c == 27 )
//            break;
//
//        if( (char)c == 'r' )
//        {
//            markerMask = Scalar::all(0);
//            img0.copyTo(img);
//            imshow( "image", img );
//        }
//
//        if( (char)c == 'w' || (char)c == ' ' )
//        {
//            int i, j, compCount = 0;
//            vector<vector<Point> > contours;
//            vector<Vec4i> hierarchy;
//
//            findContours(markerMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
//
//            if( contours.empty() )
//                continue;
//            Mat markers(markerMask.size(), CV_32S);
//            markers = Scalar::all(0);
//            int idx = 0;
//            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
//                drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
//
//            if( compCount == 0 )
//                continue;
//
//            vector<Vec3b> colorTab;
//            for( i = 0; i < compCount; i++ )
//            {
//                int b = theRNG().uniform(0, 255);
//                int g = theRNG().uniform(0, 255);
//                int r = theRNG().uniform(0, 255);
//
//                colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
//            }
//
//            double t = (double)getTickCount();
//            watershed( img0, markers );
//            t = (double)getTickCount() - t;
//            printf( "execution time = %gms\n", t*1000./getTickFrequency() );
//
//            Mat wshed(markers.size(), CV_8UC3);
//
//            // paint the watershed image
//            for( i = 0; i < markers.rows; i++ )
//                for( j = 0; j < markers.cols; j++ )
//                {
//                    int index = markers.at<int>(i,j);
//                    if( index == -1 )
//                        wshed.at<Vec3b>(i,j) = Vec3b(255,255,255);
//                    else if( index <= 0 || index > compCount )
//                        wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
//                    else
//                        wshed.at<Vec3b>(i,j) = colorTab[index - 1];
//                }
//
//            wshed = wshed*0.5 + imgGray*0.5;
//            imshow( "watershed transform", wshed );
//        }
//    }
//
//    return 0;
//}

int ImageReader::GetValue(Mat& mat, int r, int c, int b)
{
	if (!mat.isContinuous())
		return 0.0;
	if (r < 0 || r >= mat.rows)
		return 0.0;
	if (c < 0 || c >= mat.cols)
		return 0.0;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return 0.0;

	int * p = (int*)mat.ptr(r);

	int f = (int)p[c * iChannel + b];
	return f;
}

void ImageReader::SetValue(Mat& mat, int r, int c, int b, int& fValue)
{
	if (!mat.isContinuous())
		return;
	if (r < 0 || r >= mat.rows)
		return;
	if (c < 0 || c >= mat.cols)
		return;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return;

	int * p = (int*)mat.ptr(r);

	//float f = (float)p[c * iChannel + b];
	//return f;
	p[c * iChannel + b] = fValue;
}