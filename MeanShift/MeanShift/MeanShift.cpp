#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <vector>
#include "windows.h"		// for GetTickCount
#include <vector>
#include <set>
#include <map>
#include <algorithm>
using namespace std;
using namespace cv;

//int threshold_distance = 5;
std::vector<float> GetMatrixValueFloat(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	float* p = m.ptr<float>(r);
	std::vector<float> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}
std::vector<int> GetMatrixValueInt(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	int* p = m.ptr<int>(r);
	std::vector<int> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}
std::vector<ushort> GetMatrixValueUShort(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	ushort* p = m.ptr<ushort>(r);
	std::vector<ushort> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}

std::vector<short> GetMatrixValueShort(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	short* p = m.ptr<short>(r);
	std::vector<short> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}

std::vector<char> GetMatrixValueChar(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	char* p = m.ptr<char>(r);
	std::vector<char> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}

std::vector<uchar> GetMatrixValueUchar(Mat& m, int r, int c)
{
	int iChannel = m.channels();
	uchar* p = m.ptr<uchar>(r);
	std::vector<uchar> vecValue;
	if (p != NULL)
	{
	for (int b = 0; b < iChannel; b++)
		vecValue.push_back(p[c * iChannel + b]);
	}
	return vecValue;
}

// b for specific
void SetMatrixValueFloat(Mat& m, int r, int c, int b, float fValue)
{
	int iChannel = m.channels();
	float* p = m.ptr<float>(r);
	p[c * iChannel + b] = fValue;

}

float GetMatrixValueFloat(Mat& m, int r, int c, int b)
{
	int iChannel = m.channels();
	float* p = m.ptr<float>(r);
	if (p != NULL)
	{
		return p[c * iChannel + b];
	}
	return -1.0;
}
void SetMatrixValueUchar(Mat& m, int r, int c, int b, unsigned char uValue)
{
	int iChannel = m.channels();
	uchar* p = m.ptr<uchar>(r);
	p[c * iChannel + b] = uValue;

}
uchar GetMatrixValueUchar(Mat& m, int r, int c, int b)
{
	int iChannel = m.channels();
	uchar* p = m.ptr<uchar>(r);
	if (p != NULL)
	{
		return p[c * iChannel + b];
	}
	return 0;
}

void SetMatrixValueUshort(Mat& m, int r, int c, int b, ushort uValue)
{
	int iChannel = m.channels();
	ushort* p = m.ptr<ushort>(r);
	p[c * iChannel + b] = uValue;

}
ushort GetMatrixValueUshort(Mat& m, int r, int c, int b)
{
	int iChannel = m.channels();
	ushort* p = m.ptr<ushort>(r);
	if (p != NULL)
	{
		return p[c * iChannel + b];
	}
	return 0;
}
//void SetMatrixValueInt(Mat& m, int r, int c, int b, int uValue)
//{
//	int iChannel = m.channels();
//	int* p = m.ptr<int>(r);
//	p[c * iChannel + b] = uValue;
//
//}
//int GetMatrixValueInt(Mat& m, int r, int c, int b)
//{
//	int iChannel = m.channels();
//	int* p = m.ptr<int>(r);
//	if (p != NULL)
//	{
//		return p[c * iChannel + b];
//	}
//	return -1;
//}

//void SetTag(Mat& m, std::vector<int>& vecRange, int iNum)
//void SetTag(Mat& m, std::vector<int>& vecRange, ushort iNum)
void SetTag(Mat& m, std::set<int>& vecRange, ushort iNum)
{
	for (std::set<int>::iterator iter = vecRange.begin(); iter != vecRange.end(); iter++)
	{
		int r = *iter / m.cols;
		int c = *iter % m.cols;
		//SetMatrixValueInt(m, r, c, 0, iNum);
		SetMatrixValueUshort(m, r, c, 0, iNum);
	}
}

//void ReplaceTag(Mat& m, int tagnew, int tagold)
void ReplaceTag(Mat& m, ushort tagnew, ushort tagold)
{
	int iChannel = m.channels();

	for (int r = 0; r < m.rows; r++)
	{
		for (int c = 0; c < m.cols; c++)
		{
			//int* p = m.ptr<int>(r);
			ushort* p = m.ptr<ushort>(r);
			if (p[c * iChannel + 0] == tagold)
				p[c * iChannel + 0] = tagnew;
		}
	}
}

struct Pixel
{
	////unsigned char * pData;
	//unsigned char x;
	//unsigned char y;
	//unsigned char z;
	std::vector<float> vecData;
	int iIndex;

	//bool operator < (const Pixel& p) const
	//{
	//	if (x < p.x)
	//		return true;
	//	else
	//	{
	//		if (y < p.y)
	//			return true;
	//		else
	//		{
	//			if (z < p.z)
	//				return true;
	//		}
	//	}
	//	return false;
	//}
};

float GetDistance(Pixel& p1, Pixel& p2)
{
	//return (float)sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
	double dSum = 0.0;
	int iSize = p1.vecData.size();
	for (int i = 0; i < iSize; i++)
	{
		dSum += (p1.vecData[i] - p2.vecData[i]) * (p1.vecData[i] - p2.vecData[i]);
	}
	return (float)sqrt(dSum);
}

float GetDistance(std::vector<uchar> vecValue1, std::vector<uchar> vecValue2)
{
	double dSum = 0.0;
	for (std::vector<uchar>::iterator iter1 = vecValue1.begin(), iter2 = vecValue2.begin(); iter1 != vecValue1.end() || iter2 != vecValue2.end(); iter1++, iter2++)
	{
		dSum += (*iter1 - *iter2) * (*iter1 - *iter2);
	}
	return (float)sqrt(dSum);
}
bool pixelCompare(Pixel& p1, Pixel& p2)
{

	return false;
	//int iSize = p1.vecData.size();
	//for (int i = 0; i < iSize;)
	//{
	//	if (p1.vecData[i] < p2.vecData[i])
	//		return true;
	//	else if (p1.vecData[i] > p2.vecData[i])
	//		return false;
	//	else
	//		i++;
	//}


	// 这样排序是错误的, 以threshold=100为例
	// 1, 0, 136
	// 1, 3, 35
	// 1, 3, 181 ***
	// 1, 4, 32
	// 1, 6, 129

	//if (p1.x < p2.x)
	//	return true;
	//else if (p1.x > p2.x)
	//	return false;
	//else
	//{
	//	if (p1.y < p2.y)
	//		return true;
	//	else if (p1.y > p2.y)
	//		return false;
	//	else 
	//	{
	//		return p1.z < p2.z;
	//	}
	//}
	//return false;


	//// 按照到(0,0,0)的距离排序， 这样也是错的
	//if ((p1.x * p1.x + p1.y * p1.y + p1.z * p1.z) < (p1.x * p1.x + p1.y * p1.y + p1.z * p1.z))
	//	return true;
	//else
	//	return false;

}

struct thParam
{
	int start;
	int end;
	int size;
	int cols;
	int threshold_distance;
	Mat* pMatMean;
	std::vector<Pixel>* pVec;
};

DWORD WINAPI Thread_Proc(LPVOID lpParam)
{
	thParam* pParam = (thParam*)lpParam;
	if (pParam == NULL)
		return -1;

	for (int i = pParam->start; i <= pParam->end; i++)
	{
		double pt_dis = 0.0;
		int iCount = 1;
		double iSum = 0;
		for (int j = i - 1; j >= 0; j--)
		{
			//pt_dis = sqrt((vecPoint[i].x - vecPoint[j].x) * (vecPoint[i].x - vecPoint[j].x)
			//	+(vecPoint[i].y - vecPoint[j].y) * (vecPoint[i].y - vecPoint[j].y)
			//	+(vecPoint[i].z - vecPoint[j].z) * (vecPoint[i].z - vecPoint[j].z));
			pt_dis = GetDistance((*pParam->pVec)[i], (*pParam->pVec)[j]);
			if (pt_dis > pParam->threshold_distance)
				continue;
			iSum += pt_dis;
			iCount++;
		}

		for (int j = i + 1; j < pParam->size; j++)
		{
			//pt_dis = sqrt((vecPoint[i].x - vecPoint[j].x) * (vecPoint[i].x - vecPoint[j].x)
			//	+(vecPoint[i].y - vecPoint[j].y) * (vecPoint[i].y - vecPoint[j].y)
			//	+(vecPoint[i].z - vecPoint[j].z) * (vecPoint[i].z - vecPoint[j].z));

			pt_dis = GetDistance((*pParam->pVec)[i], (*pParam->pVec)[j]);
			if (pt_dis > pParam->threshold_distance)
				continue;
			iSum += pt_dis;
			iCount++;
		}

		int rr, cc;
		rr = (*pParam->pVec)[i].iIndex / pParam->cols;
		cc = (*pParam->pVec)[i].iIndex % pParam->cols;
		SetMatrixValueFloat(*pParam->pMatMean, rr, cc, 0, (float)iSum / (float)iCount);
		//if (test1.find(vecPoint[i].iIndex) != test1.end())
		//	printf("wtf\n");
		//test1.insert(std::make_pair(vecPoint[i].iIndex, iCount));
		//vecCount.push_back(iCount);
	}

	return 0;
}

void Process_8S(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		char* p = m.ptr<char>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<char> vecValue = GetMatrixValueChar(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<char>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back(*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

void Process_8U(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		uchar* p = m.ptr<uchar>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<uchar> vecValue = GetMatrixValueUchar(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<uchar>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back(*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		threadparam[i].threshold_distance = bandwidth;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

void Process_16S(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		short* p = m.ptr<short>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<short> vecValue = GetMatrixValueShort(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<short>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back(*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

void Process_16U(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		ushort* p = m.ptr<ushort>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<ushort> vecValue = GetMatrixValueUShort(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<ushort>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back(*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

void Process_32S(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		int* p = m.ptr<int>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<int> vecValue = GetMatrixValueInt(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<int>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back((float)*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

void Process_32F(Mat& m, int bandwidth)
{
	std::vector<Pixel> vecPoint;
	Pixel p;
	vecPoint.assign(m.rows * m.cols, p);
	// 存均值
	Mat res_mean(m.rows, m.cols, CV_32FC1);
	// 存结果，用颜色标出
	//Mat res(m.rows, m.cols, CV_32SC1);
	Mat res(m.rows, m.cols, CV_16UC1);
	res_mean = Scalar::all(0.0);
	res = Scalar::all(0);
	int countChannel = m.channels();
	// step1 预处理
	// 给定点pt
	// 先找范围内的有效点
	// 然后用所有有效点算均值，赋到pt上
	// step2 迭代
	// 任意给定一个点pt
	// 移动到有效范围内，均值最大的点上
	std::ostringstream ostrm;
	int is = ::GetTickCount();
	for (int r = 0; r < m.rows; r++)
	{
		float* p = m.ptr<float>(r);
		for (int c = 0; c < m.cols; c++)
		{
			int iIndex = r * m.cols + c;
			std::vector<float> vecValue = GetMatrixValueFloat(m, r, c);
			Pixel pt;
			pt.iIndex = iIndex;
			//pt.x = p[c * countChannel + 0];
			//pt.y = p[c * countChannel + 1];
			//pt.z = p[c * countChannel + 2];
			for (std::vector<float>::iterator iter = vecValue.begin(); iter != vecValue.end(); iter++)
				pt.vecData.push_back(*iter);
			vecPoint[iIndex] = pt;
			//// 太慢了。。。。 不能忍
			//// 存到vector 然后排序，为了算mean...
			//// 排序的方法是错的，放弃
		}
	}
	//std::sort(vecPoint.begin(), vecPoint.end(), pixelCompare);
	int ie = ::GetTickCount();
	//printf("copy to vec cost time: %d(ms).\n", abs(ie - is));

	int iSize = (int)vecPoint.size();

	// step1. find all mean value
	// cost too much time
	// try use 4 threads to do this
	
	thParam threadparam[4];
	HANDLE hThread[4];
	DWORD dwThreadId[4];

	threadparam[0].start = 0;
	threadparam[0].end = iSize / 4;
	threadparam[1].start = iSize / 4 + 1;
	threadparam[1].end = iSize / 2;
	threadparam[2].start = iSize / 2 + 1;
	threadparam[2].end = 3 * iSize / 4;
	threadparam[3].start = 3 * iSize / 4 + 1;
	threadparam[3].end = iSize - 1;

	
	for (int i = 0; i < 4; i++)
	{
		threadparam[i].cols = m.cols;
		threadparam[i].pVec = &vecPoint;
		threadparam[i].pMatMean = &res_mean;
		threadparam[i].size = iSize;
		hThread[i] = ::CreateThread(NULL, 0, Thread_Proc, &threadparam[i], 0, &dwThreadId[i]);
	}

	is = ::GetTickCount();
	::WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
	ie = ::GetTickCount();
	printf("using 4 threads to process cost %d(ms).\n", ie - is);

	// step2
	// given point a, find max(mean) in bandwith as b, the a move to b
	// 所有范围内的点都赋值，路线上重复的要合并
	is = ::GetTickCount();
	int iNum = 0;
	int iMax = -1;	// max index in vecPoint
	int iCurrent = 0;
	int indexCur = -1;
	for (int i = 0; i < iSize; i++)
	{
		double pt_dis = 0;
		double iSum = 0;
		int rr, cc;
		rr = vecPoint[i].iIndex / m.cols;
		cc = vecPoint[i].iIndex % m.cols;
		//int iValue = GetMatrixValueInt(res, rr, cc, 0);
		ushort iValue = GetMatrixValueUshort(res, rr, cc, 0);
		if (iValue > 0)
			continue;
		indexCur = vecPoint[i].iIndex;
		iCurrent = i;
		
		//std::vector<int> vecRange;
		std::set<int> vecRange;
		//vecRange.push_back(indexCur);
		vecRange.insert(indexCur);
		while (true)
		{
			//vecRange.clear();
			//vecRange.push_back(indexCur);
			iMax = iCurrent;
			int curr, curc;
			curr = indexCur / m.cols;
			curc = indexCur % m.cols;
			float fMax = GetMatrixValueFloat(res_mean, curr, curc, 0);
			int iCC = 1;
			for (int j = iCurrent - 1; j >= 0; j--)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			for (int j = iCurrent + 1; j < iSize; j++)
			{
				//pt_dis = sqrt((vecPoint[iCurrent].x - vecPoint[j].x) * (vecPoint[iCurrent].x - vecPoint[j].x)
				//	+(vecPoint[iCurrent].y - vecPoint[j].y) * (vecPoint[iCurrent].y - vecPoint[j].y)
				//	+(vecPoint[iCurrent].z - vecPoint[j].z) * (vecPoint[iCurrent].z - vecPoint[j].z));
				pt_dis = GetDistance(vecPoint[iCurrent], vecPoint[j]);
				if (pt_dis > bandwidth)
					continue;
				int rj, cj;
				rj = vecPoint[j].iIndex / m.cols;
				cj = vecPoint[j].iIndex % m.cols;
				//SetMatrixValueUchar(res, rj, cj, 0, iNum);
				float fValue = GetMatrixValueFloat(res_mean, rj, cj, 0);
				if (fValue > fMax)
				{
					fMax = fValue;
					iMax = j;
				}
				//vecRange.push_back(vecPoint[j].iIndex);
				vecRange.insert(vecPoint[j].iIndex);
				iCC++;
			}

			//std::map<int, int>::iterator iter = test1.find(vecPoint[iCurrent].iIndex);
			//if (iter == test1.end() || iter->second != iCC)
			//	printf("wtf\n");
			// 是否是同一个点
			// 如果是 setrange
			// 如果不是 判断max上是否已经有tag
			// 如果有 replace
			// 如果没有setrange
			if (iMax != iCurrent)
			{
				iCurrent = iMax;
				indexCur = vecPoint[iCurrent].iIndex;
			}
			else
			{
				//iNum++;
				int mr, mc;
				mr = vecPoint[iMax].iIndex / m.cols;
				mc = vecPoint[iMax].iIndex % m.cols;
				//int uValue = GetMatrixValueInt(res, mr, mc, 0);
				ushort uValue = GetMatrixValueUshort(res, mr, mc, 0);
				//if (uValue > 0)
				//	ReplaceTag(res, iNum, uValue);
				//else
				//	iNum++;

				if (uValue == 0)
					iNum++;
				SetTag(res, vecRange, iNum);
				break;
			}
		}
	}
	ie = ::GetTickCount();
	printf("clustering cost time: %d(ms).\n", abs(ie - is));

	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, "_result_");


	ostrm.str("");
	ostrm.clear();
	ostrm << szFileName;
	ostrm << bandwidth;
	ostrm << ".tiff";

	cv::imwrite(ostrm.str(), res);
	// for test only
	//ostrm.str("");
	//ostrm.clear();
	//ostrm << "I:\\Download\\result_cb_";
	//ostrm << bandwidth;
	//ostrm << ".txt";
	//FILE* pFile = fopen(ostrm.str().c_str(), "w+");
	//
	//for (int r = 0; r < res.rows; r++)
	//{
	//	//std::ostringstream ostr;
	//	ostrm.str("");
	//	ostrm.clear();
	//	for (int c = 0; c < res.cols; c++)
	//	{
	//		//ostr << GetMatrixValueInt(res, r, c, 0);
	//		ostrm << GetMatrixValueUshort(res, r, c, 0);
	//		ostrm << ", ";
	//	}
	//	ostrm << "\n";
	//	std::string str = ostrm.str();
	//	fwrite(str.c_str(), sizeof(char), str.length(), pFile);
	//}
	//fclose(pFile);
	//ostrm.str("");
	//ostrm.clear();
	//std::map<int, int> mapCount;
	//for (int r = 0; r < m.rows; r++)
	//{
	//	for (int c = 0; c < m.cols; c++)
	//	{
	//		int v = GetMatrixValueUshort(res, r, c, 0);
	//		std::map<int, int>::iterator iter = mapCount.find(v);
	//		if (iter == mapCount.end())
	//		{
	//			mapCount.insert(std::make_pair(v, 1));
	//		}
	//		else
	//			iter->second = iter->second + 1;
	//	}
	//}
}

// step1 预处理
// 给定点pt
// 先找范围内的有效点
// 然后用所有有效点算均值，赋到pt上
// step2 迭代
// 任意给定一个点pt
// 移动到有效范围内，均值最大的点上
void main(void)
{
	Mat m;
	//m = imread("I:\\Download\\test1.png");
	char szFileName[256] = {0};
	::GetModuleFileName(NULL, szFileName, 256);
	char* p1 = strstr(szFileName, ".exe");
	strcpy(p1, ".ini");
	// ini -- szFileName

	char szImage[256] = {0};
	GetPrivateProfileString("MeanShift", "Image", NULL, szImage, 256, szFileName);

	int threshold_distance = GetPrivateProfileInt("MeanShift", "Bandwidth", 0, szFileName);
	m = imread(szImage);

	
	// if can not open
	// use gdal first 
	if (m.empty() || !m.isContinuous())
	{
		printf("invalid file format or wrong path of the file.");
		return;
	}

	//imshow("testPNG", m);
	//waitKey(0);
	// store the data in mat;
	int countChannel = m.channels();
	int iDepth = m.depth();
	/*
	for depth
	#define CV_8U   0
	#define CV_8S   1
	#define CV_16U  2
	#define CV_16S  3
	#define CV_32S  4
	#define CV_32F  5
	#define CV_64F  6
	*/
	int iType = m.type();
	/*type = CV_MAKE_TYPE(depth, n)*/
	//MatIterator_<Vec3b> it, end;
	//

	switch (iDepth)
	{
	case CV_8U:
		Process_8U(m, threshold_distance);
		break;
	case CV_8S:
		Process_8S(m, threshold_distance);
		break;
	case CV_16U:
		Process_16U(m, threshold_distance);
		break;
	case CV_16S:
		Process_16S(m, threshold_distance);
		break;
	case CV_32S:
		Process_32S(m, threshold_distance);
		break;
	case CV_32F:
		Process_32F(m, threshold_distance);
		break;
	case CV_64F:
		break;
	default:
		printf("iType(%d) not support yet.");
		break;
	}
}