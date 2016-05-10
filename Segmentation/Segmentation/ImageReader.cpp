// ImageReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ImageReader.h"

ImageReader::ImageReader()
{
	GDALAllRegister();
	//m_dp.iMax = 0;
	//m_dp.fMax = 0.0;
	m_pDataset = NULL;
}

ImageReader::~ImageReader()
{
	CloseFile();
}

MaskReader::MaskReader()
{
}

MaskReader::~MaskReader()
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
	m_mean.clear();
	m_stand_deviation.clear();
	m_variogram.clear();
	m_dp.clear();
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
	int bType = getOpenCVType(gType, 1);
	//m_data.create(iHeight, iWidth, iType);
	std::vector<Mat> tobeMerged;
	cv::Mat mt;
	mt.create(iHeight, iWidth, bType);
	tobeMerged.assign(iChannel, mt);
	for (int i = 1; i <= iChannel; i++)
	{
		GDALRasterBand* poBand = m_pDataset->GetRasterBand(i);
		if (poBand != NULL)
		{
			int iCount = iWidth * iHeight * GDALGetDataTypeSize(gType) / 8;
			char *pafScanline = (char*)CPLMalloc(iCount);
			poBand->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pafScanline, iWidth, iHeight, gType, 0, 0);
			Mat mtt(iHeight, iWidth, bType, pafScanline);
			//int i1 = mtt.at<byte>(0, 0);
			Scalar s = cv::mean(mtt);
			//TRACE(_T("band:%d--mean:%.3f\n"), i, s.val[0]);
			tobeMerged[i-1] = mtt.clone();

			//char* p = pafScanline;
			//TRACE(_T("band %d:\n"), i-1);
			//for (int ii = 0; ii < iHeight; ii++)
			//{
			//	for (int jj = 0; jj < iWidth; jj++)
			//	{
			//		ASSERT((byte)*p == mtt.at<byte>(ii, jj));
			//		TRACE(_T("%d, "), (byte)*p);
			//		p++;
			//	}
			//	TRACE(_T("\n"));
			//}
			CPLFree(pafScanline);
		}
	}

	//TRACE(_T("--------------------------------\n"));
	cv::Mat img;
	merge(tobeMerged, img);
	//m_data = img.clone();
	img.convertTo(m_data, getOpenCVType(GDT_Float32, iChannel));
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
bool MaskReader::LoadFile(string strFile)
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
	if (iChannel != 1)
		return false;
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

	GDALRasterBand* poBand = m_pDataset->GetRasterBand(1);
	if (poBand != NULL)
	{
		int iCount = iWidth * iHeight * GDALGetDataTypeSize(gType) / 8;
		char *pafScanline = (char*)CPLMalloc(iCount);
		pBand->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pafScanline, iWidth, iHeight, gType, 0, 0);

		Mat mt(iHeight, iWidth, iType, pafScanline);
		m_data = mt.clone();
		CPLFree(pafScanline);
	}

	int isum1 = 0;
	for (int row = 0; row < iHeight; row++)
	{
		//float* fp = mt.ptr<float>(j);
		for (int col = 0; col < iWidth; col++)
		{
			int iValue = m_data.at<unsigned char>(row, col);
			if (iValue == 1)
			{
				isum1++;
				m_data.at<unsigned char>(row, col) = 255;
				//CString str;
				//str.Format("%d, %d\n", row, col);
				//::OutputDebugString(str);
			}
		}
	}

	m_strFile = strFile;
	return true;
}

//template<class T> bool MaskReader::GetMask(map<int, set<int>>& mapMask, T tMask, bool b4Neighbour)
//{
//	if (b4Neighbour)
//		return _GetMask4(mapMask, tMask);
//	else
//		return _GetMask8(mapMask, tMask);
//}
//template<class T> bool MaskReader::_GetMask4(map<int, set<int>>& mapMask, T tMask)
//{
//	int iNumber = 11;
//	// find connected area
//	stack<Point> stRegion;
//	for (int row = 0; row < m_data.rows; row++)
//	{
//		//float* fp = mt.ptr<float>(j);
//		for (int col = 0; col < m_data.cols; col++)
//		{
//			T vl = m_data.at<T>(row, col);
//			//if (m_data.at<Vec<uchar, 1>>(row, col)[0] == 1)
//			if (m_data.at<T>(row, col) == tMask)
//			{
//				//(x, y) -- col, row -- k, j
//				Point pt(col, row);
//				stRegion.push(pt);
//				vector<Point> vecRegion;
//				set<int> setRegion;
//				while (!stRegion.empty())
//				{
//					Point ptCur = stRegion.top();
//					m_data.at<T>(ptCur) = iNumber;
//					stRegion.pop();
//					vecRegion.push_back(ptCur);
//					setRegion.insert(ptCur.y * m_data.cols + ptCur.x);
//					// push all neighbours(==1) into stack
//					// 4 neighbour or 8neighbour?
//					//// row-1, col-1 
//					//if (ptCur.y - 1 >= 0 && ptCur.x - 1 >=0)
//					//{
//					//	Point ptNeighbour(ptCur.x - 1, ptCur.y - 1);
//					//	if (m_data.at<T>(ptNeighbour) == tMask)
//					//		stRegion.push(ptNeighbour);
//					//}
//					// row-1, col----4
//					if (ptCur.y - 1 >= 0)
//					{
//						Point ptNeighbour(ptCur.x, ptCur.y - 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					//// row-1, col+1
//					//if (ptCur.y - 1 >= 0 && ptCur.x + 1 < m_data.cols)
//					//{
//					//	Point ptNeighbour(ptCur.x + 1, ptCur.y - 1);
//					//	if (m_data.at<T>(ptNeighbour) == tMask)
//					//		stRegion.push(ptNeighbour);
//					//}
//					// row, col-1----4
//					if ( ptCur.x - 1 >=0)
//					{
//						Point ptNeighbour(ptCur.x - 1, ptCur.y);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row, col+1----4
//					if (ptCur.x + 1 < m_data.rows)
//					{
//						Point ptNeighbour(ptCur.x + 1, ptCur.y);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					//// row+1, col-1
//					//if (ptCur.y + 1 < m_data.rows && ptCur.x - 1 >=0)
//					//{
//					//	Point ptNeighbour(ptCur.x - 1, ptCur.y + 1);
//					//	if (m_data.at<T>(ptNeighbour) == tMask)
//					//		stRegion.push(ptNeighbour);
//					//}
//					// row+1, col----4
//					if (ptCur.y + 1 < m_data.rows)
//					{
//						Point ptNeighbour(ptCur.x, ptCur.y + 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					//// row+1, col+1
//					//if (ptCur.y + 1 < m_data.rows && ptCur.x + 1 < m_data.cols)
//					//{
//					//	Point ptNeighbour(ptCur.x + 1, ptCur.y + 1);
//					//	if (m_data.at<T>(ptNeighbour) == tMask)
//					//		stRegion.push(ptNeighbour);
//					//}
//				}
//
//				if (setRegion.size() > 1)
//				{
//					mapMask.insert(make_pair(iNumber, setRegion));
//					iNumber++;
//				}
//			}
//		}
//	}
//	return true;
//}
//
//template<class T> bool MaskReader::_GetMask8(map<int, set<int>>& mapMask, T tMask)
//{
//	T iNumber = 11;
//	// find connected area
//	stack<Point> stRegion;
//	for (int row = 0; row < m_data.rows; row++)
//	{
//		//float* fp = mt.ptr<float>(j);
//		for (int col = 0; col < m_data.cols; col++)
//		{
//			//if (m_data.at<Vec<uchar, 1>>(row, col)[0] == 1)
//			T vl = m_data.at<T>(row, col);
//			if (m_data.at<T>(row, col) == tMask)
//			{
//				//(x, y) -- col, row -- k, j
//				Point pt(col, row);
//				stRegion.push(pt);
//				//++iNumber;
//				vector<Point> vecRegion;
//				set<int> setRegion;
//				while (!stRegion.empty())
//				{
//					Point ptCur = stRegion.top();
//					m_data.at<T>(ptCur) = iNumber;
//					stRegion.pop();
//					vecRegion.push_back(ptCur);
//					setRegion.insert(ptCur.y * m_data.cols + ptCur.x);
//					// push all neighbours(==1) into stack
//					// 4 neighbour or 8neighbour?
//					// row-1, col-1 
//					if (ptCur.y - 1 >= 0 && ptCur.x - 1 >=0)
//					{
//						Point ptNeighbour(ptCur.x - 1, ptCur.y - 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row-1, col----4
//					if (ptCur.y - 1 >= 0)
//					{
//						Point ptNeighbour(ptCur.x, ptCur.y - 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row-1, col+1
//					if (ptCur.y - 1 >= 0 && ptCur.x + 1 < m_data.cols)
//					{
//						Point ptNeighbour(ptCur.x + 1, ptCur.y - 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row, col-1----4
//					if ( ptCur.x - 1 >=0)
//					{
//						Point ptNeighbour(ptCur.x - 1, ptCur.y);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row, col+1----4
//					if (ptCur.x + 1 < m_data.rows)
//					{
//						Point ptNeighbour(ptCur.x + 1, ptCur.y);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row+1, col-1
//					if (ptCur.y + 1 < m_data.rows && ptCur.x - 1 >=0)
//					{
//						Point ptNeighbour(ptCur.x - 1, ptCur.y + 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row+1, col----4
//					if (ptCur.y + 1 < m_data.rows)
//					{
//						Point ptNeighbour(ptCur.x, ptCur.y + 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//					// row+1, col+1
//					if (ptCur.y + 1 < m_data.rows && ptCur.x + 1 < m_data.cols)
//					{
//						Point ptNeighbour(ptCur.x + 1, ptCur.y + 1);
//						if (m_data.at<T>(ptNeighbour) == tMask)
//							stRegion.push(ptNeighbour);
//					}
//				}
//
//				if (setRegion.size() > 1)
//				{
//					mapMask.insert(make_pair(iNumber, setRegion));
//					iNumber++;
//				}
//			}
//		}
//	}
//	return true;
//}

bool ImageReader::Statistic()
{
	//m_mean.clear();
	//m_variogram.clear();
	//m_dp.clear();
	//m_dp.fMax = 0.0;
	//m_dp.fMin = 1e8;
	if (m_data.data == NULL)
		return false;
	if (!m_mean.empty())
		return true;
	Scalar s = cv::mean(m_data);
	int iChannel = m_data.channels();
	int iWidth = m_data.cols;
	int iHeight = m_data.rows;

	// mean
	for (int b = 0; b < iChannel; b++)
	{
		// mean
		m_mean.push_back((float)s.val[b]);
		// variance
		float fStandDeviation = 0.0f;
		for (int r = 0; r < iHeight; r++)
		{
			for (int c = 0; c < iWidth; c++)
			{
				float v = GetValue(r, c, b);
				fStandDeviation += (v - m_mean[b]) * (v - m_mean[b]);
			}
		}
		fStandDeviation /= (float)(iHeight * iWidth);
		fStandDeviation = sqrt(fStandDeviation);
		m_stand_deviation.push_back(fStandDeviation);
	}



	int dCount_row = iHeight / 2;
	int dCount_col = iWidth / 2;

	//int dCount = min(dCount_col, dCount_row);

	int dCount = 50;
	float fHorizontal, fVertical;
	fHorizontal = fVertical = 0.0;

	float fVariogram = 0.0;
	//m_dp.iMax = dCount;
	for (int b = 0; b < iChannel; b++)
	{
		drawParam dp;
		dp.iMax = dCount;
		dp.fMax = 0.0;
		dp.fMin = 1e8;
		std::vector<float> bandVariogram;
		for (int i = 1; i <= dCount; i++)
		{
			fHorizontal = fVertical = 0.0;
			dCount_row = dCount_col = 0;
			fVariogram = 0.0;
			//for (int r = 0; r < iHeight - dCount; r++)
			for (int r = 0; r < iHeight; r++)
			{
				for (int c = 0; c < iWidth - dCount; c++)
				//for (int c = 0; c < iWidth; c++)
				{
					float v = GetValue(r, c, b);
					float v_h = GetValue(r, c + i, b);
					float v_v = GetValue(r + i, c, b);
					fHorizontal += (v_h - v) * (v_h - v);
					fVertical += (v_v - v) * (v_v - v);
					dCount_row++;
					dCount_col++;
				}
			}

			//fVariogram = fHorizontal + fVertical;
			//fHorizontal = fHorizontal / (float)dCount_col / 2.0f;
			//fVertical = fVertical / (float)dCount_row / 2.0f;

			//fVariogram = fVariogram / (float)(dCount_col + dCount_row) / 2.0f;
			fVariogram = fVertical / (float)dCount_row / 2.0f;
			//fVariogram = sqrt(fVariogram);
			if (fVariogram > dp.fMax)
				dp.fMax = fVariogram;
			if (fVariogram < dp.fMin)
				dp.fMin = fVariogram;
			//m_variogram.insert(std::make_pair(i, fVariogram));
			bandVariogram.push_back(fVariogram);
		}
		m_variogram.push_back(bandVariogram);
		m_dp.push_back(dp);
	}
	return true;
}

float ImageReader::GetValue(int r, int c, int b)
{
	if (!m_data.isContinuous())
		return 0.0;
	if (r < 0 || r >= m_data.rows)
		return 0.0;
	if (c < 0 || c >= m_data.cols)
		return 0.0;
	int iChannel = m_data.channels();
	if (b < 0 || b >= iChannel)
		return 0.0;

	float * p = (float*)m_data.ptr(r);

	float f = (float)p[c * iChannel + b];
	return f;
}