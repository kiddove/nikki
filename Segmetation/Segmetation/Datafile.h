#pragma once

// use GDAL to open a image
#include "gdal_priv.h"
#include <vector>
#include <algorithm>

#include "Segment.h"

struct segCondition
{
	float threshold;
	float shapefactor;
};

class InterfaceData
{
public:
	//virtual void ProcessviaSeed(std::vector<Pixel>& vecSeed) = 0;

	virtual void ThreadProcessAll(int& start, int& end) = 0;

	virtual void ThreadProcessviaArea(int& start, int& end) = 0;
};

struct threadParam
{
	InterfaceData* pParam;
	int start;
	int end;
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

	void ProcessAll();

	void ProcessviaArea();
private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
	std::vector<std::vector<std::vector<T>>> data;

	segCondition condition;

	// store all the segments (pointers)
	std::vector<Segment*> segment;

	int width;
	int height;

	float CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2);

	float CalcNewCriteria(Segment* pSeg1, Segment* pSeg2);

	Segment* FindMatch(Segment* pSeg);

	void MergeSegment(Segment* pSeg1, Segment* pSeg2);

	// 
	Segment* FindMatchviaArea(Segment* pSeg);
	float CalcShapefactor(Segment* pSeg1, Segment* pSeg2);

	// thread
	std::vector<HANDLE> threads;
	std::vector<DWORD> threadIDs;

	// invalid segment
	Segment invalidSegment;

public:
	virtual void ThreadProcessAll(int& start, int& end);

	virtual void ThreadProcessviaArea(int& start, int& end);
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
