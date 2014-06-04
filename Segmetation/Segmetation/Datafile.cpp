#include "StdAfx.h"
#include "Datafile.h"

Datafile::Datafile(void)
{
	m_strFile.Empty();
	m_pDataset = NULL;
	GDALAllRegister();
}

Datafile::~Datafile(void)
{
	Close();
}

void Datafile::Close(void)
{
	m_strFile.Empty();
	if (m_pDataset != NULL)
	{
		GDALClose(m_pDataset);
		m_pDataset = NULL;
	}
}

bool Datafile::OpenFile(LPTSTR strFilePath)
{
	m_pDataset = (GDALDataset*)GDALOpen(strFilePath, GA_ReadOnly);

	if (m_pDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFilePath);
		AfxMessageBox(strErr);
		//f.Close();
		return false;
	}

	m_strFile = strFilePath;
	return true;
}

void Datafile::Process()
{
	int iRasterCount = m_pDataset->GetRasterCount();
	if (iRasterCount < 1)
		return;

	// fetch a raster band to get data type

	GDALRasterBand *poBand = m_pDataset->GetRasterBand(1);
	GDALDataType iType = poBand->GetRasterDataType();

	switch (iType)
	{
	case GDT_UInt16:
		{
			ImgData<unsigned short> usData;
			// load data
			usData.LoadData(m_pDataset);
			usData.Process();
			//return usData;
		}
		break;
	case GDT_Int16:
		{
			ImgData<short> sData;
			// load data
			sData.LoadData(m_pDataset);
			//return sData;
		}
		break;
	case GDT_Float32:
		{
			ImgData<float> fData;
			// load data
			fData.LoadData(m_pDataset);
			//return sData;
		}
		break;
	case GDT_Byte:
	default:
		{
			ImgData<byte> bData;
			// load data
			bData.LoadData(m_pDataset);
			//return bData;
		}
		break;
	}
}

/// ImgData
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
// load image data into vector
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
void ImgData<T>::Process()
{
	// segmentation
	Initialize();

	// for each segment
	// 1. find neighbours
	// 2. for each neighbour calced find the min, get the pixel of the min
	// 3. calc of the min pixel neighbour
	// 4. for each  calc ed get min
	// 5. if mutual && < theshold
	// 6. merge

	while (true)
	{
		bool bMatch = false;
		for (int i = 0; i < segment.size(); i++)
		{
			//int index = FindMatch(segment[i]);
			if (segment[i].m_area.empty())
				continue;
			Segment si = FindMatch(segment[i]);
			if (!si.m_area.empty())
			{
				Segment sj = FindMatch(si);

				if (!sj.m_area.empty())
				{
					// if (segment[i] == sj)
					if (segment[i].m_iIndex == sj.m_iIndex)
					{
						// merge
						MergeSegment(segment[i], si);
						bMatch = true;
						break;
					}
				}
			}
		}
		//if (condition)
		//else
		//break;

		if (!bMatch)
			break;
	}
}

template <class T>
void ImgData<T>::Initialize()
{
	// clear all segments
	segment.clear();
	threshold = 35000;

	// each pixel is assigned as a distinct segment
	if (data.size() > 0)
	{
		int iNum = 0;
		// row
		for (int r = 0; r < data[0].size(); r++)
		{
			// col
			for (int c = 0; c < data[0][r].size(); c++)
			{
				Segment s(r, c);
				float f = 0.0f;
				float v = 0.0f;
				for (int k = 0; k < data.size(); k++)
				{
					f += data[k][r][c];
					v += (data[k][r][c]) * (data[k][r][c]);
					// Avg
					s.m_Avg.push_back(data[k][r][c]);
					// Variance
					s.m_Variance.push_back(0.0);
				}

				s.m_iIndex = iNum++;
				segment.push_back(s);
			}
		}
	}
}

//template <class T>
//void ImgData<T>::Findneighbours()
//{
//	// find neighbour pixels
//	for (int i = 0; i < segment.size(); i++)
//	{
//		segment[i].m_neighbours.clear();
//
//		// find neighbour segments
//
//		for (int k = 0; k < segment.size(); k++)
//		{
//			// 1. not the same segment
//			// 2. adjacent
//			if (i != k && IsAdjacent(segment[i], segment[k]))
//			{
//				// push_back
//				segment[i].m_neighbours.push_back(segment[k]);
//			}
//		}
//	}
//}

template <class T>
void ImgData<T>::Findneighbours(Segment& s)
{
	s.m_neighbours.clear();

	// find neighbour segments

	for (int i = 0; i < segment.size(); i++)
	{
		// 1. not the same segment
		// 2. adjacent
		if (IsAdjacent(s, segment[i]))
		{
			// push_back
			s.m_neighbours.push_back(segment[i]);
		}
	}
}

template <class T>
bool ImgData<T>::IsAdjacent(Segment& s1, Segment& s2)
{
	for (int i = 0; i < s1.m_area.size(); i++)
	{
		for (int j = 0; j < s2.m_area.size(); j++)
		{
			if (abs(s1.m_area[i].row - s2.m_area[j].row) == 1
				|| abs(s1.m_area[i].col - s2.m_area[j].col) == 1)
				return true;
		}
	}
	return false;
}

template <class T>
float ImgData<T>::CalcEuclideanDistance(Segment& s1, Segment& s2)
{

	ASSERT(s1.m_Avg.size() == s2.m_Avg.size());
	// to be continued...
	float f = 0.0f;
	for (int i = 0; i < s1.m_Avg.size(); i++)
	{
		f += (s1.m_Avg[i] - s2.m_Avg[i]) * (s1.m_Avg[i] - s2.m_Avg[i]);
	}
	float ed = sqrt(f);
	return ed;
}

template <class T>
Segment& ImgData<T>::FindMatch(Segment& s)
{
	if (s.m_area.empty())
		return segInvalid;
	Findneighbours(s);

	float fEd_min = 1e8;
	int index = -1;
	for (int j = 0; j < s.m_neighbours.size(); j++)
	{
		if (s.m_neighbours[j].m_area.empty())
			continue;
		// calc
		float fValue = CalcEuclideanDistance(s, s.m_neighbours[j]);
		// find min
		if (fValue < fEd_min && fValue < threshold)
		{
			fEd_min = fValue;
			index = j;
		}
	}

	if (index > 0)
		return segment[index];
	else
		return segInvalid;
}

template <class T>
void ImgData<T>::MergeSegment(Segment& s1, Segment& s2)
{
	int n1 = s1.m_area.size();
	int n2 = s2.m_area.size();

	std::vector<float> a1 = s1.m_Avg;
	std::vector<float> a2 = s2.m_Avg;

	std::vector<float> v1 = s1.m_Variance;
	std::vector<float> v2 = s2.m_Variance;

	ASSERT(a1.size() == a2.size());
	ASSERT(v1.size() == v2.size());

	// for each band


	for (int i = 0; i < a1.size(); i++)
	{
		float avg1 = a1[i];
		float avg2 = a2[i];
		float var1 = v1[i];
		float var2 = v2[i];
		float avg = 0.0;
		avg= (avg1 * n1 + avg2 * n2) / (n1 + n2);
		s1.m_Avg[i] = avg;
		s1.m_Variance[i] = (n1 * var1 + n2 * var2) / (n1 + n2) + (n1 * avg1 * avg1 + n2 * avg2 * avg2) / (n1 + n2) - avg * avg;
		//v1[i] = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - s1.m_fAvg * s1.m_fAvg;
	}
	//// s2->s1
	s1.m_area.insert(s1.m_area.end(), s2.m_area.begin(), s2.m_area.end());
	s2.m_area.clear();

	//// Avg
	//s1.m_fAvg = (a1 * n1 + a2 * n2) / (n1 + n2);

	//// variance
	//s1.m_fVariance = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - s1.m_fAvg * s1.m_fAvg;
}