#include "StdAfx.h"
#include "Datafile.h"

//CRITICAL_SECTION g_cs;
//
//HANDLE g_event_find;
//HANDLE g_event_merge;

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
DWORD WINAPI ThreadProc_ProcessAll(LPVOID pParam)
{
	try
	{
		threadParam* p = (threadParam*)pParam;
		InterfaceData* pData = p->pParam;
		pData->ThreadProcessAll(p->start, p->end);
		return 0x1001;
	}
	catch (std::exception e)
	{
		TRACE(_T("%s\n"), e.what());
		return 0x2001;
	}
}
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
	// segmentation
	SetCondition(con);

	Initialize();

	InitNeighbours();

	ProcessviaSeed(vecSeed);

	ProcessAll();

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
			if (segment[iIndex] == NULL || segment[iIndex]->m_iIndex == -1)
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

template <class T>
void ImgData<T>::ProcessAll()
{
#ifdef MULTI_THREAD
	// use multithreads
	//int iThreadCount = get_core_count() * 2;
	int iThreadCount = 4;

	HANDLE * pHandle = new HANDLE[iThreadCount];
	threadParam* pThreadParam = new threadParam[iThreadCount];

	int iCount = (int)width * height;
	int iBlock = iCount / iThreadCount;
	for (int i = 0; i < iThreadCount; i++)
	{
		DWORD dwThreadID = 0;
		pThreadParam[i].pParam = (InterfaceData*)this;
		pThreadParam[i].start = i * iBlock;
		pThreadParam[i].end = (i + 1) * iBlock;

		if (i == iThreadCount - 1)
			pThreadParam[i].end = iCount;
		pHandle[i] = CreateThread(NULL, 0, ThreadProc_ProcessAll, &pThreadParam[i], 0, &dwThreadID);
		threadIDs.push_back(dwThreadID);
		threads.push_back(pHandle[i]);
	}

	::WaitForMultipleObjects(iThreadCount, pHandle, TRUE, INFINITE);

	for (int i= 0; i < iThreadCount; i++)
		::CloseHandle(pHandle[i]);
	delete[] pHandle;
	delete[] pThreadParam;
#else
	int iTime = 0;
	while (true)
	{
		int i1 = ::GetTickCount();
		bool bMatch = false;
		for (int i = 0; i < (int)segment.size(); i++)
		{
			if (segment[i] == NULL || segment[i]->m_iIndex == -1)
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
#endif
}

template <class T>
void ImgData<T>::ProcessviaArea()
{
	while (true)
	{
		bool bMatch = false;
		for (int i = 0; i < (int)segment.size(); i++)
		{
			if (segment[i] == NULL || segment[i]->m_iIndex == -1)
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
	//InitializeCriticalSection(&g_cs);
	//g_event_find = ::CreateEvent(NULL, TRUE, TRUE, NULL);
	//g_event_merge = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// clear all segments
	segment.clear();

	//invalidSegment.m_iIndex = -1;
	//invalidSegment.max_col = invalidSegment.max_row = invalidSegment.min_col = invalidSegment.min_row = 0;
	//threshold = 20000;

	// each pixel is assigned as a distinct segment
	if (data.size() > 0)
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
				segment.push_back(pSeg);
#ifdef MULTI_THREAD
				CCriticalSection* pCs = new CCriticalSection;
				cs.push_back(pCs);
#endif
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
	// segment[index] is locked outside
	if (iIndex != -1)
	{
		Segment* pSeg = segment[iIndex];
		float fEd_min = 1e8;
		int iFind = -1;

		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			if (pSeg->m_neighbours[j].iIndex == -1)
				continue;
			// calc
			//<dynamic_cast>
#ifdef MULTI_THREAD
			CCriticalSection* pSegcs = cs[pSeg->m_neighbours[j].iIndex];

			if (except < 0)
				pSegcs->Lock();
			else if (pSeg->m_neighbours[j].iIndex != except)
				pSegcs->Lock();
			//else
			//	TRACE(_T("lalala:%d, %d\n"), iIndex, except);
			// else must be lock somewhere 
#endif
			Segment* p = segment[pSeg->m_neighbours[j].iIndex];
			if (p == NULL || p->m_iIndex == -1)
			{
#ifdef MULTI_THREAD
				if (except < 0)
					pSegcs->Unlock();
				else if (pSeg->m_neighbours[j].iIndex != except)
					pSegcs->Unlock();
#endif
				continue;
			}

			// to change CalcEuclideanDistance to whatever you want
			float fValue = CalcEuclideanDistance(iIndex, p->m_iIndex);

			fValue = 0.0;
			//float fValue = CalcNewCriteria(iIndex, p->m_iIndex);
			// find min
			if (fValue < fEd_min && fValue < condition.threshold)
			{
				fEd_min = fValue;
				iFind = p->m_iIndex;
			}
#ifdef MULTI_THREAD
			if (except < 0)
				pSegcs->Unlock();
			else if (pSeg->m_neighbours[j].iIndex != except)
				pSegcs->Unlock();
#endif
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
	return;
	// segment[iIndex1], segment[iIndex2] locked before enter this function
	if (iIndex1 == -1 || iIndex2 == -1)
		return;
	
	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];


	if (pSeg1 == NULL || pSeg2 ==NULL)
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
	Neighbour nb1;
	Neighbour nb2;
	nb1.iIndex = iIndex1;
	nb1.pSeg = pSeg1;
	nb2.iIndex = iIndex2;
	nb2.pSeg = pSeg2;

	// remove seg2 from seg1's neighbours
	std::vector<Neighbour>::iterator iter = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), nb2);
	if (iter != pSeg1->m_neighbours.end())
		pSeg1->m_neighbours.erase(iter);

	// remove seg1 from seg2's neighbours
	iter = std::find(pSeg2->m_neighbours.begin(), pSeg2->m_neighbours.end(), nb1);
	if (iter != pSeg2->m_neighbours.end())
		pSeg2->m_neighbours.erase(iter);

	// change seg2 neighbours to seg1
	for (int i = 0; i < (int)pSeg2->m_neighbours.size(); i++)
	{
		Neighbour nb;
		nb.iIndex = pSeg2->m_neighbours[i].iIndex;
		if (nb.iIndex == -1 || nb.iIndex == iIndex1)
			continue;
#ifdef MULTI_THREAD
		CCriticalSection* pSegcs = cs[nb.iIndex];
		pSegcs->Lock();
#endif
		nb.pSeg = pSeg2->m_neighbours[i].pSeg;

		// if n is seg1's neighbour
		std::vector<Neighbour>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), nb);
		
		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(nb);


		// check n's neighbour if has seg2 then change to seg1
		iter1 = std::find(nb.pSeg->m_neighbours.begin(), nb.pSeg->m_neighbours.end(), nb1);
		std::vector<Neighbour>::iterator iter2 = std::find(nb.pSeg->m_neighbours.begin(), nb.pSeg->m_neighbours.end(), nb2);

		ASSERT(iter2 != nb.pSeg->m_neighbours.end());
		if (iter2 == nb.pSeg->m_neighbours.end())
		{
			::OutputDebugString(_T("wtf\n"));
		}

		if (iter1 != nb.pSeg->m_neighbours.end())
			nb.pSeg->m_neighbours.erase(iter2);
		else
		{
			iter2->iIndex = nb1.iIndex;
			iter2->pSeg = nb1.pSeg;
		}
#ifdef MULTI_THREAD
		pSegcs->Unlock();
#endif
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
	for (int i = 0; i < (int)segment.size(); i++)
	{
		if (segment[i] != NULL)
		{
			delete segment[i];
			segment[i] = NULL;
		}

		//if (cs[i] != NULL)
		//{
		//	cs[i]->Unlock();
		//	delete cs[i];
		//	cs[i] = NULL;
		//}
	}

	//::DeleteCriticalSection(&g_cs);
}

template <class T>
void ImgData<T>::InitNeighbours()
{
	for (int i = 0; i < (int)segment.size(); i++)
	{
		int r = segment[i]->m_area[0].row;
		int c = segment[i]->m_area[0].col;

		ASSERT(segment[i]->m_iIndex == r * width + c);

		// r-1, c-1
		if (r >= 1 && c >= 1)
		{
			int iIndex = (r - 1) * width + c - 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r-1, c
		if (r >= 1)
		{
			int iIndex = (r - 1) * width + c;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r-1, c+1
		if (r >= 1 && c < width - 1)
		{
			int iIndex = (r - 1) * width + c + 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r, c-1
		if (c >= 1)
		{
			int iIndex = r * width + c - 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r, c+1
		if (c < width - 1)
		{
			int iIndex = r * width + c + 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r+1, c-1
		if (r < height - 1 && c >= 1)
		{
			int iIndex = (r + 1) * width + c - 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r+1, c
		if (r < height - 1)
		{
			int iIndex = (r + 1) * width + c;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
		}
		// r+1, c+1
		if (r < height - 1 && c < width - 1)
		{
			int iIndex = (r + 1) * width + c + 1;
			Neighbour nb;
			nb.iIndex = iIndex;
			nb.pSeg = segment[iIndex];
			segment[i]->m_neighbours.push_back(nb);
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
	for (int i = 0; i < (int)segment.size(); i++)
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

	ASSERT(iSum == (int)segment.size());
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
			if (pSeg->m_neighbours[j] == NULL)
				continue;
			// calc
			Segment* p = (Segment*)(pSeg->m_neighbours[j]);
			if (p == NULL || p->m_iIndex == -1)
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
void ImgData<T>::ThreadProcessAll(int& start, int& end)
{
	int iRound = 0;
	while (true)
	{
		int i1 = ::GetTickCount();
		bool bMatch = true;
		for (int i = start; i < end; i++)
		{
			// get lock 
			CCriticalSection* pSegcs = cs[i];
			if (pSegcs == NULL)
				continue;

			pSegcs->Lock();
			if (segment[i] == NULL || segment[i]->m_iIndex == -1)
			{
				pSegcs->Unlock();
				continue;
			}

			int iIndex = segment[i]->m_iIndex;
			int iIndex1 = FindMatch(iIndex);

			if (iIndex1 != -1)
			{
				CCriticalSection* pSegcs1 = cs[iIndex1];
				pSegcs1->Lock();
				// may cause dead lock here
				int iIndex2 = FindMatch(iIndex1, iIndex);
				if (iIndex2 != -1 && iIndex == iIndex2)
				{
					MergeSegment(iIndex, iIndex1);
					bMatch = true;
				}
				pSegcs1->Unlock();
			}
			pSegcs->Unlock();
		}

		if (!bMatch)
		{
			TRACE(_T("Round: %d of [%d, %d] exit!!!!\n"), ++iRound, start, end);
			break;
		}

		int i2 = ::GetTickCount();
		TRACE(_T("Round: %d of [%d, %d] cost time %d(ms)\n"), ++iRound, start, end, i2 - i1);
	}
}

template <class T>
void ImgData<T>::ThreadProcessviaArea(int& start, int& end)
{
}

template <class T>
void ImgData<T>::LockNecessarySegments(int iIndex)
{
	bigcs.Lock();
	if (iIndex > 0)
	{
		std::set<int> lockSet;
		// current process segment
		CCriticalSection* pCur = cs[iIndex];
		pCur->Lock();

		Segment* pSeg = segment[iIndex];
		
		if (pSeg == NULL)
			pCur->Unlock();
		else
		{
			lockSet.insert(iIndex);
			// neighbour of cur
			for (int i = 0; i < pSeg->m_neighbours.size(); i++)
			{
				if (pSeg->m_neighbours[i].iIndex > 0)
				{
					CCriticalSection* pCurNeighbour = cs[pSeg->m_neighbours[i].iIndex];

					pCurNeighbour->Lock();

					Segment* pSegNeighbour = pSeg->m_neighbours[i].pSeg;
					if (pSegNeighbour == NULL)
					{
						pCurNeighbour->Unlock();
					}
					else
					{
						lockSet.insert(pSeg->m_neighbours[i].iIndex);
					}

					// neighbour of cur's neighbour
					for (int j = 0; j < pSegNeighbour->m_neighbours.size(); j++)
					{
						std::set<int>::iterator iter = lockSet.find(pSegNeighbour->m_neighbours[j].iIndex);
						if (iter == lockSet.end())
						{
							CCriticalSection* pNoN = cs[pSegNeighbour->m_neighbours[j].iIndex];
							pNoN->Lock();

							Segment* pSegNoN = pSegNeighbour->m_neighbours[j].pSeg;

							if (pSegNoN == NULL)
							{
								pNoN->Unlock();
							}
							else
							{
								lockSet.insert(pSegNeighbour->m_neighbours[j].iIndex);
							}
						}
						// already locked
					}
				}
			}
		}

		// neighbour of cur's neighbour
	}
	bigcs.Unlock();
}

template <class T>
void ImgData<T>::UnlockNecessarySegments(int iIndex)
{
	bigcs.Lock();
	if (iIndex > 0)
	{
		// current process segment
		Segment* pSeg = segment[iIndex];

		CCriticalSection* pCur = cs[iIndex];
		pCur->Unlock();
		if (pSeg != NULL)
		// neighbour of cur
		{
			for (int i = 0; i < pSeg->m_neighbours.size(); i++)
			{
				CCriticalSection* pCur = cs[pSeg->m_neighbours[i].iIndex];
				pCur->Unlock();

				Segment* pNeighbour = pSeg->m_neighbours[i].pSeg;
				if (pSeg != NULL)
				{
					for (int j = 0; j < pNeighbour->m_neighbours.size(); j++)
					{
						CCriticalSection* pNoN = cs[pNeighbour->m_neighbours[i].iIndex];
						pNoN->Unlock();
					}
				}
			}
		}
		// neighbour of cur's neighbour
	}
	bigcs.Unlock();
}
#endif