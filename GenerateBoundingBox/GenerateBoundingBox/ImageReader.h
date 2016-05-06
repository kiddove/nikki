#pragma once

/*
use gdal to open image file, and store data in cv::Mat
*/
#include <iostream>
#include <string>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gdal_priv.h"

class ImageReader
{
public:
	ImageReader();
	virtual ~ImageReader();
	bool LoadFile(std::string strFile);
	void CloseFile();

	//template<class T>
	// class member
	cv::Mat m_data;
private:
	std::string m_strFile;
	GDALDataset* m_pDataset;
public:
	static int getOpenCVType(GDALDataType gType, int iChannel);
	static int GetValue(cv::Mat& mat, int r, int c, int b);
	static void SetValue(cv::Mat& mat, int r, int c, int b, int& fValue);
};
