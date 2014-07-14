#pragma once

// use GDAL to open a image
#include "gdal_priv.h"
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <algorithm>

#include "InterfaceFilter.h"

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

struct segCondition
{
	CalcType calcType;
	float threshold;
	//float shapefactor;
	int min_size;
	NeigbourType neibType;
public:
	segCondition()
	{
		calcType = None;
		threshold = 0.0;
		neibType = None_Neighbour;
		min_size = 1;
	}
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

template <class T>
class ImgData : public InterfaceData
{
public:
	ImgData(void) { memset(&condition, 0, sizeof(segCondition)); };
	virtual ~ImgData(void);

public:
	// load image data into vector
	void LoadData(GDALDataset* pDataset);

	// 
	void Process(std::vector<Pixel>& vecSeed, segCondition& con);

	// 
	void SetCondition(segCondition& con)
	{
		memcpy(&condition, &con, sizeof(segCondition));
	}

#ifdef USETXT
	void Output(CString& strFile);
#else
	void Output(CString& strFile, GDALDataset* pDataset);
#endif
protected:
	void LoadBand(GDALRasterBand* pBand);
	// each pixel is a segment
	void Initialize();

	void UnInitialize();

	void InitNeighbours();
	void InitNeighbours_4();
	void InitNeighbours_8();

	void ProcessviaSeed(std::vector<Pixel>& vecSeed);

	void ProcessAllSegment();

	//void ProcessviaArea();
private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
	std::vector<std::vector<std::vector<T>>> data;
	InterfaceFilter* ptFilter;
	//GDALDataType datetype;
#ifdef ONLY_SEED
	std::vector<Pixel> seedPixel;
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

	// return index
	int FindMatch(int iIndex, int except = -1);

	void MergeSegment(int iIndex1, int iIndex2);


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
	void Process(std::vector<Pixel>& vecSeed, segCondition& con);

private:
	CString m_strFile;
	//ImgInfo m_imgInfo;
	GDALDataset* m_pDataset;
};
