// GLCM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FeatureDetect.h"
#include "opencv\cv.h"
#include "opencv\cvaux.h" 
//#include <opencv2\highgui\highgui_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const char* filename = "E:\\Code\\Github\\nikki\\GLCM\\s41nir.tif"; 
const CString output = "E:\\Code\\Github\\nikki\\GLCM\\result.txt";

int _tmain(int argc, _TCHAR* argv[])
{
	//IplImage* dst = NULL;
	IplImage* dst = cvLoadImage(filename,CV_LOAD_IMAGE_UNCHANGED);	

	Mat a (dst);

	if (dst == NULL)
		return -1;
	int selSize = 51;	
	int selAngle = 0;	
	ZBGLCM glcm;	
	double Sa[5] = {0}; 

	glcm.SetNum(dst->depth);

	glcm.GLCMFeature(((MYTYPE*)dst->imageData),dst->width,dst->height,selSize,selAngle);
/*	Sa[0] = glcm.GLCMFeature(((unsigned char *)dst->imageData),dst->width,dst->height,selSize,selAngle).dCorrelation;
	Sa[1] = glcm.GLCMFeature(((unsigned char *)dst->imageData),dst->width,dst->height,selSize,selAngle).dEnergy;
	Sa[2] = glcm.GLCMFeature(((unsigned char *)dst->imageData),dst->width,dst->height,selSize,selAngle).dEntropy;
	Sa[3] = glcm.GLCMFeature(((unsigned char *)dst->imageData),dst->width,dst->height,selSize,selAngle).dInertiaQuadrature;
	Sa[4] = glcm.GLCMFeature(((unsigned char *)dst->imageData),dst->width,dst->height,selSize,selAngle).dLocalCalm;	
	cout<<"相关："<<Sa[0]<<endl<<"能量："<<Sa[1]<<endl<<"熵："<<Sa[2]<<endl<<"对比度："<<Sa[3]<<endl<<"逆差距："<<Sa[4]<<endl;*/	


	//CStdioFile f;
	//f.Open(output, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);
	//CString str1, str2;

	//f.WriteString("RD\n");

	//for (int i = 0; i < glcm.GrayLayerNum; i++)
	//{
	//	str1.Empty();
	//	for (int j =0 ; j < glcm.GrayLayerNum; j++)
	//	{
	//		str2.Empty();
	//		str2.Format(_T("%d, "), glcm.PMatrixRD[i][j]);
	//		str1 += str2;
	//	}
	//	str1 += _T("\n");
	//	f.WriteString(str1);
	//}

	//f.WriteString("LD\n");

	//for (int i = 0; i < glcm.GrayLayerNum; i++)
	//{
	//	str1.Empty();
	//	for (int j =0 ; j < glcm.GrayLayerNum; j++)
	//	{
	//		str2.Empty();
	//		str2.Format(_T("%d, "), glcm.PMatrixLD[i][j]);
	//		str1 += str2;
	//	}
	//	str1 += _T("\n");
	//	f.WriteString(str1);
	//}

	//f.WriteString("V\n");

	//for (int i = 0; i < glcm.GrayLayerNum; i++)
	//{
	//	str1.Empty();
	//	for (int j =0 ; j < glcm.GrayLayerNum; j++)
	//	{
	//		str2.Empty();
	//		str2.Format(_T("%d, "), glcm.PMatrixV[i][j]);
	//		str1 += str2;
	//	}
	//	str1 += _T("\n");
	//	f.WriteString(str1);
	//}

	//f.WriteString("H\n");

	//for (int i = 0; i < glcm.GrayLayerNum; i++)
	//{
	//	str1.Empty();
	//	for (int j =0 ; j < glcm.GrayLayerNum; j++)
	//	{
	//		str2.Empty();
	//		str2.Format(_T("%d, "), glcm.PMatrixH[i][j]);
	//		str1 += str2;
	//	}
	//	str1 += _T("\n");
	//	f.WriteString(str1);
	//}
	//f.Close();
	//


	return 0;
}

