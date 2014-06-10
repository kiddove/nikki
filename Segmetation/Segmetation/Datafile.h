#pragma once

// use GDAL to open a image
#include "gdal_priv.h"
#include <vector>
#include <algorithm>

#include "Segment.h"

template <class T>
class ImgData
{
public:
	ImgData(void) {};
	virtual ~ImgData(void);

public:
	// load image data into vector
	void LoadData(GDALDataset* pDataset);

	// 
	void Process();

	// 
	void SetThreshold(T& t)
	{
		threshold = t;
	}

	void Output();
protected:
	void LoadBand(GDALRasterBand* pBand);
	// each pixel is a segment
	void Initialize();

	void UnInitialize();

	// only used when initialize
	void Findneighbours(Segment& s, int r, int c);

	void InitNeighbours();

private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
	std::vector<std::vector<std::vector<T>>> data;
	float threshold;

	// store all the segments (pointers)
	std::vector<Segment*> segment;

	int width;
	int height;

	bool IsAdjacent(Segment* pSeg1, Segment* pSeg2);

	float CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2);

	Segment* FindMatch(Segment* pSeg);

	void MergeSegment(Segment* pSeg1, Segment* pSeg2);

	Segment segInvalid;

};

class Datafile
{
public:
	Datafile(void);
	virtual ~Datafile(void);

	bool OpenFile(LPTSTR strFilePath);
	void Close();

	void Process();

private:
	CString m_strFile;
	//ImgInfo m_imgInfo;
	GDALDataset* m_pDataset;
};
