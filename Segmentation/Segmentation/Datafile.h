#pragma once

// use GDAL to open a image
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <algorithm>

#include "InterfaceFilter.h"
#include "Log2File.h"
#include "gdal_priv.h"

#include "ImageReader.h"
//struct segLock
//{
//	CCriticalSection* pFindLock;
//	CCriticalSection* pCountLock;
//	CCriticalSection* pMergeLock;
//	int iFindCount;
//};

struct MergePair
{
	int m;
	int n;
};

struct Blockpair
{
	int row_start;
	int row_end;
	int col_start;
	int col_end;
};

class InterfaceData
{
public:
	//virtual void ProcessviaSeed(std::vector<Pixel>& vecSeed) = 0;
#ifdef MULTI_THREAD
	//virtual void ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end) = 0;

	//virtual void ThreadProcessviaArea(int& start, int& end) = 0;

	virtual void ThreadFind() = 0;

	virtual void ThreadMerge() = 0;
#endif
};

struct threadParam
{
	// [start, end)
	InterfaceData* pParam;
	int row_start;
	int row_end;

	int col_start;
	int col_end;
};

class ImgData : public InterfaceData
{
public:
	ImgData(void)
	{
		memset(&condition, 0, sizeof(segCondition));
		memset(&tInfo, 0, sizeof(TraceInfo));
		bPreprocess = false;
		bFinished = false;
		segment = NULL;
		ptFilter = NULL;
		ptFilter_Pre = NULL;
		height = width = 0;
	};
	virtual ~ImgData(void);

public:
	void Mat2Vec(cv::Mat& mat);
	void CopyMat(cv::Mat& mat);
	float GetValue(int r, int c, int b);
	Mat result_data;
	// load image data into vector

//#ifdef USE_OPEN_CV
	//void LoadData(cv::Mat& mat);
//#else
//	void LoadData(GDALDataset* pDataset);
//#endif
	// 
	void Init(segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo);
	void Process(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo);
	void PreProcess(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo);

	// 
	void SetCondition(segCondition& con)
	{
		memcpy(&condition, &con, sizeof(segCondition));
	}

	void SetTraceInfo(TraceInfo& traceinfo)
	{
		memcpy(&tInfo, &traceinfo, sizeof(TraceInfo));
		tInfo.idx = tInfo.row * width + tInfo.col;
	}

#ifdef USETXT
	void Output(CString& strFile);
#else
	void Output(CString& strFile, GDALDataset* pDataset);
#endif
protected:
	//void LoadBand(GDALRasterBand* pBand);
	// each pixel is a segment
	bool Initialize();

	void UnInitialize();

	void InitNeighbours();
	void InitNeighbours_4();
	void InitNeighbours_8();

	void ProcessviaSeed(std::vector<cv::Point>& vecSeed);

	void ProcessviaMask(std::map<int, set<int>>& mapMask);

	void ProcessAllSegment();

	void ProcessPre();
	//void ProcessviaArea();

	// for ENT
	void InitializeForEntropy();
private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
//#ifdef USE_OPEN_CV
//	Mat data;
//#else
//	std::vector<std::vector<std::vector<float>>> data;
//#endif
	Mat data;
	InterfaceFilter* ptFilter;
	InterfaceFilter* ptFilter_Pre;
	TraceInfo tInfo;

	bool bPreprocess;
	bool bFinished;
	//GDALDataType datetype;
#ifdef ONLYSEED
	//std::vector<Pixel> seedPixel;
	std::vector<cv::Point> seedPixel;
#endif
#ifdef MULTI_THREAD
private:

	void ProcessviaMultiThread();

	std::queue<Blockpair> queBlock;
	CCriticalSection queLock;
	// to be merged
	std::vector<MergePair> vecMerge;
	CCriticalSection vecMergeLock;

	HANDLE* pFindEventArray;
	HANDLE mergeHandle;
	std::map<DWORD, HANDLE> mapFindEvent;
	bool bQuitFind;
	bool bForceMerge;
#endif
	segCondition condition;

	// store all the segments (pointers)
	//std::vector<Segment*> segment;
	Segment** segment;

	int width;
	int height;
	int channel;
	std::vector<int> vecMin;
	std::vector<int> vecMax;

	// return index
	int FindMatch(int iIndex, int except = -1);
	int FindMatchPre(int iIndex, int except = -1);

	void MergeSegment(int iIndex1, int iIndex2);

	// merge seg2's neighbour into seg1
	void MergeNeighbours(int iIndex1, int iIndex2);

	void SetInvalid(int iIndex);

#ifdef MULTI_THREAD
public:
	//virtual void ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end);

	//virtual void ThreadProcessviaArea(int& start, int& end);

	virtual void ThreadFind();

	virtual void ThreadMerge();
#endif
};

class Datafile
{
public:
	Datafile(void);
	virtual ~Datafile(void);

	bool OpenFile(LPCTSTR strFilePath);
	void Close();

	//void Process();
	void Process(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo);
	void PreProcess(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo);

private:
	CString m_strFile;
	//ImgInfo m_imgInfo;
	//cv::Mat m_Mat;
	GDALDataset* m_pDataset;
	CString GetSuffix(segCondition& con, bool bTxt = false);
	CString GetSuffix_Pre(segCondition& con, bool bTxt = false);
	ImgData m_imgData;
	cv::Mat m_MatResize;
	
public:
	ImageReader m_Image;
	bool m_bResize;
};