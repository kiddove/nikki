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
	virtual ~ImgData(void) {};

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
protected:
	void LoadBand(GDALRasterBand* pBand);
	// each pixel is a segment
	void Initialize();

	//void Findneighbours();

	void Findneighbours(Segment& s);

private:
	// data[0][1][2] --- 0-band, 1-row(y), 2-col(x)
	std::vector<std::vector<std::vector<T>>> data;
	float threshold;

	std::vector<Segment> segment;

	int width;
	int height;

	bool IsAdjacent(Segment& s1, Segment& s2);

	float CalcEuclideanDistance(Segment& s1, Segment& s2);

	Segment& FindMatch(Segment& s);

	void MergeSegment(Segment& s1, Segment& s2);

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
