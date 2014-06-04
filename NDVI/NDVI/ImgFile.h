#pragma once
#include "gdal_priv.h"

template <class T>
class ImgData
{
public:
	ImgData(void);
	virtual ~ImgData(void);

public:
	void LoadData(GDALDataset* pDataset);
	void CalcNDVI();
private:

	void LoadBand(GDALRasterBand* pBand);
	std::vector<std::vector<std::vector<T>>> data;

	int width;
	int height;
};

struct ImgInfo
{
	CString strFile;
	GDALDataType type;
};

class ImgFile
{
public:
	ImgFile(void);
	virtual ~ImgFile(void);
	
	GDALDataset* OpenFile(CString& strFle);
	void LoadImgInfo();

	void Close();

	void Process();

private:
	GDALDataset* m_pDataset;
	ImgInfo m_info;
};
