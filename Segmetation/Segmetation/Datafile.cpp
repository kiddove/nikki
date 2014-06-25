#include "StdAfx.h"
#include "Datafile.h"

//#define THREAD_COUNT 9
#define ROW_BLOCK	10
#define COL_BLOCK	10

bool lessRow(const Pixel& p1, const Pixel& p2)
{
	if (p1.row < p2.row)
		return true;
	return false;
}

bool lessCol(const Pixel& p1, const Pixel& p2)
{
	if (p1.col < p2.col)
		return true;
	return false;
}

// get cpu count
int get_core_count()
{
	int iCount = 1;
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	iCount = si.dwNumberOfProcessors;
	return iCount;
}
#ifdef MULTI_THREAD
DWORD WINAPI ThreadProcAll(LPVOID pParam)
{
	try
	{
		threadParam* p = (threadParam*)pParam;
		InterfaceData* pData = p->pParam;
		pData->ThreadProcessAll(p->row_start, p->row_end, p->col_start, p->col_end);
		return 0x1001;
	}
	catch (std::exception e)
	{
		TRACE(_T("%s\n"), e.what());
		return 0x2001;
	}
}
//DWORD WINAPI ThreadProc_ProcessFind(LPVOID pParam)
//{
//	try
//	{
//		threadParam* p = (threadParam*)pParam;
//		InterfaceData* pData = p->pParam;
//		//pData->ThreadProcessAll(p->start, p->end);
//		pData->ThreadFind(p->start, p->end);
//		return 0x1001;
//	}
//	catch (std::exception e)
//	{
//		TRACE(_T("%s\n"), e.what());
//		return 0x2001;
//	}
//}
//
//DWORD WINAPI ThreadProc_ProcessMerge(LPVOID pParam)
//{
//	try
//	{
//		InterfaceData* pData = (InterfaceData*)pParam;
//		pData->ThreadMerge();
//		return 0x1002;
//	}
//	catch (std::exception e)
//	{
//		TRACE(_T("%s\n"), e.what());
//		return 0x2002;
//	}
//}
#endif

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

bool Datafile::OpenFile(LPCTSTR strFilePath)
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

void Datafile::Process(std::vector<Pixel>& vecSeed, segCondition& con)
{
	int iRasterCount = m_pDataset->GetRasterCount();
	if (iRasterCount < 1)
		return;

	// fetch a raster band to get data type

	GDALRasterBand *poBand = m_pDataset->GetRasterBand(1);
	GDALDataType iType = poBand->GetRasterDataType();

	CString strOutput;
	int ipos = m_strFile.ReverseFind(_T('.'));
	strOutput = m_strFile.Left(ipos);
	strOutput += _T(".txt");
	switch (iType)
	{
	case GDT_UInt16:
		{
			ImgData<unsigned short> usData;
			// load data
			usData.LoadData(m_pDataset);
			usData.Process(vecSeed, con);
			usData.Output(strOutput);
			//usData.UnInitialize();
			//return usData;
		}
		break;
	case GDT_Int16:
		{
			ImgData<short> sData;
			// load data
			sData.LoadData(m_pDataset);
			sData.Process(vecSeed, con);
			sData.Output(strOutput);
			//return sData;
		}
		break;
	case GDT_Float32:
		{
			ImgData<float> fData;
			// load data
			fData.LoadData(m_pDataset);
			fData.Process(vecSeed, con);
			fData.Output(strOutput);
			//return sData;
		}
		break;
	case GDT_Byte:
	default:
		{
			ImgData<byte> bData;
			// load data
			bData.LoadData(m_pDataset);
			bData.Process(vecSeed, con);
			bData.Output(strOutput);
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
void ImgData<T>::Process(std::vector<Pixel>& vecSeed, segCondition& con)
{
	int i1 = ::GetTickCount();
	// segmentation
	SetCondition(con);

	Initialize();

	InitNeighbours();

	ProcessviaSeed(vecSeed);

#ifdef MULTI_THREAD
	int is = ::GetTickCount();
	Divide();
	ProcessBoundary();
	SetBoundary();
	ProcessviaMultiThread();
	ClearBoundary();
	int ie = ::GetTickCount();
	CString ss;
	ss.Format(_T("MultiThread cost time : %d(ms)\n"), ie - is);
	::OutputDebugString(ss);
#endif
	
	ProcessAllSegment();

	int i2 = ::GetTickCount();
	CString str;
	str.Format(_T("total time : %d(s)\n"), (i2 - i1) / 1000);
	::OutputDebugString(str);
//	ProcessviaArea();
}


template <class T>
void ImgData<T>::ProcessviaSeed(std::vector<Pixel>& vecSeed)
{
	while (true)
	{
		bool bMatch = false;
		for (int i = 0; i < (int)vecSeed.size(); i++)
			//for (int i = 0; i < segment.size(); i++)
		{
			// maybe several different ways
			// for now start from bottom-left
			// if start from top-left
			//int iIndex = vecSeed[i].row * width + vecSeed[i].col;
			// if start from bottom-left
			ASSERT(vecSeed[i].row < height && vecSeed[i].col < width);
			int iIndex = (height - 1 - vecSeed[i].row) * width + vecSeed[i].col;
			if (segment[iIndex] == NULL || segment[iIndex]->m_bBoundary)
				continue;
			int iIndex1 = FindMatch(iIndex);
			if (iIndex1 != -1)
			{
				int iIndex2 = FindMatch(iIndex1, iIndex);

				if (iIndex2 != -1)
				{
					if (iIndex == iIndex2)
					{
						// merge
						MergeSegment(iIndex, iIndex1);
						bMatch = true;
					}
				}
			}
		}

		if (!bMatch)
			break;
	}
}
#ifdef MULTI_THREAD
template <class T>
void ImgData<T>::ProcessBoundary()
{
	int iRound = 0;
	while (true)
	{
		int i1 = ::GetTickCount();
		bool bMatch = false;
		for (int i = 0; i < (int)vecBoundary.size(); i++)
		{
			ASSERT(vecBoundary[i].row < height && vecBoundary[i].col < width);
			int iIndex = vecBoundary[i].row * width + vecBoundary[i].col;
			if (segment[iIndex] == NULL || segment[iIndex]->m_bBoundary)
				continue;
			int iIndex1 = FindMatch(iIndex);
			if (iIndex1 != -1)
			{
				int iIndex2 = FindMatch(iIndex1, iIndex);

				if (iIndex2 != -1)
				{
					if (iIndex == iIndex2)
					{
						// merge
						MergeSegment(iIndex, iIndex1);
						bMatch = true;
					}
				}
			}
		}

		int i2 = ::GetTickCount();
		TRACE(_T("ProcessBoundary Round %d cost %d(ms)\n"), iRound++, i2 - i1);
		if (!bMatch)
			break;
	}
}

template <class T>
void ImgData<T>::ProcessviaMultiThread()
{
	// use multithreads
	int iThreadCount = 4;
	//int iThreadCount = ROW_BLOCK * COL_BLOCK;

	HANDLE * pHandle = new HANDLE[iThreadCount];
	threadParam* pThreadParam = new threadParam[iThreadCount];

	int iCount = 0;
	for (int i = 0; i < ROW_BLOCK; i++)
	{
		for (int j = 0; j < COL_BLOCK; j++)
		{
			Blockpair bp;
			bp.i = i;
			bp.j = j;
			queBlock.push(bp);
		}
	}
	//for (int i = 0; i < ROW_BLOCK; i++)
	for (int i = 0; i < iThreadCount; i++)
	{
		//for (int j = 0; j < COL_BLOCK; j++)
		////for (int j = 0; j < 2; j++)
		//{
			DWORD dwThreadID = 0; 
			pThreadParam[iCount].pParam = (InterfaceData*)this;
			pThreadParam[iCount].row_start = i + 1;
			pThreadParam[iCount].row_end = -1;

			pThreadParam[iCount].col_start = -1;
			pThreadParam[iCount].col_end = -1;

			pHandle[iCount] = CreateThread(NULL, 0, ThreadProcAll, &pThreadParam[iCount], 0, &dwThreadID);
			
			//::WaitForMultipleObjects(1, &pHandle[iCount], TRUE, INFINITE);
			iCount++;
		//}

	}

	::WaitForMultipleObjects(iThreadCount, pHandle, TRUE, INFINITE);

	for (int i= 0; i < iThreadCount; i++)
		::CloseHandle(pHandle[i]);
	delete[] pHandle;
	delete[] pThreadParam;
}
#endif

template <class T>
void ImgData<T>::ProcessAllSegment()
{
	int iTime = 0;
	while (true)
	{
		int i1 = ::GetTickCount();
		bool bMatch = false;
		for (int i = 0; i < width * height; i++)
		{
			if (segment[i] == NULL || segment[i]->m_bBoundary)
				continue;

			int iIndex1 = FindMatch(segment[i]->m_iIndex);
			if (iIndex1 != -1)
			{
				int iIndex2 = FindMatch(iIndex1);

				if (iIndex2 != -1)
				{
					if (segment[i]->m_iIndex == iIndex2)
					{
						// merge
						MergeSegment(segment[i]->m_iIndex, iIndex1);
						bMatch = true;
					}
				}
			}
		}

		if (!bMatch)
			break;

		int i2 = ::GetTickCount();
		CString str;
		str.Format(_T("%d loop , cost time %d\n"), ++iTime, i2 - i1);
		::OutputDebugString(str);
	}
}

template <class T>
void ImgData<T>::ProcessviaArea()
{
	while (true)
	{
		bool bMatch = false;
		for (int i = 0; i < (int)segment.size(); i++)
		{
			if (segment[i] == NULL || segment[i]->m_bBoundary)
				continue;
			Segment* pSeg1 = FindMatchviaArea(segment[i]);
			if (pSeg1 != NULL && pSeg1->m_iIndex != -1)
			{
				Segment* pSeg2 = FindMatchviaArea(pSeg1);

				if (pSeg2 != NULL && pSeg2->m_iIndex != -1)
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
}

template <class T>
void ImgData<T>::Initialize()
{
	segment = new Segment*[width * height];
	// each pixel is assigned as a distinct segment
	if (!data.empty())
	{
		int iNum = 0;
		// row
		for (int r = 0; r < (int)data[0].size(); r++)
		{
			// col
			for (int c = 0; c < (int)data[0][r].size(); c++)
			{
				Segment* pSeg = new Segment(r, c);
				//Segment s(r, c);
				float f = 0.0f;
				float v = 0.0f;
				for (int k = 0; k < (int)data.size(); k++)
				{
					f += data[k][r][c];
					v += (data[k][r][c]) * (data[k][r][c]);
					// Avg
					pSeg->m_Avg.push_back(data[k][r][c]);
					// Variance
					pSeg->m_Variance.push_back(0.0);
				}

				pSeg->m_iIndex = iNum++;
				pSeg->min_row = pSeg->max_row = r;
				pSeg->min_col = pSeg->max_col = c;
				segment[pSeg->m_iIndex] = pSeg;
			}
		}
	}
}

template <class T>
float ImgData<T>::CalcEuclideanDistance(int iIndex1, int iIndex2)
{
	//segment[iIndex1], segment[iIndex2] lock befor enter this function
	if (iIndex1 == -1 || iIndex2 == -1)
		return 1e8;

	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());

	// to be continued...
	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}

	//if (f < 0.1)
	//{
	//	CString str;
	//	str.Format(_T("wtf --- f: %.3f, pSeg1: %d, pSeg2: %d\n"), f, pSeg1->m_iIndex, pSeg2->m_iIndex);
	//	::OutputDebugString(str);
	//}
	float ed = 1e8;
	try
	{
		ed = sqrt(f);
	}
	catch (std::exception e)
	{
		::OutputDebugString(e.what());
		return ed;
	}
	return ed;
}

template <class T>
float ImgData<T>::CalcNewCriteria(int iIndex1, int iIndex2)
{
	if (iIndex1 == -1 || iIndex2 == -1)
		return 1e8;

	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];

	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float u = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
		//f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
		f += (pSeg1->m_Avg[i] * pSeg1->m_Avg[i] * n1 + pSeg2->m_Avg[i] * pSeg2->m_Avg[i] * n2 - (n1 + n2) * u * u);
	}
	float ed = sqrt(f / (float)pSeg1->m_Avg.size());
	return ed;
}

template <class T>
int ImgData<T>::FindMatch(int iIndex, int except = -1)
{
	if (iIndex != -1)
	{
		Segment* pSeg = segment[iIndex];
		float fEd_min = 1e8;
		int iFind = -1;

		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			if (pSeg->m_neighbours[j] == -1)
				continue;

			Segment* p = segment[pSeg->m_neighbours[j]];
			if (p == NULL || p->m_bBoundary)
			{
				continue;
			}
			// to change CalcEuclideanDistance to whatever you want
			float fValue = CalcEuclideanDistance(iIndex, p->m_iIndex);

			//float fValue = CalcNewCriteria(iIndex, p->m_iIndex);
			// find min
			if (fValue < fEd_min && fValue < condition.threshold)
			{
				fEd_min = fValue;
				iFind = p->m_iIndex;
			}
		}
		if (iFind >= 0)
		{
			return iFind;
		}
	}

	return -1;
}

template <class T>
void ImgData<T>::MergeSegment(int iIndex1, int iIndex2)
{
	// segment[iIndex1], segment[iIndex2] locked before enter this function
	if (iIndex1 == -1 || iIndex2 == -1)
		return;

	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];


	if (pSeg1 == NULL || pSeg2 ==NULL || pSeg1->m_bBoundary || pSeg2->m_bBoundary)
		return;

	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	std::vector<float> a1 = pSeg1->m_Avg;
	std::vector<float> a2 = pSeg2->m_Avg;

	std::vector<float> v1 = pSeg1->m_Variance;
	std::vector<float> v2 = pSeg2->m_Variance;

	ASSERT(a1.size() == a2.size());
	ASSERT(v1.size() == v2.size());

	// for each band
	for (int i = 0; i < (int)a1.size(); i++)
	{
		float avg1 = a1[i];
		float avg2 = a2[i];
		float var1 = v1[i];
		float var2 = v2[i];
		float avg = 0.0;
		avg= (avg1 * n1 + avg2 * n2) / (float)(n1 + n2);
		pSeg1->m_Avg[i] = avg;
		pSeg1->m_Variance[i] = (n1 * var1 + n2 * var2) / (n1 + n2) + (n1 * avg1 * avg1 + n2 * avg2 * avg2) / (n1 + n2) - avg * avg;
		//v1[i] = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - pSeg1->m_fAvg * pSeg1->m_fAvg;
	}
	//// s2->s1
	pSeg1->m_area.insert(pSeg1->m_area.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
	//pSeg2->m_area.clear();

	// update neighbours

	// remove seg2 from seg1's neighbours
	// put seg2 neighbours to seg1 (except seg1)
	// change seg2 neighbours to seg1 

	int n11 = pSeg1->m_neighbours.size();
	int n22 = pSeg2->m_neighbours.size();

	//if (n11 < 3 || n22 < 3)

	//{
	//	TRACE(_T("lalalala\n"));
	//}
	// remove seg2 from seg1's neighbours
	std::vector<int>::iterator iter = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), iIndex2);
	if (iter != pSeg1->m_neighbours.end())
		pSeg1->m_neighbours.erase(iter);


	// remove seg1 from seg2's neighbours
	iter = std::find(pSeg2->m_neighbours.begin(), pSeg2->m_neighbours.end(), iIndex1);
	if (iter != pSeg2->m_neighbours.end())
		pSeg2->m_neighbours.erase(iter);


	// change seg2 neighbours to seg1
	for (int i = 0; i < (int)pSeg2->m_neighbours.size(); i++)
	{
		//Neighbour nb;
		//nb.iIndex = pSeg2->m_neighbours[i].iIndex;
		//if (nb.iIndex == -1 || nb.iIndex == iIndex1)
		//	continue;
		int iIndex = pSeg2->m_neighbours[i];
		if (iIndex == -1 || iIndex == iIndex1 || iIndex == iIndex2)
			continue;

		Segment* pSeg = segment[iIndex];
		if (pSeg == NULL || pSeg->m_bBoundary)
			continue;
		// if n is seg1's neighbour
		std::vector<int>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), iIndex);
		
		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(iIndex);


		// check n's neighbour if has seg2 then change to seg1
		iter1 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex1);
		std::vector<int>::iterator iter2 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex2);

		ASSERT(iter2 != pSeg->m_neighbours.end());
		if (iter2 == pSeg->m_neighbours.end())
		{
			::OutputDebugString(_T("wtf\n"));
		}

		if (iter1 != pSeg->m_neighbours.end())
			pSeg->m_neighbours.erase(iter2);
		else
		{
			*iter2 = iIndex1;
		}
	}

	pSeg1->min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	pSeg1->min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	pSeg1->max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	pSeg1->max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
	//TRACE(_T("merge %d to %d\n"), pSeg2->m_iIndex, pSeg1->m_iIndex);
	segment[iIndex2] = NULL;
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
	for (int i = 0; i < width * height; i++)
	{
		if (segment[i] != NULL)
		{
			delete segment[i];
			segment[i] = NULL;
		}
	}

	delete[] segment;
}

template <class T>
void ImgData<T>::InitNeighbours()
{
	for (int i = 0; i < width * height; i++)
	{
		int r = segment[i]->m_area[0].row;
		int c = segment[i]->m_area[0].col;

		ASSERT(segment[i]->m_iIndex == r * width + c);

		// r-1, c-1
		if (r >= 1 && c >= 1)
		{
			int iIndex = (r - 1) * width + c - 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r-1, c
		if (r >= 1)
		{
			int iIndex = (r - 1) * width + c;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r-1, c+1
		if (r >= 1 && c < width - 1)
		{
			int iIndex = (r - 1) * width + c + 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r, c-1
		if (c >= 1)
		{
			int iIndex = r * width + c - 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r, c+1
		if (c < width - 1)
		{
			int iIndex = r * width + c + 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r+1, c-1
		if (r < height - 1 && c >= 1)
		{
			int iIndex = (r + 1) * width + c - 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r+1, c
		if (r < height - 1)
		{
			int iIndex = (r + 1) * width + c;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
		// r+1, c+1
		if (r < height - 1 && c < width - 1)
		{
			int iIndex = (r + 1) * width + c + 1;
			//Neighbour nb;
			//nb.iIndex = iIndex;
			//nb.pSeg = segment[iIndex];
			//segment[i]->m_neighbours.push_back(nb);
			segment[i]->m_neighbours.push_back(iIndex);
		}
	}
}

template <class T>
void ImgData<T>::Output(CString& strFile)
{
//	return;
	std::vector<std::vector<int>> vecResult;
	std::vector<int> vecRow;
	vecRow.assign(width, 0);
	vecResult.assign(height, vecRow);

	int iSum = 0;
	int iIndex = 0;
	for (int i = 0; i < width * height; i++)
	{
		if (segment[i] != NULL && segment[i]->m_iIndex != -1)
		{
			iIndex++;
			iSum += segment[i]->m_area.size();
			for (int j = 0; j < (int)segment[i]->m_area.size(); j++)
			{
				//vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = segment[i]->m_iIndex;
				vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = iIndex;
			}
		}
	}

	ASSERT(iSum == width * height);
	CStdioFile f;
	f.Open(strFile, CFile::modeCreate | CFile::modeReadWrite);

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

template <class T>
Segment* ImgData<T>::FindMatchviaArea(Segment* pSeg)
{
	if (pSeg != NULL && pSeg->m_iIndex != -1)
	{
		float fEd_min = 1e8;
		int index = -1;
		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			// calc
			Segment* p = pSeg->m_neighbours[j].pSeg;
			if (p == NULL || p->m_bBoundary)
				continue;

			// to change CalcEuclideanDistance to whatever you want
			//float fValue = CalcEuclideanDistance(pSeg, p);

			float fValue = CalcShapefactor(pSeg, p);
			// find min
			if (fValue < fEd_min && fValue < condition.shapefactor)
			{
				fEd_min = fValue;
				index = p->m_iIndex;
			}
		}
		if (index >= 0)
		{
			//TRACE(_T("%d match %d\n"), pSeg->m_iIndex, index);
			return segment[index];
		}
	}

	return NULL;
}

template <class T>
float ImgData<T>::CalcShapefactor(Segment* pSeg1, Segment* pSeg2)
{
	float sf = 1e8;

	std::vector<Pixel> vecArea;
	vecArea.insert(vecArea.end(), pSeg1->m_area.begin(), pSeg1->m_area.end());
	vecArea.insert(vecArea.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());

	if (vecArea.empty())
		return sf;

	int iSize = vecArea.size();

	int min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	int min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	int max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	int max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
	
	sf = (float)(iSize) / (float)((max_row - min_row + 1) * (max_col - min_col + 1));
	return sf;
}
#ifdef MULTI_THREAD
template <class T>
void ImgData<T>::ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end)
{
	//GetFromQue(row_start, row_end, col_start, col_end);

	//SYSTEM_INFO systeminfo;
	//::GetSystemInfo(&systeminfo);
	//CString strInfo;
	//strInfo.Format(_T("CPU total count %d. Thread %d is using num %d(%x)\n"), systeminfo.dwNumberOfProcessors, ::GetCurrentThreadId(), systeminfo.dwActiveProcessorMask, systeminfo.dwActiveProcessorMask);
	//::OutputDebugString(strInfo);

	//int iCpu = 1;
	//for (int i = 0; i < row_start; i++)
	//{
	//	iCpu = iCpu << 1;
	//}
	//DWORD dMask = ::SetThreadAffinityMask(::GetCurrentThread(), iCpu);

	//::GetSystemInfo(&systeminfo);
	//strInfo.Format(_T("After SetThreadAffinityMask. CPU total count %d. Thread %d is using num %d(%x), preMask is %d(%x)\n"), systeminfo.dwNumberOfProcessors, ::GetCurrentThreadId(), systeminfo.dwActiveProcessorMask, systeminfo.dwActiveProcessorMask, dMask, dMask);
	//::OutputDebugString(strInfo);
	while (true)
	{
		queLock.Lock();
		if (queBlock.empty())
		{
			queLock.Unlock();
			break;
		}
		Blockpair bp = queBlock.front();
		queBlock.pop();
		queLock.Unlock();
		int i = bp.i;
		int j = bp.j;

		row_start = i == 0 ? i * rInterval: i * rInterval + 1;
		row_end = (i == ROW_BLOCK - 1) ? height: (i + 1) * rInterval;

		col_start = j == 0 ? j * cInterval: j * cInterval + 1;
		col_end = (j == COL_BLOCK - 1) ? width: (j + 1) * cInterval;

		CString str;
		str.Format(_T("Process of [(%d, %d), (%d, %d)] begins~!!\n"), row_start, row_end, col_start, col_end);
		::OutputDebugString(str);

		int i1 = ::GetTickCount();
		Segment** pSegBlock = RetriveBlock(row_start, row_end, col_start, col_end);

		// calc with segBlock
		ProcessBlock(pSegBlock, row_start, row_end, col_start, col_end);

		RestoreBlock(pSegBlock, row_start, row_end, col_start, col_end);

		DestroyBlock(pSegBlock, row_start, row_end, col_start, col_end);
		int i2 = ::GetTickCount();

		str.Format(_T("Process of [(%d, %d), (%d, %d)] cost time %d(ms)\n"), row_start, row_end, col_start, col_end, i2 - i1);
		::OutputDebugString(str);
	}
	//// [start, end)
	//int iRound = 0;
	//while (true)
	//{
	//	int i1 = ::GetTickCount();
	//	bool bMatch = false;
	//	for (int i = row_start; i < row_end; i++)
	//	{
	//		for (int j = col_start; j < col_end; j++)
	//		{
	//			int iIndex = i * width + j;
	//			if (segment[iIndex] == NULL || segment[iIndex]->m_bBoundary)
	//			{
	//				continue;
	//			}

	//			int iIndex1 = FindMatch(iIndex);

	//			if (iIndex1 != -1)
	//			{
	//				int iIndex2 = FindMatch(iIndex1, iIndex);
	//				if (iIndex2 != -1 && iIndex == iIndex2)
	//				{
	//					MergeSegment(iIndex, iIndex1);
	//					bMatch = true;
	//				}
	//			}
	//		}
	//	}

	//	if (!bMatch)
	//	{
	//		CString str;
	//		str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] exit!!!!\n"), ++iRound, row_start, row_end, col_start, col_end);
	//		::OutputDebugString(str);
	//		break;
	//	}

	//	int i2 = ::GetTickCount();
	//	CString str;
	//	str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] cost time %d(ms)\n"), ++iRound, row_start, row_end, col_start, col_end, i2 - i1);
	//	::OutputDebugString(str);
	//}
}

template <class T>
void ImgData<T>::ThreadProcessviaArea(int& start, int& end)
{

}

//template <class T>
//void ImgData<T>::ThreadFind(int &start, int &end)
//{
//	int iRound = 0;
//	DWORD iThreadID = ::GetCurrentThreadId();
//	::Sleep(10);
//	std::map<DWORD, HANDLE>::iterator iter = mapSync.find(iThreadID);
//	HANDLE hCurrent = iter->second;
//	while (true)
//	{
//		int i1 = ::GetTickCount();
//		bool bMatch = false;
//		::WaitForSingleObject(hCurrent, INFINITE);
//		::WaitForSingleObject(hEmpty, INFINITE);
//		for (int i = start; i < end; i++)
//		{
//			// get merge lock 
//			if (segment[i] != NULL)
//			{
//				int iIndex = segment[i]->m_iIndex;
//				int iIndex1 = FindMatch(iIndex);
//
//				if (iIndex1 != -1)
//				{
//					// may cause dead lock here
//					int iIndex2 = FindMatch(iIndex1, iIndex);
//
//					if (iIndex2 != -1 && iIndex == iIndex2)
//					{
//						//MergeSegment(iIndex, iIndex1);
//						quelock.Lock();
//						MergePair mp;
//						mp.iIndex1 = iIndex;
//						mp.iIndex2 = iIndex1;
//						//queMerge
//						queMerge.push(mp);
//						bMatch = true;
//						quelock.Unlock();
//					}
//				}
//			}
//		}
//
//		if (!bMatch)
//		{
//			TRACE(_T("Round: %d of [%d, %d] exit!!!!\n"), ++iRound, start, end);
//			break;
//		}
//		else
//		{
//			::ResetEvent(hCurrent);
//			::SetEvent(hFull);
//		}
//
//		int i2 = ::GetTickCount();
//		TRACE(_T("Round: %d of [%d, %d] cost time %d(ms)\n"), ++iRound, start, end, i2 - i1);
//	}
//}

//template <class T>
//void ImgData<T>::ThreadMerge()
//{
//	while (!bExitMerge)
//	{
//		::WaitForSingleObject(hFull, INFINITE);
//
//		//quelock.Lock();
//		//if (queMerge.empty())
//		//{
//		//	quelock.Unlock();
//		//	continue;
//		//}
//		//MergePair mp = queMerge.front();
//		//queMerge.pop();
//		//
//
//		//::ResetEvent(mergeOperation);
//		//MergeSegment(mp.iIndex1, mp.iIndex2);
//		//::SetEvent(mergeOperation);
//		//quelock.Unlock();
//
//		::ResetEvent(hEmpty);
//		// wait for all find thread to pause
//		//::WaitForMultipleObjects(THREAD_FIND_COUNT, pHandleFind, TRUE, INFINITE);
//		quelock.Lock();
//		while (!queMerge.empty())
//		{
//			//if (findCount > 0)
//			//	TRACE(_T("wtf!!!!\n"));
//			MergePair mp = queMerge.front();
//			queMerge.pop();
//			MergeSegment(mp.iIndex1, mp.iIndex2);
//		}
//		quelock.Unlock();
//
//		for (int i = 0; i < THREAD_COUNT; i++)
//			::SetEvent(pHandleFind[i]);
//		::SetEvent(hEmpty);
//	}
//}

template <class T>
void ImgData<T>::Divide()
{
	// make a pre process seed
	// THREAD_COUNT must be 4, 9, 16, etc
	cInterval = width / COL_BLOCK;
	rInterval = height / ROW_BLOCK;

	std::set<int> setRow;
	std::set<int> setCol;
	for (int i = 1; i < ROW_BLOCK; i++)
	{
		setRow.insert(rInterval * i);
	}
	for (int i = 1; i < COL_BLOCK; i++)
	{
		setCol.insert(cInterval * i);
	}

	// row
	for (std::set<int>::iterator iter = setRow.begin(); iter != setRow.end(); iter++)
	{
		int iRow = *iter;
		for (int i = 0; i < width; i++)
		{
			Pixel p;
			p.row = iRow;
			p.col = i;
			vecBoundary.push_back(p);
		}
	}

	// col
	for (std::set<int>::iterator iter = setCol.begin(); iter != setCol.end(); iter++)
	{
		int iCol = *iter;
		for (int i = 0; i < height; i++)
		{
			Pixel p;
			p.row = i;
			p.col = iCol;

			if (i > 0 && i % rInterval == 0)
			{
				std::set<int>::iterator iter = setRow.find(i);
				if (iter != setRow.end())
				{
					TRACE(_T("already exist (%d, %d)\n"), i, iCol);
					continue;
				}
			}
			vecBoundary.push_back(p);
		}
	}

	ASSERT(vecBoundary.size() == (ROW_BLOCK - 1) * width + (COL_BLOCK - 1) * height - (ROW_BLOCK - 1) * (COL_BLOCK - 1));
}

template <class T>
void ImgData<T>::SetBoundary()
{
	for (int i = 0; i < (int)vecBoundary.size(); i++)
	{
		int iIndex = vecBoundary[i].row * width + vecBoundary[i].col;

		// after merge maybe NULL
		//TRACE(_T("row:%d, col:%d, index:%d\n"), vecBoundary[i].row, vecBoundary[i].col, iIndex);
		if (segment[iIndex] != NULL)
		{
			segment[iIndex]->m_bBoundary = true;

			/////////////////////////////
			for (int j = 0; j < (int)segment[iIndex]->m_neighbours.size(); j++)
			{
				int iNeighbour = segment[iIndex]->m_neighbours[j];
				Segment* pNeighbour = segment[iNeighbour];

				if (pNeighbour == NULL || pNeighbour->m_bBoundary == true)
					continue;

				pNeighbour->m_bBoundary = true;
			}
			//////////////////////////////
		}
	}
}

template <class T>
void ImgData<T>::ClearBoundary()
{
	for (int i = 0; i < (int)vecBoundary.size(); i++)
	{
		int iIndex = vecBoundary[i].row * width + vecBoundary[i].col;
		if (segment[iIndex] != NULL)
		{
			segment[iIndex]->m_bBoundary = false;

			/////////////////////////////
			for (int j = 0; j < (int)segment[iIndex]->m_neighbours.size(); j++)
			{
				int iNeighbour = segment[iIndex]->m_neighbours[j];
				Segment* pNeighbour = segment[iNeighbour];

				if (pNeighbour == NULL || pNeighbour->m_bBoundary == false)
					continue;

				pNeighbour->m_bBoundary = false;
			}
			//////////////////////////////
		}
	}
}
//template <class T>
//bool ImgData<T>::LockNecessarySegments(int iIndex)
//{
//	//bigcs.Lock();
//	std::set<int>::iterator iter = lockSet.find(iIndex);
//	if (iIndex > -1)
//	{
//		if (iter != lockSet.end())
//		{
//			//bigcs.Unlock();
//			return false;
//		}
//		//// current process segment
//		//CCriticalSection* pCur = cs[iIndex];
//		//pCur->Lock();
//
//		Segment* pSeg = segment[iIndex];
//		
//		if (pSeg == NULL)
//		{
//			//bigcs.Unlock();
//			return false;
//		}
//		else
//		{
//			std::set<int> tobeLocked;
//			tobeLocked.insert(iIndex);
//			// neighbour of cur
//			for (int i = 0; i < (int)pSeg->m_neighbours.size(); i++)
//			{
//				if (pSeg->m_neighbours[i].iIndex > -1)
//				{
//					//CCriticalSection* pCurNeighbour = cs[pSeg->m_neighbours[i].iIndex];
//
//					//pCurNeighbour->Lock();
//
//					Segment* pSegNeighbour = pSeg->m_neighbours[i].pSeg;
//					//if (pSegNeighbour == NULL)
//					//{
//					//	pCurNeighbour->Unlock();
//					//}
//					//else
//					//{
//					//	lockSet.insert(pSeg->m_neighbours[i].iIndex);
//					//}
//
//					iter = lockSet.find(pSeg->m_neighbours[i].iIndex);
//					if (iter != lockSet.end())
//					{
//						//bigcs.Unlock();
//						return false;
//					}
//					else
//						tobeLocked.insert(pSeg->m_neighbours[i].iIndex);
//
//
//					// neighbour of cur's neighbour
//					for (int j = 0; j < (int)pSegNeighbour->m_neighbours.size(); j++)
//					{
//						if (pSegNeighbour->m_neighbours[j].iIndex > -1)
//						{
//							iter = lockSet.find(pSegNeighbour->m_neighbours[j].iIndex);
//							if (iter != lockSet.end())
//							{
//								//bigcs.Unlock();
//								return false;
//							}
//							else
//							{
//								tobeLocked.insert(pSegNeighbour->m_neighbours[j].iIndex);
//								//CCriticalSection* pNoN = cs[pSegNeighbour->m_neighbours[j].iIndex];
//								//pNoN->Lock();
//
//								//Segment* pSegNoN = pSegNeighbour->m_neighbours[j].pSeg;
//
//								//if (pSegNoN == NULL)
//								//{
//								//	pNoN->Unlock();
//								//}
//								//else
//								//{
//								//	lockSet.insert(pSegNeighbour->m_neighbours[j].iIndex);
//								//}
//							}
//						}
//						// already locked
//					} // end of for cur's neighbour
//				}
//			} // end of for cur
//			// put tobelocked into lockSet
//			bigcs.Lock();
//			lockSet.insert(tobeLocked.begin(), tobeLocked.end());
//			bigcs.Unlock();
//			return true;
//		}
//
//		// neighbour of cur's neighbour
//	}
//	//bigcs.Unlock();
//	return false;
//}
//
//template <class T>
//void ImgData<T>::UnlockNecessarySegments(int iIndex)
//{
//	bigcs.Lock();
//	if (iIndex > -1)
//	{
//		//// current process segment
//		Segment* pSeg = segment[iIndex];
//
//		//CCriticalSection* pCur = cs[iIndex];
//		//pCur->Unlock();
//		std::set<int>::iterator iter = lockSet.find(iIndex);
//		ASSERT(iter != lockSet.end());
//		lockSet.erase(iter);
//		if (pSeg != NULL)
//		// neighbour of cur
//		{
//			for (int i = 0; i < (int)pSeg->m_neighbours.size(); i++)
//			{
//				//CCriticalSection* pCur = cs[pSeg->m_neighbours[i].iIndex];
//				//pCur->Unlock();
//
//				iter = lockSet.find(pSeg->m_neighbours[i].iIndex);
//				if (iter != lockSet.end())
//					lockSet.erase(iter);
//				Segment* pNeighbour = pSeg->m_neighbours[i].pSeg;
//				if (pSeg != NULL)
//				{
//					for (int j = 0; j < (int)pNeighbour->m_neighbours.size(); j++)
//					{
//						//CCriticalSection* pNoN = cs[pNeighbour->m_neighbours[j].iIndex];
//						//pNoN->Unlock();
//						iter = lockSet.find(pNeighbour->m_neighbours[j].iIndex);
//						if (iter != lockSet.end())
//							lockSet.erase(iter);
//					}
//				}
//			}
//		}
//		// neighbour of cur's neighbour
//	}
//	bigcs.Unlock();
//}

template <class T>
Segment** ImgData<T>::RetriveBlock(int& row_start, int& row_end, int& col_start, int& col_end)
{
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;
	Segment** pSegBlock = new Segment*[rowCount * colCount];

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < colCount; j++)
		{
			int iBlockIndex = i * colCount + j;
			int iOriginalIndex = (i + row_start) * width + (j + col_start);
			// get correspond segment 
			// copy to block and chang index include neighbour

			Segment* pSeg = Original2BlockSegment(i, j, row_start, row_end, col_start, col_end);

			//TRACE(_T("Retrive(ori---block) [%d(%d, %d)]---[%d(%d, %d)]\n"), iOriginalIndex, i + row_start, j + col_start, iBlockIndex, i, j);

			pSegBlock[iBlockIndex] = pSeg;
		}
	}

	return pSegBlock;
}
template <class T>
void ImgData<T>::ProcessBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int iRound = 0;
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;
	while (true)
	{
		//int i1 = ::GetTickCount();
		bool bMatch = false;
		for (int i = 0; i < rowCount; i++)
		{
			for (int j = 0; j < colCount; j++)
			{
				int iIndex = i * colCount + j;
				if (pSegBlock[iIndex] == NULL || pSegBlock[iIndex]->m_bBoundary)
				{
					continue;
				}

				int iIndex1 = FindMatch(pSegBlock, iIndex, row_start, row_end, col_start, col_end);

				if (iIndex1 != -1)
				{
					int iIndex2 = FindMatch(pSegBlock, iIndex1, row_start, row_end, col_start, col_end, iIndex);
					if (iIndex2 != -1 && iIndex == iIndex2)
					{
						MergeSegment(pSegBlock, iIndex, iIndex1, row_start, row_end, col_start, col_end);
						bMatch = true;
					}
				}
				//::Sleep(1);
			}
		}

		if (!bMatch)
		{
			//CString str;
			//str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] exit!!!!\n"), ++iRound, row_start, row_end, col_start, col_end);
			//::OutputDebugString(str);
			break;
		}

		//int i2 = ::GetTickCount();
		//CString str;
		//str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] cost time %d(ms)\n"), ++iRound, row_start, row_end, col_start, col_end, i2 - i1);
		//::OutputDebugString(str);

		//Sleep(1);
	}
}

template <class T>
void ImgData<T>::RestoreBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < colCount; j++)
		{
			int iBlockIndex = i * colCount + j;
			int iOriginalIndex = (i + row_start) * width + (j + col_start);

			Segment* pBlock = pSegBlock[iBlockIndex];
			Segment* pOriginal = segment[iOriginalIndex];
			// get correspond segment 
			// copy to block and chang index include neighbour

			Block2OriginalSegment(pOriginal, pBlock, i, j, row_start, row_end, col_start, col_end);
		}
	}
}
template <class T>
void ImgData<T>::DestroyBlock(Segment** pSegBlock, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < colCount; j++)
		{
			int iIndex = i * colCount + j;
			delete pSegBlock[iIndex];
			pSegBlock[iIndex] = NULL;
		}
	}

	delete[] pSegBlock;
	pSegBlock = NULL;
}

template <class T>
int ImgData<T>::FindMatch(Segment** pSegBlock, int iIndex, int& row_start, int& row_end, int& col_start, int& col_end, int except = -1)
{
	if (iIndex != -1)
	{
		int colCount = col_end - col_start;
		Segment* pSeg = pSegBlock[iIndex];
		float fEd_min = 1e8;
		int iFind = -1;

		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			int iNeighbour = pSeg->m_neighbours[j];
			if (iNeighbour > width * height)
				continue;
			Segment* p = pSegBlock[pSeg->m_neighbours[j]];
			if (p == NULL || p->m_bBoundary)
			{
				continue;
			}
			// to change CalcEuclideanDistance to whatever you want
			float fValue = CalcEuclideanDistance(pSegBlock, iIndex, p->m_iIndex);

			//float fValue = CalcNewCriteria(iIndex, p->m_iIndex);
			// find min
			if (fValue < fEd_min && fValue < condition.threshold)
			{
				fEd_min = fValue;
				iFind = p->m_iIndex;
			}
		}
		if (iFind >= 0)
		{
			return iFind;
		}
	}

	return -1;
}

template <class T>
void ImgData<T>::MergeSegment(Segment** pSegBlock, int& iIndex1, int& iIndex2, int& row_start, int& row_end, int& col_start, int& col_end)
{
	// pSegBlock[iIndex1], pSegBlock[iIndex2] locked before enter this function
	if (iIndex1 == -1 || iIndex2 == -1)
		return;

	Segment* pSeg1 = pSegBlock[iIndex1];
	Segment* pSeg2 = pSegBlock[iIndex2];


	if (pSeg1 == NULL || pSeg2 ==NULL || pSeg1->m_bBoundary || pSeg2->m_bBoundary)
		return;

	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	std::vector<float> a1 = pSeg1->m_Avg;
	std::vector<float> a2 = pSeg2->m_Avg;

	std::vector<float> v1 = pSeg1->m_Variance;
	std::vector<float> v2 = pSeg2->m_Variance;

	ASSERT(a1.size() == a2.size());
	ASSERT(v1.size() == v2.size());

	// for each band
	for (int i = 0; i < (int)a1.size(); i++)
	{
		float avg1 = a1[i];
		float avg2 = a2[i];
		float var1 = v1[i];
		float var2 = v2[i];
		float avg = 0.0;
		avg= (avg1 * n1 + avg2 * n2) / (float)(n1 + n2);
		pSeg1->m_Avg[i] = avg;
		pSeg1->m_Variance[i] = (n1 * var1 + n2 * var2) / (n1 + n2) + (n1 * avg1 * avg1 + n2 * avg2 * avg2) / (n1 + n2) - avg * avg;
		//v1[i] = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - pSeg1->m_fAvg * pSeg1->m_fAvg;
	}
	//// s2->s1
	pSeg1->m_area.insert(pSeg1->m_area.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
	//pSeg2->m_area.clear();

	// update neighbours

	// remove seg2 from seg1's neighbours
	// put seg2 neighbours to seg1 (except seg1)
	// change seg2 neighbours to seg1 

	// remove seg2 from seg1's neighbours
	std::vector<int>::iterator iter = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), iIndex2);
	if (iter != pSeg1->m_neighbours.end())
		pSeg1->m_neighbours.erase(iter);


	// remove seg1 from seg2's neighbours
	iter = std::find(pSeg2->m_neighbours.begin(), pSeg2->m_neighbours.end(), iIndex1);
	if (iter != pSeg2->m_neighbours.end())
		pSeg2->m_neighbours.erase(iter);


	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	

	// change seg2 neighbours to seg1
	for (int i = 0; i < (int)pSeg2->m_neighbours.size(); i++)
	{
		int iIndex = pSeg2->m_neighbours[i];
		if (iIndex == -1 || iIndex == iIndex1 || iIndex == iIndex2)
			continue;

		if (iIndex > width * height)
			continue;

		Segment* pSeg = pSegBlock[iIndex];
		if (pSeg == NULL || pSeg->m_bBoundary)
			continue;
		// if n is seg1's neighbour
		std::vector<int>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), iIndex);
		
		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(iIndex);


		// check n's neighbour if has seg2 then change to seg1
		iter1 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex1);
		std::vector<int>::iterator iter2 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex2);

		ASSERT(iter2 != pSeg->m_neighbours.end());
		if (iter2 == pSeg->m_neighbours.end())
		{
			::OutputDebugString(_T("wtf\n"));
		}

		if (iter1 != pSeg->m_neighbours.end())
			pSeg->m_neighbours.erase(iter2);
		else
		{
			*iter2 = iIndex1;
		}
	}

	pSeg1->min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	pSeg1->min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	pSeg1->max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	pSeg1->max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
	//TRACE(_T("merge %d to %d\n"), pSeg2->m_iIndex, pSeg1->m_iIndex);
	pSegBlock[iIndex2] = NULL;
	delete pSeg2;
	pSeg2 = NULL;
}

template <class T>
Segment* ImgData<T>::Original2BlockSegment(int& iBlockRow, int& iBlockCol, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	int iBlockIndex = iBlockRow * colCount + iBlockCol;
	int iOriginalIndex = (iBlockRow + row_start) * width + (iBlockCol + col_start);
	
	Segment* pSegOriginal = segment[iOriginalIndex];

	if (pSegOriginal == NULL)
		return NULL;
	Segment* pSeg = new Segment;
	
	pSeg->m_bBoundary = pSegOriginal->m_bBoundary;
	pSeg->m_iIndex = iBlockIndex;
	pSeg->m_Avg = pSegOriginal->m_Avg;
	pSeg->m_Variance = pSegOriginal->m_Variance;
	pSeg->max_row = pSegOriginal->max_row - row_start;
	pSeg->max_col = pSegOriginal->max_col - col_start;
	pSeg->min_row = pSegOriginal->min_row - row_start;
	pSeg->min_col = pSegOriginal->min_col - col_start;

	// neighbour
	for (int i = 0; i < (int)pSegOriginal->m_neighbours.size(); i++)
	{
		//Segment* pNeighbour = segment[pSegOriginal->m_neighbours[i]];
		//if (pNeighbour == NULL || pNeighbour->m_bBoundary)
		//	continue;
		int iBlockIndex = Original2Block(pSegOriginal->m_neighbours[i], row_start, row_end, col_start, col_end);
		pSeg->m_neighbours.push_back(iBlockIndex);
	}
	// area
	for (int i = 0; i < (int)pSegOriginal->m_area.size(); i++)
	{
		Pixel pxOri = pSegOriginal->m_area[i];
		pxOri.row -= row_start;
		pxOri.col -= col_start;
		pSeg->m_area.push_back(pxOri);
	}
	return pSeg;
}

template <class T>
int ImgData<T>::Block2Original(int& iBlockIndex, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int iRet = -1;

	if (iBlockIndex > width * height)
	{
		return iBlockIndex - width * height;
	}
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	int rowBlock = iBlockIndex / colCount;
	int colBlock = iBlockIndex % colCount;

	int rowOri = rowBlock + row_start;
	int colOri = colBlock + col_start;

	iRet = rowOri * width + colOri;

	return iRet;
}

template <class T>
int ImgData<T>::Original2Block(int& iOriginalIndex, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int iRet = -1;
	int rowOri = iOriginalIndex / width;
	int colOri = iOriginalIndex % width;

	if (rowOri < row_start || rowOri >= row_end
		|| colOri < col_start || colOri >= col_end)
	{
		/// has to be a invalid value and can be calc back
		return iOriginalIndex + width * height;
	}
	//int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	int rowBlock = rowOri - row_start;
	int colBlock = colOri - col_start;

	iRet = rowBlock * colCount + colBlock;
	return iRet;
}

template <class T>
void ImgData<T>::Block2OriginalSegment(Segment* pSegOriginal, Segment* pSegBlock, int& iBlockRow, int& iBlockCol, int& row_start, int& row_end, int& col_start, int& col_end)
{
	int rowCount = row_end - row_start;
	int colCount = col_end - col_start;

	int iBlockIndex = iBlockRow * colCount + iBlockCol;
	int iOriginalIndex = (iBlockRow + row_start) * width + (iBlockCol + col_start);
	
	if (pSegBlock == NULL)
	{
		if (pSegOriginal != NULL)
		{
			delete pSegOriginal;
			segment[iOriginalIndex] = NULL;
		}
		return;
	}

	ASSERT(pSegOriginal != NULL);

	pSegOriginal->m_bBoundary = pSegBlock->m_bBoundary;
	pSegOriginal->m_iIndex = iOriginalIndex;
	pSegOriginal->m_Avg = pSegBlock->m_Avg;
	pSegOriginal->m_Variance = pSegBlock->m_Variance;
	pSegOriginal->max_row = pSegBlock->max_row + row_start;
	pSegOriginal->max_col = pSegBlock->max_col + col_start;
	pSegOriginal->min_row = pSegBlock->min_row + row_start;
	pSegOriginal->min_col = pSegBlock->min_col + col_start;

	// neighbour
	pSegOriginal->m_neighbours.clear();
	for (int i = 0; i < (int)pSegBlock->m_neighbours.size(); i++)
	{
		int iOriginalIndex = Block2Original(pSegBlock->m_neighbours[i], row_start, row_end, col_start, col_end);
		pSegOriginal->m_neighbours.push_back(iOriginalIndex);
	}
	// area
	pSegOriginal->m_area.clear();
	for (int i = 0; i < (int)pSegBlock->m_area.size(); i++)
	{
		Pixel pxOri = pSegBlock->m_area[i];
		pxOri.row += row_start;
		pxOri.col += col_start;
		pSegOriginal->m_area.push_back(pxOri);
	}
}

template <class T>
float ImgData<T>::CalcEuclideanDistance(Segment** pSegBlock, int iIndex1, int iIndex2)
{
	//segment[iIndex1], segment[iIndex2] lock befor enter this function
	if (iIndex1 == -1 || iIndex2 == -1)
		return 1e8;

	Segment* pSeg1 = pSegBlock[iIndex1];
	Segment* pSeg2 = pSegBlock[iIndex2];
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());

	// to be continued...
	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}

	//if (f < 0.1)
	//{
	//	CString str;
	//	str.Format(_T("wtf --- f: %.3f, pSeg1: %d, pSeg2: %d\n"), f, pSeg1->m_iIndex, pSeg2->m_iIndex);
	//	::OutputDebugString(str);
	//}
	float ed = 1e8;
	try
	{
		ed = sqrt(f);
	}
	catch (std::exception e)
	{
		::OutputDebugString(e.what());
		return ed;
	}
	return ed;
}

template <class T>
float ImgData<T>::CalcNewCriteria(Segment** pSegBlock, int iIndex1, int iIndex2)
{
	if (iIndex1 == -1 || iIndex2 == -1)
		return 1e8;

	Segment* pSeg1 = pSegBlock[iIndex1];
	Segment* pSeg2 = pSegBlock[iIndex2];

	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float u = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
		//f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
		f += (pSeg1->m_Avg[i] * pSeg1->m_Avg[i] * n1 + pSeg2->m_Avg[i] * pSeg2->m_Avg[i] * n2 - (n1 + n2) * u * u);
	}
	float ed = sqrt(f / (float)pSeg1->m_Avg.size());
	return ed;
}
#endif