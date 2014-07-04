#pragma once

// use GDAL to open a image
#include "gdal_priv.h"
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <algorithm>

#include "Segment.h"

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
	float threshold;
	float shapefactor;
};

class InterfaceData
{
public:
	//virtual void ProcessviaSeed(std::vector<Pixel>& vecSeed) = 0;
#ifdef MULTI_THREAD
	virtual void ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end) = 0;

	virtual void ThreadProcessviaArea(int& start, int& end) = 0;

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

	void Output(CString& strFile);
protected:
	void LoadBand(GDALRasterBand* pBand);
	// each pixel is a segment
	void Initialize();

	void UnInitialize();

	void InitNeighbours();

	void ProcessviaSeed(std::vector<Pixel>& vecSeed);

	void ProcessAllSegment();

	void ProcessviaArea();
private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
	std::vector<std::vector<std::vector<T>>> data;
#ifdef MULTI_THREAD
private:
	int rInterval;
	int cInterval;
	void Divide();
	void SetBoundary();
	void ClearBoundary();
	void ProcessBoundary();
	void ProcessviaMultiThread();

	Segment* Original2BlockSegment(int& iBlockRow, int& iBlockCol, int& row_start, int& row_count, int& col_start, int& col_end);
	Segment** RetriveBlock(int& row_start, int& row_end, int& col_start, int& col_end);
	void ProcessBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end);
	void RestoreBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end);
	void DestroyBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end);
	int FindMatch(Segment** pSegBlock, int iIndex, int& row_start, int& row_end, int& col_start, int& col_end, int except = -1);
	void MergeSegment(Segment** pSegBlock, int& iIndex1, int& iIndex2, int& row_start, int& row_end, int& col_start, int& col_end);
	float CalcEuclideanDistance(Segment** pSegBlock, int iIndex1, int iIndex2);
	float CalcNewCriteria(Segment** pSegBlock, int iIndex1, int iIndex2);

	int Block2Original(int& iBlockIndex, int& row_start, int& row_end, int& col_start, int& col_end);
	int Original2Block(int& iOriginalIndex, int& row_start, int& row_end, int& col_start, int& col_end);

	void Block2OriginalSegment(Segment* pSegOriginal, Segment* pSegBlock, int& iBlockRow, int& iBlockCol, int& row_start, int& row_end, int& col_start, int& col_end);

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

	float CalcEuclideanDistance(int iIndex1, int iIndex2);
	float CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2);

	float CalcNewCriteria(int iIndex1, int iIndex2);
	float CalcNewCriteria(Segment* pSeg1, Segment* pSeg2);
	// return index
	int FindMatch(int iIndex, int except = -1);

	void MergeSegment(int iIndex1, int iIndex2);

	// 
	Segment* FindMatchviaArea(Segment* pSeg);
	float CalcShapefactor(Segment* pSeg1, Segment* pSeg2);


#ifdef MULTI_THREAD
public:
	virtual void ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end);

	virtual void ThreadProcessviaArea(int& start, int& end);

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
