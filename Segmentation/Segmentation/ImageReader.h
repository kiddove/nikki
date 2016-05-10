#pragma once

/*
use gdal to open image file, and store data in cv::Mat
*/
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gdal_priv.h"

using namespace std;
using namespace cv;

struct drawParam
{
	int iMax;
	float fMax;
	float fMin;
};

class ImageReader
{
public:
	ImageReader();
	virtual ~ImageReader();
	bool virtual LoadFile(string strFile);
	void CloseFile();

	// calculate mean value and variogram, draw a bmp of variogram
	bool Statistic();
	//template<class T>
	// class member
	cv::Mat m_data;
protected:
	std::string m_strFile;
	GDALDataset* m_pDataset;
	int getOpenCVType(GDALDataType gType, int iChannel);
	float GetValue(int r, int c, int b);
public:
	std::vector<float> m_mean;
	std::vector<float> m_stand_deviation;
	//std::map<int, float> m_variogram;
	std::vector<std::vector<float>> m_variogram;
	std::vector<drawParam> m_dp;
};

class MaskReader : public ImageReader
{
public:
	MaskReader();
	virtual ~MaskReader();
	bool virtual LoadFile(string strFile);

	template<class T> bool GetMask(map<int, set<int>>& mapMask, T tMask, bool b4Neighbour = false)
	{
		if (b4Neighbour)
			return _GetMask4(mapMask, tMask);
		else
			return _GetMask8(mapMask, tMask);
	}
	void CloseFile();
private:
	template<class T> bool _GetMask4(map<int, set<int>>& mapMask, T tMask)
	{
		int iNumber = 11;
		// find connected area
		stack<Point> stRegion;
		for (int row = 0; row < m_data.rows; row++)
		{
			//float* fp = mt.ptr<float>(j);
			for (int col = 0; col < m_data.cols; col++)
			{
				T vl = m_data.at<T>(row, col);
				//if (m_data.at<Vec<uchar, 1>>(row, col)[0] == 1)
				if (m_data.at<T>(row, col) == tMask)
				{
					//(x, y) -- col, row -- k, j
					Point pt(col, row);
					stRegion.push(pt);
					vector<Point> vecRegion;
					set<int> setRegion;
					while (!stRegion.empty())
					{
						Point ptCur = stRegion.top();
						m_data.at<T>(ptCur) = iNumber;
						stRegion.pop();
						vecRegion.push_back(ptCur);
						setRegion.insert(ptCur.y * m_data.cols + ptCur.x);
						// push all neighbours(==1) into stack
						// 4 neighbour or 8neighbour?
						//// row-1, col-1 
						//if (ptCur.y - 1 >= 0 && ptCur.x - 1 >=0)
						//{
						//	Point ptNeighbour(ptCur.x - 1, ptCur.y - 1);
						//	if (m_data.at<T>(ptNeighbour) == tMask)
						//		stRegion.push(ptNeighbour);
						//}
						// row-1, col----4
						if (ptCur.y - 1 >= 0)
						{
							Point ptNeighbour(ptCur.x, ptCur.y - 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						//// row-1, col+1
						//if (ptCur.y - 1 >= 0 && ptCur.x + 1 < m_data.cols)
						//{
						//	Point ptNeighbour(ptCur.x + 1, ptCur.y - 1);
						//	if (m_data.at<T>(ptNeighbour) == tMask)
						//		stRegion.push(ptNeighbour);
						//}
						// row, col-1----4
						if ( ptCur.x - 1 >=0)
						{
							Point ptNeighbour(ptCur.x - 1, ptCur.y);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row, col+1----4
						if (ptCur.x + 1 < m_data.rows)
						{
							Point ptNeighbour(ptCur.x + 1, ptCur.y);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						//// row+1, col-1
						//if (ptCur.y + 1 < m_data.rows && ptCur.x - 1 >=0)
						//{
						//	Point ptNeighbour(ptCur.x - 1, ptCur.y + 1);
						//	if (m_data.at<T>(ptNeighbour) == tMask)
						//		stRegion.push(ptNeighbour);
						//}
						// row+1, col----4
						if (ptCur.y + 1 < m_data.rows)
						{
							Point ptNeighbour(ptCur.x, ptCur.y + 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						//// row+1, col+1
						//if (ptCur.y + 1 < m_data.rows && ptCur.x + 1 < m_data.cols)
						//{
						//	Point ptNeighbour(ptCur.x + 1, ptCur.y + 1);
						//	if (m_data.at<T>(ptNeighbour) == tMask)
						//		stRegion.push(ptNeighbour);
						//}
					}

					if (setRegion.size() > 1)
					{
						mapMask.insert(make_pair(iNumber, setRegion));
						iNumber++;
					}
				}
			}
		}
		return true;
	}

	template<class T> bool _GetMask8(map<int, set<int>>& mapMask, T tMask)
	{
		T iNumber = 11;
		// find connected area
		stack<Point> stRegion;
		for (int row = 0; row < m_data.rows; row++)
		{
			//float* fp = mt.ptr<float>(j);
			for (int col = 0; col < m_data.cols; col++)
			{
				//if (m_data.at<Vec<uchar, 1>>(row, col)[0] == 1)
				T vl = m_data.at<T>(row, col);
				if (m_data.at<T>(row, col) == tMask)
				{
					//(x, y) -- col, row -- k, j
					Point pt(col, row);
					stRegion.push(pt);
					//++iNumber;
					vector<Point> vecRegion;
					set<int> setRegion;
					while (!stRegion.empty())
					{
						Point ptCur = stRegion.top();
						m_data.at<T>(ptCur) = iNumber;
						stRegion.pop();
						vecRegion.push_back(ptCur);
						setRegion.insert(ptCur.y * m_data.cols + ptCur.x);
						// push all neighbours(==1) into stack
						// 4 neighbour or 8neighbour?
						// row-1, col-1 
						if (ptCur.y - 1 >= 0 && ptCur.x - 1 >=0)
						{
							Point ptNeighbour(ptCur.x - 1, ptCur.y - 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row-1, col----4
						if (ptCur.y - 1 >= 0)
						{
							Point ptNeighbour(ptCur.x, ptCur.y - 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row-1, col+1
						if (ptCur.y - 1 >= 0 && ptCur.x + 1 < m_data.cols)
						{
							Point ptNeighbour(ptCur.x + 1, ptCur.y - 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row, col-1----4
						if ( ptCur.x - 1 >=0)
						{
							Point ptNeighbour(ptCur.x - 1, ptCur.y);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row, col+1----4
						if (ptCur.x + 1 < m_data.rows)
						{
							Point ptNeighbour(ptCur.x + 1, ptCur.y);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row+1, col-1
						if (ptCur.y + 1 < m_data.rows && ptCur.x - 1 >=0)
						{
							Point ptNeighbour(ptCur.x - 1, ptCur.y + 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row+1, col----4
						if (ptCur.y + 1 < m_data.rows)
						{
							Point ptNeighbour(ptCur.x, ptCur.y + 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
						// row+1, col+1
						if (ptCur.y + 1 < m_data.rows && ptCur.x + 1 < m_data.cols)
						{
							Point ptNeighbour(ptCur.x + 1, ptCur.y + 1);
							if (m_data.at<T>(ptNeighbour) == tMask)
								stRegion.push(ptNeighbour);
						}
					}

					if (setRegion.size() > 1)
					{
						mapMask.insert(make_pair(iNumber, setRegion));
						iNumber++;
					}
				}
			}
		}
		return true;
	}
	
};
