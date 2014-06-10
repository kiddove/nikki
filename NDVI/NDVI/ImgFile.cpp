#include "StdAfx.h"
#include "ImgFile.h"

template <class T>
ImgData<T>::ImgData(void)
{
}

template <class T>
ImgData<T>::~ImgData(void)
{
}


template <class T>
void ImgData<T>::LoadData(GDALDataset* pDataset)
{
	data.clear();
	// band count
	int iRasterCount = pDataset->GetRasterCount();
	for (int i = 1; i <= iRasterCount; i++)
	{
		GDALRasterBand* poBand = pDataset->GetRasterBand(i);
		if (poBand != NULL)
			LoadBand(poBand);
	}
}

template <class T>
void ImgData<T>::LoadBand(GDALRasterBand* pBand)
{
	char *pafScanline;
	width = pBand->GetXSize();		//width,colcount
	height = pBand->GetYSize();		//height,rowcownt
	GDALDataType iType = pBand->GetRasterDataType();

	int iCount = width * height * sizeof(T);
	pafScanline = (char*)CPLMalloc(iCount);
	pBand->RasterIO(GF_Read, 0, 0, width, height, pafScanline, width, height, iType, 0, 0);

	T* pData = (T*)pafScanline;
	std::vector<std::vector<T>> vecBand;

	for (int i = 0; i < height; i++)
	{
		std::vector<T> vecRow;
		for (int j = 0; j < width; j++)
		{
			vecRow.push_back(pData[i * width + j]);
		}
		vecBand.push_back(vecRow);
	}
	data.push_back(vecBand);
}

template <class T>
void ImgData<T>::CalcNDVI()
{
	// (band1-band2)/(band1+band2)

	// vecResult is the result
	std::vector<std::vector<float>> vecResult;
	for (int i = 0; i < height; i++)
	{
		std::vector<float> vecRow;
		for (int j = 0; j < width; j++)
		{
			vecRow.push_back((float)(data[0][i][j] - data[1][i][j]) / (float)(data[0][i][j] + data[1][i][j]));
		}
		vecResult.push_back(vecRow);
	}
}

ImgFile::ImgFile(void)
{
	// register
	GDALAllRegister();
	m_pDataset = NULL;
}

ImgFile::~ImgFile(void)
{
	Close();
}

void ImgFile::Close(void)
{
	m_info.strFile.Empty();
	m_info.type = GDT_Unknown;
	if (m_pDataset != NULL)
	{
		GDALClose(m_pDataset);
		m_pDataset = NULL;
	}
}

GDALDataset* ImgFile::OpenFile(CString& strFle)
{
	// open file
	m_pDataset = (GDALDataset*)GDALOpen(strFle, GA_ReadOnly);

	if (m_pDataset == NULL)
	{
		// open failed.
		// record log information
	}

	m_info.strFile = strFle;
	return m_pDataset;
}
void ImgFile::LoadImgInfo()
{
	int iRasterCount = m_pDataset->GetRasterCount();
	if (iRasterCount < 1)
		return;

	// fetch a raster band to get data type

	GDALRasterBand *poBand = m_pDataset->GetRasterBand(1);
	m_info.type = poBand->GetRasterDataType();
}

void ImgFile::Process()
{
	switch(m_info.type)
	{
	case GDT_Byte:
		{
			ImgData<byte> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
			//ImgData<byte>* pData = new ImgData<byte>();
			//pData->LoadData(m_pDataset);
			//pData->CalcNDVI();
			//m_pData = (LPVOID)pData;
		}
		break;
	case GDT_UInt16:
		{
			ImgData<unsigned short> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_Int16:
		{
			ImgData<short> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_UInt32:
		{
			ImgData<unsigned int> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_Int32:
		{
			ImgData<int> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_Float32:
		{
			ImgData<float> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_Float64:
		{
			ImgData<double> imgdata;
			imgdata.LoadData(m_pDataset);
			imgdata.CalcNDVI();
		}
		break;
	case GDT_Unknown:
	default:
		break;
	}
}