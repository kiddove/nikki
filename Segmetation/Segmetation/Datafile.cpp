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

	//// use multithreads
	////int iThreadCount = get_core_count() * 2;
	//int iThreadCount = 2;

	//HANDLE * pHandle = new HANDLE[iThreadCount];
	//threadParam* pThreadParam = new threadParam[iThreadCount];

	//int iCount = (int)width * height;
	//int iBlock = iCount / iThreadCount;
	//for (int i = 0; i < iThreadCount; i++)
	//{
	//	DWORD dwThreadID = 0;
	//	pThreadParam[i].pParam = (InterfaceData*)this;
	//	pThreadParam[i].start = i * iBlock;
	//	pThreadParam[i].end = (i + 1) * iBlock;

	//	if (i == iThreadCount - 1)
	//		pThreadParam[i].end = iCount;
	//	pHandle[i] = CreateThread(NULL, 0, ThreadProc_ProcessAll, &pThreadParam[i], 0, &dwThreadID);
	//	threadIDs.push_back(dwThreadID);
	//	threads.push_back(pHandle[i]);
	//}

	//::WaitForMultipleObjects(iThreadCount, pHandle, TRUE, INFINITE);

	//for (int i= 0; i < iThreadCount; i++)
	//	::CloseHandle(pHandle[i]);
	//delete[] pHandle;
	//delete[] pThreadParam;


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
			Segment* pSeg1 = FindMatch(segment[iIndex]);
			if (pSeg1 != NULL && pSeg1->m_iIndex != -1)
			{
				Segment* pSeg2 = FindMatch(pSeg1);

				if (pSeg2 != NULL && pSeg2->m_iIndex != -1)
				{
					if (segment[iIndex] == pSeg2)
					{
						// merge
						MergeSegment(segment[iIndex], pSeg1);
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
	while (true)
	{
		bool bMatch = false;
		for (int i = 0; i < (int)segment.size(); i++)
		{
			if (segment[i] == NULL || segment[i]->m_iIndex == -1)
				continue;
			Segment* pSeg1 = FindMatch(segment[i]);
			if (pSeg1 != NULL && pSeg1->m_iIndex != -1)
			{
				Segment* pSeg2 = FindMatch(pSeg1);

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

	invalidSegment.m_iIndex = -1;
	invalidSegment.max_col = invalidSegment.max_row = invalidSegment.min_col = invalidSegment.min_row = 0;
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
			}
		}
	}
}

template <class T>
float ImgData<T>::CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	if (pSeg1->m_iIndex == -1 || pSeg2->m_iIndex == -1)
		return 1e8;
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
float ImgData<T>::CalcNewCriteria(Segment* pSeg1, Segment* pSeg2)
{
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
	float ed = f / (float)pSeg1->m_Avg.size();
	return ed;
}

template <class T>
Segment* ImgData<T>::FindMatch(Segment* pSeg)
{
	//::WaitForSingleObject(g_event_find, INFINITE);
	//::ResetEvent(g_event_merge);
	
	if (pSeg != NULL && pSeg->m_iIndex != -1)
	{
		float fEd_min = 1e8;
		int index = -1;

		//::EnterCriticalSection(&pSeg->cs);
		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			if (pSeg->m_neighbours[j] == NULL)
				continue;
			// calc
			//<dynamic_cast>
			Segment* p = (Segment*)(pSeg->m_neighbours[j]);
			//::EnterCriticalSection(&p->cs);
			if (p == NULL || p->m_iIndex == -1)
			{
				//::LeaveCriticalSection(&p->cs);
				continue;
			}

			// to change CalcEuclideanDistance to whatever you want
			float fValue = CalcEuclideanDistance(pSeg, p);
			//::LeaveCriticalSection(&p->cs);

			//float fValue = CalcNewCriteria(pSeg, p);
			// find min
			if (fValue < fEd_min && fValue < condition.threshold)
			{
				fEd_min = fValue;
				index = p->m_iIndex;
			}
		}
		if (index >= 0)
		{
			//TRACE(_T("%d match %d\n"), pSeg->m_iIndex, index);
			//::SetEvent(g_event_merge);
			//::LeaveCriticalSection(&g_cs_merge);
			//::LeaveCriticalSection(&pSeg->cs);
			return segment[index];
		}
		//::SetEvent(g_event_merge);
		//::LeaveCriticalSection(&g_cs_merge);
		//::LeaveCriticalSection(&pSeg->cs);
	}
	//::LeaveCriticalSection(&g_cs);
	//::SetEvent(g_event_merge);
	
	return NULL;
}

template <class T>
void ImgData<T>::MergeSegment(Segment* pSeg1, Segment* pSeg2)
{
	//::WaitForSingleObject(g_event_merge, INFINITE);
	//::ResetEvent(g_event_find);
	//::EnterCriticalSection(&g_cs);
	//::EnterCriticalSection(&pSeg1->cs);
	//::EnterCriticalSection(&pSeg2->cs);
	if (pSeg1->m_iIndex == -1 || pSeg2->m_iIndex == -1)
	{
		//::SetEvent(g_event_find);
		//::LeaveCriticalSection(&g_cs);
		//::LeaveCriticalSection(&pSeg1->cs);
		//::LeaveCriticalSection(&pSeg2->cs);
		return;
	}
	

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
	pSeg2->m_area.clear();

	// update neighbours

	// remove seg2 from seg1's neighbours
	// put seg2 neighbours to seg1 (except seg1)
	// change seg2 neighbours to seg1 

	std::vector<int>::iterator iter = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), (int)pSeg2);
	if (iter != pSeg1->m_neighbours.end())
		pSeg1->m_neighbours.erase(iter);

	iter = std::find(pSeg2->m_neighbours.begin(), pSeg2->m_neighbours.end(), (int)pSeg1);
	if (iter != pSeg2->m_neighbours.end())
		pSeg2->m_neighbours.erase(iter);

	for (int i = 0; i < (int)pSeg2->m_neighbours.size(); i++)
	{
		std::vector<int>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), pSeg2->m_neighbours[i]);
		
		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(pSeg2->m_neighbours[i]);

		Segment* pSeg = (Segment*)(pSeg2->m_neighbours[i]);
		if (pSeg == NULL || pSeg->m_iIndex == -1)
			continue;

		// 如果这里面已经有seg1了。。 就直接把seg2删掉
		// if seq1 is seq's neighbour then delete seq2 from seq's neighbour
		iter1 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), (int)pSeg1);
		std::vector<int>::iterator iter2 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), (int)pSeg2);

		ASSERT(iter2 != pSeg->m_neighbours.end());
		if (iter2 == pSeg->m_neighbours.end())
		{
			::OutputDebugString(_T("wtf\n"));
		}

		if (iter1 != pSeg->m_neighbours.end())
			pSeg->m_neighbours.erase(iter2);
		else
			*iter2 = (int)pSeg1;
	}

	

	pSeg1->min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	pSeg1->min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	pSeg1->max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	pSeg1->max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
	//TRACE(_T("merge %d to %d\n"), pSeg2->m_iIndex, pSeg1->m_iIndex);
	segment[pSeg2->m_iIndex] = NULL;
	delete pSeg2;
	pSeg2 = NULL;

	// for multithread NULL
	//memcpy(pSeg2, &invalidSegment, sizeof(Segment));
	//pSeg2->Clear();

	//::SetEvent(g_event_find);
	//::LeaveCriticalSection(&g_cs);

	//::LeaveCriticalSection(&pSeg1->cs);
	//::LeaveCriticalSection(&pSeg2->cs);
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
void ImgData<T>::Output(CString& strFile)
{
	std::vector<std::vector<int>> vecResult;
	std::vector<int> vecRow;
	vecRow.assign(width, 0);
	vecResult.assign(height, vecRow);

	int iIndex = 0;
	for (int i = 0; i < (int)segment.size(); i++)
	{
		if (segment[i] != NULL && segment[i]->m_iIndex != -1)
		{
			iIndex++;
			for (int j = 0; j < (int)segment[i]->m_area.size(); j++)
			{
				//vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = segment[i]->m_iIndex;
				vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = iIndex;
			}
		}
	}

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

	// sort is too slow
	int iSize = vecArea.size();
	//// sort 
	//std::sort(vecArea.begin(), vecArea.end(), lessRow);
	//int x = (vecArea[iSize - 1].row - vecArea[0].row + 1);

	//std::sort(vecArea.begin(), vecArea.end(), lessCol);
	//int y = (vecArea[iSize - 1].col - vecArea[0].col + 1);

	int min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	int min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	int max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	int max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
	
	sf = (float)(iSize) / (float)((max_row - min_row + 1) * (max_col - min_col + 1));
	return sf;
}

template <class T>
void ImgData<T>::ThreadProcessAll(int& start, int& end)
{
	int iRound = 0;
	while (true)
	{
		TRACE(_T("Round: %d of [%d, %d]\n"), ++iRound, start, end);
		bool bMatch = false;
		for (int i = start; i < end; i++)
		{
			if (segment[i] == NULL || segment[i]->m_iIndex == -1)
				continue;
			Segment* pSeg1 = FindMatch(segment[i]);
			if (pSeg1 != NULL && pSeg1->m_iIndex != -1)
			{
				Segment* pSeg2 = FindMatch(pSeg1);

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
void ImgData<T>::ThreadProcessviaArea(int& start, int& end)
{
}