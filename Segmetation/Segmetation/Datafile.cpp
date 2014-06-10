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
			//usData.UnInitialize();
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

	InitNeighbours();

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
			if (segment[i] == NULL)
				continue;
			Segment* pSeg1 = FindMatch(segment[i]);
			if (pSeg1 != NULL)
			{
				Segment* pSeg2 = FindMatch(pSeg1);

				if (pSeg2 != NULL)
				{
					if (segment[i] == pSeg2)
					{
						// merge
						MergeSegment(segment[i], pSeg1);
						bMatch = true;
					}
				}
			}
		}

		if (!bMatch)
			break;
	}

	Output();
}

template <class T>
void ImgData<T>::Initialize()
{
	// clear all segments
	segment.clear();
	threshold = 20000;

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
				Segment* pSeg = new Segment(r, c);
				//Segment s(r, c);
				float f = 0.0f;
				float v = 0.0f;
				for (int k = 0; k < data.size(); k++)
				{
					f += data[k][r][c];
					v += (data[k][r][c]) * (data[k][r][c]);
					// Avg
					pSeg->m_Avg.push_back(data[k][r][c]);
					// Variance
					pSeg->m_Variance.push_back(0.0);
				}

				pSeg->m_iIndex = iNum++;
				segment.push_back(pSeg);
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
//		segment[i].m_neighbourpSeg>clear();
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
//				segment[i].m_neighbourpSeg>push_back(segment[k]);
//			}
//		}
//	}
//}

template <class T>
void ImgData<T>::Findneighbours(Segment& s, int r, int c)
{
	pSeg>m_neighbourpSeg>clear();

	// find neighbour segments
	// 8 directions
	// r-1, c-1
	if (r > 1 && c > 1)
		pSeg>m_neighbourpSeg>push_back();
	// r-1, c
	// r-1, c+1
	// r, c-1
	// r, c+1
	// r+1, c-1
	// r+1, c
	// r+1, c+1

	for (int i = 0; i < segment.size(); i++)
	{
		// 1. not the same segment
		// 2. adjacent
		if (IsAdjacent(s, segment[i]))
		{
			// push_back
			pSeg>m_neighbourpSeg>push_back(segment[i]);
		}
	}
}

template <class T>
bool ImgData<T>::IsAdjacent(Segment* pSeg1, Segment* pSeg2)
{
	if (pSeg1 == NULL || pSeg2 == NULL)
		return false;
	for (int i = 0; i < pSeg1->m_area.size(); i++)
	{
		for (int j = 0; j < pSeg2->m_area.size(); j++)
		{
			if (abs(pSeg1->m_area[i].row - pSeg2->m_area[j].row) == 1
				|| abs(pSeg1->m_area[i].col - pSeg2->m_area[j].col) == 1)
				return true;
		}
	}
	return false;
}

template <class T>
float ImgData<T>::CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2)
{

	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	float f = 0.0f;
	for (int i = 0; i < pSeg1->m_Avg.size(); i++)
	{
		f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}
	float ed = sqrt(f);
	return ed;
}

template <class T>
Segment* ImgData<T>::FindMatch(Segment* pSeg)
{
	if (pSeg != NULL)
	{
		float fEd_min = 1e8;
		int index = -1;
		for (int j = 0; j < pSeg->m_neighbours.size(); j++)
		{
			if (pSeg->m_neighbours[j] == NULL)
				continue;
			// calc
			//<dynamic_cast>
			Segment* p = (Segment*)(pSeg->m_neighbours[j]);
			if (p == NULL)
				continue;
			float fValue = CalcEuclideanDistance(pSeg, p);
			// find min
			if (fValue < fEd_min && fValue < threshold)
			{
				fEd_min = fValue;
				index = p->m_iIndex;
			}
		}
		if (index > 0)
		{
			//TRACE(_T("%d match %d\n"), pSeg->m_iIndex, index);
			return segment[index];
		}
	}

	return NULL;
}

template <class T>
void ImgData<T>::MergeSegment(Segment* pSeg1, Segment* pSeg2)
{
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	std::vector<float> a1 = pSeg1->m_Avg;
	std::vector<float> a2 = pSeg2->m_Avg;

	std::vector<float> v1 = pSeg1->m_Variance;
	std::vector<float> v2 = pSeg2->m_Variance;

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
		pSeg1->m_Avg[i] = avg;
		pSeg1->m_Variance[i] = (n1 * var1 + n2 * var2) / (n1 + n2) + (n1 * avg1 * avg1 + n2 * avg2 * avg2) / (n1 + n2) - avg * avg;
		//v1[i] = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - pSeg1->m_fAvg * pSeg1->m_fAvg;
	}
	//// s2->s1
	pSeg1->m_area.insert(pSeg1->m_area.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
	pSeg2->m_area.clear();

	// update neighbours

	// remove seg2 from seg1's neighbours
	// put seg2 neighbours to seg1 (except seg1)
	// change seg2 neighbours to seg1 

	std::vector<int>::iterator iter = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), (int)pSeg2);
	if (iter != pSeg1->m_neighbours.end());
		pSeg1->m_neighbours.erase(iter);

	iter = std::find(pSeg2->m_neighbours.begin(), pSeg2->m_neighbours.end(), (int)pSeg1);
	if (iter != pSeg2->m_neighbours.end());
		pSeg2->m_neighbours.erase(iter);

	for (int i = 0; i < pSeg2->m_neighbours.size(); i++)
	{
		std::vector<int>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), pSeg2->m_neighbours[i]);
		
		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(pSeg2->m_neighbours[i]);

		Segment* pSeg = (Segment*)(pSeg2->m_neighbours[i]);
		if (pSeg == NULL)
			continue;

		// 如果这里面已经有seg1了。。 就直接把seg2删掉
		// if seq1 is seq's neighbour then delete seq2 from seq's neighbour
		iter1 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), (int)pSeg1);
		std::vector<int>::iterator iter2 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), (int)pSeg2);

		ASSERT(iter2 != pSeg->m_neighbours.end());

		if (iter1 != pSeg->m_neighbours.end())
			pSeg->m_neighbours.erase(iter2);
		else
			*iter2 = (int)pSeg1;
	}

	segment[pSeg2->m_iIndex] = NULL;

	//TRACE(_T("merge %d to %d\n"), pSeg2->m_iIndex, pSeg1->m_iIndex);
	delete pSeg2;
	pSeg2 = NULL;
}

template <class T>
ImgData<T>::~ImgData()
{
	UnInitialize();
}

template <class T>
void ImgData<T>::UnInitialize()
{
	for (int i = 0; i < segment.size(); i++)
	{
		if (segment[i] != NULL)
		{
			delete segment[i];
			segment[i] = NULL;
		}
	}
}

template <class T>
void ImgData<T>::InitNeighbours()
{
	for (int i = 0; i < segment.size(); i++)
	{
		int r = segment[i]->m_area[0].row;
		int c = segment[i]->m_area[0].col;

		ASSERT(segment[i]->m_iIndex == r * width + c);

		// r-1, c-1
		if (r >= 1 && c >= 1)
		{
			int iIndex = (r - 1) * width + c - 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r-1, c
		if (r >= 1)
		{
			int iIndex = (r - 1) * width + c;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r-1, c+1
		if (r >= 1 && c < width - 1)
		{
			int iIndex = (r - 1) * width + c + 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r, c-1
		if (c >= 1)
		{
			int iIndex = r * width + c - 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r, c+1
		if (c < width - 1)
		{
			int iIndex = r * width + c + 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r+1, c-1
		if (r < height - 1 && c >= 1)
		{
			int iIndex = (r + 1) * width + c - 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r+1, c
		if (r < height - 1)
		{
			int iIndex = (r + 1) * width + c;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
		// r+1, c+1
		if (r < height - 1 && c < width - 1)
		{
			int iIndex = (r + 1) * width + c + 1;
			segment[i]->m_neighbours.push_back((int)segment[iIndex]);
		}
	}
}

template <class T>
void ImgData<T>::Output()
{
	std::vector<std::vector<int>> vecResult;
	std::vector<int> vecRow;
	vecRow.assign(width, 0);
	vecResult.assign(height, vecRow);

	for (int i = 0; i < segment.size(); i++)
	{
		if (segment[i] != NULL)
		{
			for (int j = 0; j < segment[i]->m_area.size(); j++)
			{
				vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = segment[i]->m_iIndex;
			}
		}
	}

	CStdioFile f;
	f.Open(_T("E:\\result.txt"), CFile::modeCreate | CFile::modeReadWrite);

	for (int i = 0; i < height; i++)
	{
		CString strRow;
		for (int j = 0; j < width; j++)
		{
			CString str;
			str.Format(_T("%d\t"), vecResult[i][j]);
			strRow += str;
		}
		strRow += _T("\n");
		f.WriteString(strRow);
	}
	f.Close();
}