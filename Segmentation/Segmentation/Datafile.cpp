#include "StdAfx.h"
#include "Datafile.h"

//#define THREAD_COUNT 9
#define ROW_BLOCK	10
#define COL_BLOCK	10
#define THREAD_COUNT 4
int g_count = 0;
int g_time = 0;

//bool lessRow(const Pixel& p1, const Pixel& p2)
//{
//	if (p1.row < p2.row)
//		return true;
//	return false;
//}
//
//bool lessCol(const Pixel& p1, const Pixel& p2)
//{
//	if (p1.col < p2.col)
//		return true;
//	return false;
//}

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
//DWORD WINAPI ThreadProcAll(LPVOID pParam)
//{
//	try
//	{
//		threadParam* p = (threadParam*)pParam;
//		InterfaceData* pData = p->pParam;
//		pData->ThreadProcessAll(p->row_start, p->row_end, p->col_start, p->col_end);
//		return 0x1001;
//	}
//	catch (std::exception e)
//	{
//		TRACE(_T("%s\n"), e.what());
//		return 0x2001;
//	}
//}

DWORD WINAPI ThreadProc_ProcessFind(LPVOID pParam)
{
	try
	{
		threadParam* p = (threadParam*)pParam;
		InterfaceData* pData = p->pParam;
		pData->ThreadFind();
		return 0x1001;
	}
	catch (std::exception e)
	{
		TRACE(_T("%s\n"), e.what());
		return 0x2001;
	}
}

DWORD WINAPI ThreadProc_ProcessMerge(LPVOID pParam)
{
	try
	{
		threadParam* p = (threadParam*)pParam;
		InterfaceData* pData = p->pParam;
		pData->ThreadMerge();
		return 0x1002;
	}
	catch (std::exception e)
	{
		TRACE(_T("%s\n"), e.what());
		return 0x2002;
	}
}
#endif

Datafile::Datafile(void)
{
	m_strFile.Empty();

	m_pDataset = NULL;
	GDALAllRegister();
	m_bResize = false;
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
	m_Image.CloseFile();
}

bool Datafile::OpenFile(LPCTSTR strFilePath)
{
	//m_Mat = imread(strFilePath, -1);
	m_pDataset = (GDALDataset*)GDALOpen(strFilePath, GA_ReadOnly);

	if (m_pDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFilePath);
		AfxMessageBox(strErr);
		//f.Close();
		return false;
	}

	//ImageReader ir;
	//ir.LoadFile(strFilePath);
	//ir.Statistic();
	m_Image.LoadFile(strFilePath);
	//m_Image.Statistic();
	if (m_Image.m_data.data != NULL)
	{
		//m_imgData.Mat2Vec(ir.m_data);
		if (m_bResize)
		{
			cv::Size sz;
			sz.width = m_Image.m_data.cols / 4;
			sz.height = m_Image.m_data.rows / 4;
			cv::resize(m_Image.m_data, m_MatResize, sz, CV_INTER_CUBIC);
			m_imgData.CopyMat(m_MatResize);

			// test resize back
			//cv::Mat mt1;
			//sz.width = m_Image.m_data.cols;
			//sz.height = m_Image.m_data.rows;
			//cv::resize(m_MatResize, mt1, sz, CV_INTER_CUBIC);

			//cv::Mat mt2;
			//mt1.convertTo(mt2, mt1.type(), 1/255.0);
			//imshow("resize back", mt2);
			//::waitKey(0);
		}
		else
			m_imgData.CopyMat(m_Image.m_data);
		m_strFile = strFilePath;
		m_Image.CloseFile();
	}
	else
		return false;
	return true;
}

CString Datafile::GetSuffix(segCondition& con, bool bTxt)
{
	/*
	Euclidean_Distance,
	Old_BSMSE,
	New_BSMSE,
	Old_BSMSE_New_Equation,
	New_BSMSE_New_Equation,
	*/
	CString strSuffix = _T("[");
	switch (con.calcType)
	{
	case Euclidean_Distance:
		strSuffix += _T("ED");
		break;
	case Old_BSMSE:	//BSMSE
		strSuffix += _T("BSSE");
		break;
	case New_BSMSE:	//BSMSE
		strSuffix += _T("New_BSMSE");
		break;
	case Old_BSMSE_New_Equation:	//BSMSE
		strSuffix += _T("Old_BSMSE_New_Equation");
		break;
	case New_BSMSE_New_Equation:	//BSMSE
		strSuffix += _T("New_BSMSE_New_Equation");
		break;
	case Similarity_Measure:
		strSuffix += _T("Similarity_Measure");
		break;
	case Standard_Deviation:
		strSuffix += _T("SD");
		break;
	case Entropy_Measure:
		strSuffix += _T("ENT");
		break;
	case Root_Squared_Error:
		strSuffix += _T("RSE");
		break;
	case Rooted_Difference_in_Variance:
		strSuffix += _T("RDV");
		break;
	case Rooted_Difference_in_Squared_Error:
		strSuffix += _T("RDSE");
		break;
	case Spectral_Angle_Mapper:
		strSuffix += _T("SAM");
		break;
	default:
		break;
	}
	//con.neibType; con.threshold; con.min_size(if > 0); con.alpha(if necessary), con.beta(if necessary);
	switch (con.neibType)
	{
	case Four_Neighbour:
		strSuffix += _T("_4");
		break;
	case Eight_Neighbour:
		strSuffix += _T("_8");
		break;
	default:
		break;
	}
	
	CString str;
	str.Format(_T("_%.2f"), con.threshold_pre);
	strSuffix += str;
	str.Format(_T("_%.2f"), con.threshold);
	strSuffix += str;

	str.Format(_T("_%d"), con.iteration);
	strSuffix += str;

	if (con.min_size > 1)
	{
		str.Format(_T("_%d"), con.min_size);
		strSuffix += str;
	}

	//if (con.calcType == Similarity_Measure)
	//{
	//	str.Format(_T("_%.2f_%.2f"), con.alpha, con.beta);
	//	strSuffix += str;
	//}

	if (bTxt)
		strSuffix += _T("].txt");
	else
		strSuffix += _T("].tif");

	return strSuffix;
}

void Datafile::Process(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
{
	int iRasterCount = m_pDataset->GetRasterCount();
	if (iRasterCount < 1)
		return;

	// fetch a raster band to get data type

	GDALRasterBand *poBand = m_pDataset->GetRasterBand(1);
	GDALDataType iType = poBand->GetRasterDataType();

	CString strTemp;
	int ipos = m_strFile.ReverseFind(_T('.'));
	strTemp = m_strFile.Left(ipos);
	if (ipos == -1)
		strTemp = m_strFile;
#ifdef USETXT
	CString strOutput;
	strOutput.Format(_T("%s%s"), strTemp, GetSuffix(con, true));
#else
	CString strOutput;
	strOutput.Format(_T("%s%s"), strTemp, GetSuffix(con));
#endif
	con.scale = true;

	m_imgData.Process(vecSeed, con, mapMask, traceinfo);
	m_imgData.Output(strOutput, m_pDataset);

	//if (m_bResize)
	//{
	//	cv::Size sz;
	//	sz.height = m_Image.m_data.rows;
	//	sz.width = m_Image.m_data.cols;
	//	m_MatResize = m_imgData.result_data.resize();
	//}
//	switch (iType)
//	{
//	case GDT_UInt16:
//		{
//			ImgData<unsigned short> usData;
//			// load data
//			usData.LoadData(m_pDataset);
//			usData.Process(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			usData.Output(strOutput);
//#else
//			usData.Output(strOutput, m_pDataset);
//#endif
//			//usData.UnInitialize();
//			//return usData;
//		}
//		break;
//	case GDT_Int16:
//		{
//			ImgData<short> sData;
//			// load data
//			sData.LoadData(m_pDataset);
//			sData.Process(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			sData.Output(strOutput);
//#else
//			sData.Output(strOutput, m_pDataset);
//#endif
//			//return sData;
//		}
//		break;
//	case GDT_Float32:
//		{
//			ImgData<float> fData;
//			// load data
//			fData.LoadData(m_pDataset);
//			fData.Process(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			fData.Output(strOutput);
//#else
//			fData.Output(strOutput, m_pDataset);
//#endif
//			//return sData;
//		}
//		break;
//	case GDT_Byte:
//	default:
//		{
//			//con.scale = false;
//			ImgData<byte> bData;
//			// load data
//			bData.LoadData(m_pDataset);
//			bData.Process(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			bData.Output(strOutput);
//#else
//			bData.Output(strOutput, m_pDataset);
//#endif
//			//return bData;
//		}
//		break;
//	}
}

///// ImgData
//template <class T>
//void ImgData<T>::LoadData(Mat& mat)
//{
//	//data = mat;
//	width = data.rows;
//	height = data.cols;
//}

//void ImgData<T>::LoadBand(GDALRasterBand* pBand)
//{
//	char *pafScanline;
//	width = pBand->GetXSize();		//width,colcount
//	height = pBand->GetYSize();		//height,rowcownt
//	GDALDataType iType = pBand->GetRasterDataType();
//
//	int iCount = width * height * sizeof(T);
//	pafScanline = (char*)CPLMalloc(iCount);
//	pBand->RasterIO(GF_Read, 0, 0, width, height, pafScanline, width, height, iType, 0, 0);
//
//	T* pData = (T*)pafScanline;
//	std::vector<std::vector<T>> vecBand;
//
//	for (int i = 0; i < height; i++)
//	{
//		std::vector<T> vecRow;
//		for (int j = 0; j < width; j++)
//		{
//			vecRow.push_back(pData[i * width + j]);
//		}
//		vecBand.push_back(vecRow);
//	}
//	data.push_back(vecBand);
//	CPLFree(pafScanline);
//}
//
//template <class T>
//// load image data into vector
//void ImgData<T>::LoadData(GDALDataset* pDataset)
//{
//	data.clear();
//	std::vector<std::vector<std::vector<T>>>().swap(data);
//	// band count
//	int iRasterCount = pDataset->GetRasterCount();
//	for (int i = 1; i <= iRasterCount; i++)
//	{
//		GDALRasterBand* poBand = pDataset->GetRasterBand(i);
//		if (poBand != NULL)
//			LoadBand(poBand);
//	}
//}

//template <class T>
//void ImgData<T>::Process(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
//{
//	int i1 = ::GetTickCount();
//	SetTraceInfo(traceinfo);
//	// segmentation
//	SetCondition(con);
//
//	Initialize();
//
//	InitNeighbours();
//
//	//ProcessviaSeed(vecSeed);
//
//	
////#ifdef ONLYSEED
////	return;
////
////#endif
//	ProcessviaMask(mapMask);
//#ifdef MULTI_THREAD
//	int is = ::GetTickCount();
//	//Divide();
//	//ProcessBoundary();
//	//SetBoundary();
//	ProcessviaMultiThread();
//	//ClearBoundary();
//	int ie = ::GetTickCount();
//	CString ss;
//	ss.Format(_T("MultiThread cost time : %d(ms)\n"), ie - is);
//	::OutputDebugString(ss);
//	ProcessAllSegment();
//#else
//	ProcessAllSegment();
//#endif
//	int i2 = ::GetTickCount();
//	CString str;
//	str.Format(_T("total time : %d(s)\n"), (i2 - i1) / 1000);
//	::OutputDebugString(str);
////	ProcessviaArea();
//}

//template <class T>
void ImgData::Process(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
{
	int i1 = ::GetTickCount();
	
	if (!bPreprocess)
	{
		UnInitialize();
		//traceinfo.bTrace = true;
		//traceinfo.col = 12;
		//traceinfo.row = 0;
		SetTraceInfo(traceinfo);
		// segmentation
		SetCondition(con);

		Initialize();

		InitNeighbours();

		ProcessviaMask(mapMask);

		ProcessPre();
	}

	SetTraceInfo(traceinfo);
	// segmentation
	SetCondition(con);
#ifdef MULTI_THREAD
	int is = ::GetTickCount();
	//Divide();
	//ProcessBoundary();
	//SetBoundary();
	ProcessviaMultiThread();
	//ClearBoundary();
	int ie = ::GetTickCount();
	CString ss;
	ss.Format(_T("MultiThread cost time : %d(ms)\n"), ie - is);
	::OutputDebugString(ss);
	ProcessAllSegment();
#else
	// if did not choose for process at first 
	if ( ptFilter== NULL)
	{
		switch (condition.calcType)
		{
		case Euclidean_Distance:
			ptFilter = new EuclideanDistance(condition);
			break;
		case Old_BSMSE:
			ptFilter = new OldBSMSE(condition);
			break;
		case New_BSMSE:
			ptFilter = new NewBSMSE(condition);
			break;
		case Old_BSMSE_New_Equation:
			ptFilter = new OldBSMSE_NewEquation(condition);
			break;
		case New_BSMSE_New_Equation:
			ptFilter = new NewBSMSE_NewEquation(condition);
			break;
			//case Similarity_Measure:
			//	ptFilter = new SimilarityMeasure(condition);
			//	break;
		case Standard_Deviation:
			ptFilter = new StandardDeviation(condition);
			break;
		case Spectral_Angle_Mapper:
			ptFilter = new SAM(condition);
			break;
		case Entropy_Measure:
			ptFilter = new ENT(condition);
			break;
		case Root_Squared_Error:
			ptFilter = new RSE(condition);
			break;
		case Rooted_Difference_in_Variance:
			ptFilter = new RDV(condition);
			break;
		case Rooted_Difference_in_Squared_Error:
			ptFilter = new RDSE(condition);
			break;
		default:
			ptFilter = NULL;
			break;
		}
		ASSERT(ptFilter != NULL);
	}
	ProcessviaSeed(vecSeed);
	ProcessAllSegment();
#endif
	int i2 = ::GetTickCount();
	CString str;
	str.Format(_T("total time : %d(s)\n"), (i2 - i1) / 1000);
	::OutputDebugString(str);
//	ProcessviaArea();
}

void ImgData::ProcessviaSeed(std::vector<cv::Point>& vecSeed)
{
#ifdef ONLYSEED
	seedPixel.clear();
	seedPixel = vecSeed;
#endif
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
			//ASSERT(vecSeed[i].row < height && vecSeed[i].col < width);
			//if (vecSeed[i].row >= height || vecSeed[i].col >= width)
			if (vecSeed[i].y >= height || vecSeed[i].x >= width)
				continue;
			//int iIndex = (height - 1 - vecSeed[i].row) * width + vecSeed[i].col;
			//int iIndex = (height - 1 - vecSeed[i].y) * width + vecSeed[i].x;
			int iIndex = vecSeed[i].y * width + vecSeed[i].x;
			if (segment[iIndex] == NULL || !segment[iIndex]->m_bValid)
				continue;
			int iIndex1 = FindMatch(iIndex);
			if (iIndex1 != -1)
			{

				// merge
				MergeSegment(iIndex, iIndex1);
				bMatch = true;

				//int iIndex2 = FindMatch(iIndex1, iIndex);
				//if (iIndex2 != -1)
				//{
				//	if (iIndex == iIndex2)
				//	{
				//		// merge
				//		MergeSegment(iIndex, iIndex1);
				//		bMatch = true;
				//	}
				//}
			}
		}

		if (!bMatch)
			break;
	}
}
#ifdef MULTI_THREAD
//template <class T>
//void ImgData<T>::ProcessBoundary()
//{
//	int iRound = 0;
//	while (true)
//	{
//		int i1 = ::GetTickCount();
//		bool bMatch = false;
//		for (int i = 0; i < (int)vecBoundary.size(); i++)
//		{
//			ASSERT(vecBoundary[i].row < height && vecBoundary[i].col < width);
//			int iIndex = vecBoundary[i].row * width + vecBoundary[i].col;
//			if (segment[iIndex] == NULL || segment[iIndex]->m_bBoundary)
//				continue;
//			int iIndex1 = FindMatch(iIndex);
//			if (iIndex1 != -1)
//			{
//				int iIndex2 = FindMatch(iIndex1, iIndex);
//
//				if (iIndex2 != -1)
//				{
//					if (iIndex == iIndex2)
//					{
//						// merge
//						MergeSegment(iIndex, iIndex1);
//						bMatch = true;
//					}
//				}
//			}
//		}
//
//		int i2 = ::GetTickCount();
//		TRACE(_T("ProcessBoundary Round %d cost %d(ms)\n"), iRound++, i2 - i1);
//		if (!bMatch)
//			break;
//	}
//}

void ImgData::ProcessviaMultiThread()
{
	// use multithreads to find, main thread to merge
	int iThreadCount = THREAD_COUNT;
	//int iThreadCount = ROW_BLOCK * COL_BLOCK;

	HANDLE * pHandle = new HANDLE[iThreadCount + 1];
	threadParam* pThreadParam = new threadParam[iThreadCount + 1];

	int iCount = 0;
	int iWidth = width / COL_BLOCK;
	int iHeight = height / ROW_BLOCK;
	for (int i = 0; i < ROW_BLOCK; i++)
	{
		for (int j = 0; j < COL_BLOCK; j++)
		{
			Blockpair bp;
			bp.row_start = i * iHeight;
			bp.row_end =  (i + 1) * iHeight >= height ? height : (i + 1) * iHeight;
			bp.col_start = j * iWidth;
			bp.col_end = (j + 1) * iWidth >= width ? width : (j + 1) * iWidth; 
			queBlock.push(bp);
			//TRACE(_T("block(%d, %d) -- [(%d, %d), (%d, %d)]\n"), i, j, bp.row_start, bp.row_end, bp.col_start, bp.col_end);
		}
	}
	g_time = ::GetTickCount();
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

			pHandle[iCount] = CreateThread(NULL, 0, ThreadProc_ProcessFind, &pThreadParam[iCount], 0, &dwThreadID);
			//::WaitForMultipleObjects(1, &pHandle[iCount], TRUE, INFINITE);
			mapFindEvent.insert(std::make_pair(dwThreadID, pFindEventArray[iCount]));
			iCount++;
		//}

	}

	DWORD dwMergeID = 0;
	// merge thread
	pThreadParam[iCount].pParam = (InterfaceData*)this;
	pHandle[iCount] = CreateThread(NULL, 0, ThreadProc_ProcessMerge, &pThreadParam[iCount], 0, &dwMergeID);
	::WaitForMultipleObjects(iThreadCount + 1, pHandle, TRUE, INFINITE);

	for (int i= 0; i < iThreadCount; i++)
		::CloseHandle(pHandle[i]);
	delete[] pHandle;
	delete[] pThreadParam;
}
#endif

void ImgData::ProcessAllSegment()
{
	int iTime = 0;
	while (true)
	{
		int i1 = ::GetTickCount();
		//std::vector<MergePair> vecTest;
		int iMerge = 0;
		bool bMatch = false;
#ifdef REVERSE
		for (int i = width * height - 1; i >= 0; i--)
#else
		for (int i = 0; i < width * height; i++)
#endif
		{
			if (segment[i] == NULL || !segment[i]->m_bValid)
				continue;

			int iIndex1 = FindMatch(segment[i]->m_iIndex);
			if (iIndex1 != -1)
			{
				int iIndex2 = FindMatch(iIndex1);
				if (segment[i]->m_iIndex == tInfo.idx)
					TRACE(_T("segment(%d) bestfit is %d, segment(%d) bestfit is %d\n"), tInfo.idx, iIndex1, iIndex1, iIndex2);
				if (iIndex2 != -1)
				{
					if (segment[i]->m_iIndex == iIndex2)
					{
						// merge
						//TRACE(_T("Loop %d : %d(i), %d(iIndex1), %d(iIndex2), %.2f(Coefficient)\n"), iTime, i, segment[i]->m_iIndex, iIndex1, ptFilter->CalcCoefficient(segment[i], segment[iIndex1]));
						MergeSegment(segment[i]->m_iIndex, iIndex1);
						//MergePair mp;
						//mp.m = segment[i]->m_iIndex;
						//mp.n = iIndex1;
						//vecTest.push_back(mp);
						iMerge++;
						bMatch = true;
					}
				}
			}
		}

		int i2 = ::GetTickCount();
		//CString str;
		//str.Format(_T("%d loop , find cost time %d, %d merged\n"), iTime, i2 - i1, iMerge);
		//str.Format(_T("%d loop, cost time %d.\n"), iTime, i2 - i1);
		//::OutputDebugString(str);

		if (!bMatch)
			break;

		////int iCost = 0;
		//////TRACE(_T("Merge Cout : %d\n"), vecTest.size());
		//for (int k = 0; k < (int)vecTest.size(); k++)
		//{
		//	//int is = ::GetTickCount();
		//	MergeSegment(vecTest[k].m, vecTest[k].n);
		//	//int ie = ::GetTickCount();
		//	//iCost += (ie - is);
		//}

		//int countFalse = 0;
		//for (int i = 0; i < height; i++)
		//{
		//	for (int j = 0; j < width; j++)
		//	{
		//		if (segment[i * width + j]->m_bValid == false)
		//			countFalse++;
		//	}
		//}
		/////
		//CString ss;
		//ss.Format(_T("Merge %d. %d false\n"), vecTest.size(), countFalse);
		//::OutputDebugString(ss);
		
		//int i3 = ::GetTickCount();
		////CString str;
		//str.Format(_T("%d loop , merge(%d) cost time %d, avg time : %.3f, total time: %d \n"), iTime, vecTest.size(), i3 - i2, (double)iCost / (double)vecTest.size(), i3 - i1);
		//::OutputDebugString(str);
		++iTime;
		//vecTest.clear();
		//std::vector<MergePair>().swap(vecTest);
	}
}

//bool ImgData::Initialize()
//{
//	if (tInfo.bTrace)
//		CLog2File::GetInstance()->CreateLogFile();
//	segment = new Segment*[width * height];
//	// each pixel is assigned as a distinct segment
//	if (!data.empty())
//	{
//		int iNum = 0;
//		// row
//		for (int r = 0; r < (int)data[0].size(); r++)
//		{
//			// col
//			for (int c = 0; c < (int)data[0][r].size(); c++)
//			{
//				Segment* pSeg = new Segment(r, c);
//			
//				for (int k = 0; k < (int)data.size(); k++)
//				{
//					// Avg
//					pSeg->m_Avg.push_back(data[k][r][c]);
//					// Variance
//					//pSeg->m_Variance.push_back(0.0f);
//					pSeg->m_StandardDeviation.push_back(0.0f);
//				}
//				pSeg->m_Entropy = 0.0;
//				pSeg->m_iIndex = iNum++;
//				//pSeg->min_row = pSeg->max_row = r;
//				//pSeg->min_col = pSeg->max_col = c;
//				segment[pSeg->m_iIndex] = pSeg;
//
//				if (tInfo.bTrace && tInfo.row == r && tInfo.col == c)
//				{
//					CString strInfo;
//					strInfo.Format(_T("\nsegmentation start image size width[%d] x height[%d]\n\n\n"), width, height);
//					CLog2File::GetInstance()->WriteLog(strInfo);
//					
//					strInfo.Format(_T("Original segment ID : %d[%d, %d]\n"), (r * width + c), r, c);
//					CLog2File::GetInstance()->WriteLog(strInfo);
//					for (int k = 0; k < (int)data.size(); k++)
//					{
//						strInfo.Format(_T("Original mean value[%d] : %.3f\n"), k, pSeg->m_Avg[k]);
//						CLog2File::GetInstance()->WriteLog(strInfo);
//					}
//				}
//			}
//		}
//	}
//#ifdef MULTI_THREAD
//	pFindEventArray = new HANDLE[THREAD_COUNT];
//	for (int i = 0; i < THREAD_COUNT; i++)
//	{
//		pFindEventArray[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//	}
//
//	mergeHandle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//	bQuitFind = false;
//	bForceMerge = false;
//#endif
//	CalcType oldType = condition.calcType;
//	//condition.calcType = Euclidean_Distance;
//	//ptFilterED = new EuclideanDistance(condition);
//	condition.calcType = Spectral_Angle_Mapper;
//	ptFilterED = new SAM(condition);
//
//	condition.calcType = oldType;
//	/*
//	Euclidean_Distance,
//	Old_BSMSE,
//	New_BSMSE,
//	Old_BSMSE_New_Equation,
//	New_BSMSE_New_Equation,
//	Similarity_Measure
//	*/
//	switch (condition.calcType)
//	{
//	case Euclidean_Distance:
//		ptFilter = new EuclideanDistance(condition);
//		break;
//	case Old_BSMSE:
//		ptFilter = new OldBSMSE(condition);
//		break;
//	case New_BSMSE:
//		ptFilter = new NewBSMSE(condition);
//		break;
//	case Old_BSMSE_New_Equation:
//		ptFilter = new OldBSMSE_NewEquation(condition);
//		break;
//	case New_BSMSE_New_Equation:
//		ptFilter = new NewBSMSE_NewEquation(condition);
//		break;
//	//case Similarity_Measure:
//	//	ptFilter = new SimilarityMeasure(condition);
//	//	break;
//	case Standard_Deviation:
//		ptFilter = new StandardDeviation(condition);
//		break;
//	case Spectral_Angle_Mapper:
//		ptFilter = new SAM(condition);
//		break;
//	case Entropy_Measure:
//		ptFilter = new ENT(condition);
//		break;
//	default:
//		ptFilter = NULL;
//		break;
//	}
//	ASSERT(ptFilter != NULL);
//
//	if (condition.calcType == Entropy_Measure)
//	{
//		InitializeForEntropy();
//	}
//	return true;
//}
/*
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
float ImgData<T>::CalcEuclideanDistance(Segment* pSeg1, Segment* pSeg2)
{
	// pSeg1 && pSeg2 must be valid before enter
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}

	float ed = 1e8;
	if (f > 0.0001)
		ed = sqrt(f);

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
	float ed = sqrt(f / (float)pSeg1->m_Avg.size());
	return ed;
}
*/

int ImgData::FindMatch(int iIndex, int except)
{
	if (iIndex != -1)
	{
		Segment* pSeg = segment[iIndex];
		float fEd_min = 1e8;
		int iFind = -1;

		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			int iIndexNeighbour = pSeg->m_neighbours[j];

			Segment* p = segment[iIndexNeighbour];
			if (p == NULL || !p->m_bValid)
			{
				continue;
			}
			float fValue = ptFilter->CalcCoefficient(pSeg, p);
			// find min
#ifdef MULTI_THREAD
			if (bForceMerge)
			{
				if (fValue < fEd_min)
				{
					fEd_min = fValue;
					iFind = p->m_iIndex;
				}
			}
			else
			{
#endif
				if (fValue < fEd_min && fValue < condition.threshold)
				{
					fEd_min = fValue;
					iFind = p->m_iIndex;
				}
				//else if (fValue > condition.threshold)

				//{
				//	::OutputDebugString(_T("wtf\n"));
				//}
				//else if (fValue > condition.threshold)
				//{
				//	TRACE(_T("Not reasonable~!\n"));
				//}

				if (pSeg->m_iIndex == tInfo.idx || p->m_iIndex == tInfo.idx)
				{
					TRACE(_T("FindMatch[segment(%d), neighbour(%d), Coefficient(%.2f), threshold(%.2f), bestfit(%d, %.2f)\n"), iIndex, p->m_iIndex, fValue, condition.threshold, iFind, fEd_min);
				}
#ifdef MULTI_THREAD
			}
#endif
		}

		if (iFind >= 0)
		{
			return iFind;
		}
		else
			return -1;
	}

	return -1;
}

void ImgData::MergeSegment(int iIndex1, int iIndex2)
{
	// segment[iIndex1], segment[iIndex2] locked before enter this function
	if (iIndex1 == -1 || iIndex2 == -1 || iIndex1 == iIndex2)
		return;

	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];


	if (pSeg1 == NULL || pSeg2 ==NULL || !pSeg1->m_bValid || !pSeg2->m_bValid)
		return;

	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	std::vector<float> a1 = pSeg1->m_Avg;
	std::vector<float> a2 = pSeg2->m_Avg;

	//std::vector<float> v1 = pSeg1->m_Variance;
	//std::vector<float> v2 = pSeg2->m_Variance;

	ASSERT(a1.size() == a2.size());
	//ASSERT(v1.size() == v2.size());

	bool bTrace = false;
	if (tInfo.bTrace && (tInfo.idx == iIndex1 || tInfo.idx == iIndex2))
	{
		CLog2File::GetInstance()->WriteLog(_T("------------------------------\n"));
		CString strInfo;
		strInfo.Format(_T("%d will be merged into %d\n"), iIndex2, iIndex1);
		CLog2File::GetInstance()->WriteLog(strInfo);
		bTrace = true;

		strInfo.Format(_T("segment[%d] and segment[%d] method[%s] value : %.3f\n"), iIndex1, iIndex2, ptFilter->GetName(), ptFilter->CalcCoefficient(pSeg1, pSeg2));
		CLog2File::GetInstance()->WriteLog(strInfo);
	}
	///////////////////////////////////////
	if (bTrace)
	{
		CString strInfo;
		strInfo.Format(_T("befor merge %d into %d\n"), iIndex2, iIndex1);
		CLog2File::GetInstance()->WriteLog(strInfo);
		strInfo.Format(_T("segment[%d] contains %d pixels\n"), iIndex1, n1);
		CLog2File::GetInstance()->WriteLog(strInfo);

		for (int i = 0; i < (int)a1.size(); i++)
		{
			strInfo.Format(_T("band[%d] mean value:%.3f\n"), i, pSeg1->m_Avg[i]);
			CLog2File::GetInstance()->WriteLog(strInfo);
		}

		strInfo.Format(_T("segment[%d] contains %d pixels\n"), iIndex2, n2);
		CLog2File::GetInstance()->WriteLog(strInfo);
		for (int i = 0; i < (int)a1.size(); i++)
		{
			strInfo.Format(_T("band[%d] mean value:%.3f\n"), i, pSeg2->m_Avg[i]);
			CLog2File::GetInstance()->WriteLog(strInfo);
		}
	}

	// for each band
	float fEntropy = 0.0;
	for (int i = 0; i < (int)a1.size(); i++)
	{
		int iCount = 0;
		float fEpy = 0.0;
		float avg1 = a1[i];
		float avg2 = a2[i];
		//float var1 = v1[i];
		//float var2 = v2[i];
		float avg = 0.0;
		avg = (avg1 * n1 + avg2 * n2) / (float)(n1 + n2);
		
		//pSeg1->m_Variance[i] = (n1 * var1 + n2 * var2) / (n1 + n2) + (n1 * avg1 * avg1 + n2 * avg2 * avg2) / (n1 + n2) - avg * avg;
		//v1[i] = (n1 * v1 + n2 * v2) / (n1 + n2) + (n1 * a1 * a1 + n2 * a2 * a2) / (n1 + n2) - pSeg1->m_fAvg * pSeg1->m_fAvg;
		float standarddeviation = (n1 * pSeg1->m_StandardDeviation[i] * pSeg1->m_StandardDeviation[i] + n2 * pSeg2->m_StandardDeviation[i] * pSeg2->m_StandardDeviation[i]) / (float)(n1 + n2)
			+ (n1 * pSeg1->m_Avg[i] * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i] * pSeg2->m_Avg[i]) / (float)(n1 + n2) - avg * avg;
//		ASSERT(standarddeviation > 0.0001);

		standarddeviation = sqrt(standarddeviation);
		pSeg1->m_StandardDeviation[i] = standarddeviation;
		pSeg1->m_Avg[i] = avg;
		
		// ENT
		if (condition.calcType == Entropy_Measure)
		{
			for (int b = 0; b < (int)pSeg1->m_vecBrightness[i].size(); b++)
			{
				int i1 = pSeg1->m_vecBrightness[i][b];
				int i2 = pSeg2->m_vecBrightness[i][b];
				int v = i1 + i2;
				if (i1 + i2 > 0)
				{
					float p = (float)v / (float)(n1 + n2);
					fEpy +=  (float)(p * (log(p) / log(2))); 
					if (bTrace)
					{
						CString strInfo;
						strInfo.Format(_T("band[%d], brightness[%d], segment[%d](n1) = %d, segment[%d](n2) = %d, p value:%.3f, ENT=%.3f\n"), i, b, iIndex1, i1, iIndex2, i2, p, -1.0 * p * (log(p) / log(2)));
						CLog2File::GetInstance()->WriteLog(strInfo);
					}
				}
				iCount += v;
				pSeg1->m_vecBrightness[i][b] = v;
			}
			if (iCount != n1 + n2)
				::OutputDebugString(_T("wwwww\n"));
		}
		
		fEntropy += fEpy;
		if (bTrace)
		{
			CString strInfo;
			strInfo.Format(_T("segment[%d] band[%d] mean value:%.3f, Entropy: %.3f\n"), iIndex1, i, avg, fEpy);
			CLog2File::GetInstance()->WriteLog(strInfo);
		}
	}
	fEntropy *= -1.0;
	fEntropy /= a1.size();
	pSeg1->m_Entropy = (n1 + n2) * fEntropy - n1 * pSeg1->m_Entropy - n2 * pSeg2->m_Entropy;

	/*pSeg1->min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
	pSeg1->min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;

	pSeg1->max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
	pSeg1->max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;*/
	//TRACE(_T("merge %d to %d\n"), pSeg2->m_iIndex, pSeg1->m_iIndex);
	//TRACE(_T("%d\n"), iIndex2);
	//segment[iIndex2] = NULL;

	MergeNeighbours(iIndex1, iIndex2);

	//remove seg2 -- set invalid and erase all data
	SetInvalid(iIndex2);
	
	//delete pSeg2;
	//pSeg2 = NULL;
	//pSeg2->m_bValid = false;
	g_count++;
	///////////////////////////////////////
	ASSERT(pSeg1->m_area.size() == n1 + n2);
	if (bTrace)
	{
		CString strInfo;
		strInfo.Format(_T("after merge %d into %d, segment[%d] contains %d pixels, segment[%d] Entropy : %.3f\n"), iIndex2, iIndex1, iIndex1, pSeg1->m_area.size(), iIndex1, pSeg1->m_Entropy);
		CLog2File::GetInstance()->WriteLog(strInfo);
	}

	if (tInfo.bTrace && tInfo.idx == iIndex2)
	{
		tInfo.idx = iIndex1;
	}
	//g_map_merge.insert(iIndex2);
}

//template <class T>
ImgData::~ImgData()
{
	UnInitialize();
}

//template <class T>
void ImgData::UnInitialize()
{
#ifdef MULTI_THREAD
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		//pFindEventArray[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL):
		::CloseHandle(pFindEventArray[i]);
	}
	delete[] pFindEventArray;
	::CloseHandle(mergeHandle);
#endif
	if (ptFilter != NULL)
	{
		delete ptFilter;
		ptFilter = NULL;
	}

	if (ptFilter_Pre != NULL)
	{
		delete ptFilter_Pre;
		ptFilter_Pre = NULL;
	}

	if (segment != NULL)
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
}

void ImgData::InitNeighbours()
{
	if (condition.neibType == Four_Neighbour)
		InitNeighbours_4();
	else if (condition.neibType == Eight_Neighbour)
		InitNeighbours_8();
}

void ImgData::InitNeighbours_4()
{
	// [r, c - 1], [r, c + 1], [r - 1, c], [r + 1, c]
	for (int i = 0; i < width * height; i++)
	{
		//int r = segment[i]->m_area[0].row;
		//int c = segment[i]->m_area[0].col;
		int r = segment[i]->m_area[0].y;
		int c = segment[i]->m_area[0].x;

		ASSERT(segment[i]->m_iIndex == r * width + c);
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
	}
}

void ImgData::InitNeighbours_8()
{
	for (int i = 0; i < width * height; i++)
	{
		//int r = segment[i]->m_area[0].row;
		//int c = segment[i]->m_area[0].col;
		int r = segment[i]->m_area[0].y;
		int c = segment[i]->m_area[0].x;
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


//template <class T>
//void ImgData<T>::InitNeighbours_8()
//{
//	for (int i = 0; i < width * height; i++)
//	{
//		//int r = segment[i]->m_area[0].row;
//		//int c = segment[i]->m_area[0].col;
//		int r = segment[i]->m_area[0].y;
//		int c = segment[i]->m_area[0].x;
//		ASSERT(segment[i]->m_iIndex == r * width + c);
//
//		
//		
//		
//		
//		
//		
//		
//		
//		// r+1, c+1
//		if (r < height - 1 && c < width - 1)
//		{
//			int iIndex = (r + 1) * width + c + 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r+1, c
//		if (r < height - 1)
//		{
//			int iIndex = (r + 1) * width + c;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r+1, c-1
//		if (r < height - 1 && c >= 1)
//		{
//			int iIndex = (r + 1) * width + c - 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r, c+1
//		if (c < width - 1)
//		{
//			int iIndex = r * width + c + 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r, c-1
//		if (c >= 1)
//		{
//			int iIndex = r * width + c - 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r-1, c+1
//		if (r >= 1 && c < width - 1)
//		{
//			int iIndex = (r - 1) * width + c + 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r-1, c
//		if (r >= 1)
//		{
//			int iIndex = (r - 1) * width + c;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//		// r-1, c-1
//		if (r >= 1 && c >= 1)
//		{
//			int iIndex = (r - 1) * width + c - 1;
//			//Neighbour nb;
//			//nb.iIndex = iIndex;
//			//nb.pSeg = segment[iIndex];
//			//segment[i]->m_neighbours.push_back(nb);
//			segment[i]->m_neighbours.push_back(iIndex);
//		}
//	}
//}


#ifdef USETXT
void ImgData::Output(CString& strFile)
{
//	return;
	std::vector<std::vector<int>> vecResult;
	std::vector<int> vecRow;
	vecRow.assign(width, 0);
	vecResult.assign(height, vecRow);

#ifndef ONLYSEED
	int iSum = 0;
	int iIndex = 0;
	for (int i = 0; i < width * height; i++)
	{
		if (segment[i]->m_bValid)
		{
			iIndex++;
			iSum += segment[i]->m_area.size();
			for (int j = 0; j < (int)segment[i]->m_area.size(); j++)
			{
				//vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = segment[i]->m_iIndex;
				vecResult[segment[i]->m_area[j].y][segment[i]->m_area[j].x] = iIndex;
			}
		}
	}
	ASSERT(iSum == width * height);
#else
	// only seed
	int iNumber = 0;
	for (int i = 0; i < (int)seedPixel.size(); i++)
	{
		//int iIndex = seedPixel[i].row * width + seedPixel[i].col;
		//int iIndex = (height - 1 - seedPixel[i].y) * width + seedPixel[i].x;
		int iIndex = seedPixel[i].y * width + seedPixel[i].x;
		if (segment[iIndex] != NULL && segment[iIndex]->m_bValid)
		{
			iNumber++;
			for (int j = 0; j < (int)segment[iIndex]->m_area.size(); j++)
			{
				//vecResult[segment[i]->m_area[j].row][segment[i]->m_area[j].col] = segment[i]->m_iIndex;
				vecResult[segment[iIndex]->m_area[j].y][segment[iIndex]->m_area[j].x] = iNumber;
			}
		}
	}
#endif
	
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
#else
//template <class T>
//void ImgData<T>::Output(CString& strFile, GDALDataset* pDataset)
//{
//	const char* pszFormat = _T("GTiff");
//	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
//	char** papszMetadata;
//
//	if (poDriver == NULL)
//	{
//		AfxMessageBox(_T("Output:Can not get GDAL Driver~!."));
//		return;
//	}
//
//	papszMetadata = poDriver->GetMetadata();
//	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
//		TRACE( _T("Driver %s supports Create() method.\n"), pszFormat );
//	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
//		TRACE( _T("Driver %s supports CreateCopy() method.\n"), pszFormat );
//
//	// create file
//	GDALDataset* poDstDS;
//	char** papszOptions = NULL;
//	//poDstDS = poDriver->Create(strFile.GetBuffer(1024), width, height, 1, GDT_UInt16, papszMetadata);
//	poDstDS = poDriver->Create(strFile.GetBuffer(1024), width, height, 1, GDT_Float32, papszMetadata);
//	strFile.ReleaseBuffer();
//
//	double adfGeoTransform[6];
//	if (pDataset->GetGeoTransform(adfGeoTransform) == CE_None)
//	{
//		poDstDS->SetGeoTransform(adfGeoTransform);
//	}
//	//else
//	//{
//	//	AfxMessageBox(_T("Output:Can not GetGeoTransform."));
//	//	GDALClose( (GDALDatasetH) poDstDS );
//	//	return;
//	//}
//	const char* pszProjection = pDataset->GetProjectionRef();
//	poDstDS->SetProjection(pszProjection);
//	
//	GDALRasterBand* poBand = poDstDS->GetRasterBand(1);
//	if (poBand == NULL)
//	{
//		AfxMessageBox(_T("Output:Can not get RasterBand."));
//		GDALClose( (GDALDatasetH) poDstDS );
//		return;
//	}
//
//	//unsigned short* pData = new unsigned short[width * height];
//	float* pData = new float[width * height];
//#ifndef ONLYSEED
//	int iNumber = 0;
//	for (int i = 0; i < width * height; i++)
//	{
//		if (segment[i] != NULL && segment[i]->m_bValid)
//		{
//			iNumber++;
//			int iA = segment[i]->m_area.size();
//			for (int j = 0; j < (int)segment[i]->m_area.size(); j++)
//			{
//				//int iIndex = segment[i]->m_area[j].row * width + segment[i]->m_area[j].col;
//				int iIndex = segment[i]->m_area[j].y * width + segment[i]->m_area[j].x;
//				//pData[iIndex] = iNumber;
//				pData[iIndex] = segment[i]->m_Avg[0];
//			}
//		}
//	}
//#else
//	// to be continued...
//#endif
//
//	char* pszData = (char*)pData;
//	poBand->RasterIO(GF_Write, 0, 0, width, height, pszData, width, height, GDT_UInt16, 0, 0);
//	/* Once we're done, close properly the dataset */
//	GDALClose( (GDALDatasetH) poDstDS );
//	delete pData;
//
//	// use opencv to show image
//
//	Mat ret = imread(strFile.GetBuffer(1024), CV_LOAD_IMAGE_UNCHANGED);
//	strFile.ReleaseBuffer();
//
//	if (ret.data != NULL)
//	{
//		imshow("result", ret);
//	}
//}

void ImgData::Output(CString& strFile, GDALDataset* pDataset)
{
	int iBand = pDataset->GetRasterCount();
	cv::Mat result;
	const char* pszFormat = _T("GTiff");
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	char** papszMetadata;

	if (poDriver == NULL)
	{
		AfxMessageBox(_T("Output:Can not get GDAL Driver~!."));
		return;
	}

	papszMetadata = poDriver->GetMetadata();
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
		TRACE( _T("Driver %s supports Create() method.\n"), pszFormat );
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
		TRACE( _T("Driver %s supports CreateCopy() method.\n"), pszFormat );

	// create file
	GDALDataset* poDstDS;
	char** papszOptions = NULL;
	//poDstDS = poDriver->Create(strFile.GetBuffer(1024), width, height, 1, GDT_UInt16, papszMetadata);
	poDstDS = poDriver->Create(strFile.GetBuffer(1024), width, height, 1 + iBand, GDT_Float32, papszMetadata);
	strFile.ReleaseBuffer();

	double adfGeoTransform[6];
	if (pDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		poDstDS->SetGeoTransform(adfGeoTransform);
	}
	//else
	//{
	//	AfxMessageBox(_T("Output:Can not GetGeoTransform."));
	//	GDALClose( (GDALDatasetH) poDstDS );
	//	return;
	//}
	const char* pszProjection = pDataset->GetProjectionRef();
	poDstDS->SetProjection(pszProjection);
	
	for (int k = 1; k <= iBand + 1; k++)
	{
		GDALRasterBand* poBand = poDstDS->GetRasterBand(k);
		if (poBand == NULL)
		{
			AfxMessageBox(_T("Output:Can not get RasterBand."));
			GDALClose( (GDALDatasetH) poDstDS );
			return;
		}

		//unsigned short* pData = new unsigned short[width * height];
		float* pData = new float[width * height];

		int iNumber = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (segment[i] != NULL && segment[i]->m_bValid)
			{
				iNumber++;
				int iA = segment[i]->m_area.size();
				for (int j = 0; j < (int)segment[i]->m_area.size(); j++)
				{
					//int iIndex = segment[i]->m_area[j].row * width + segment[i]->m_area[j].col;
					int iIndex = segment[i]->m_area[j].y * width + segment[i]->m_area[j].x;
					//pData[iIndex] = iNumber;
					if (k == iBand + 1)
						pData[iIndex] = (float)iNumber;
					else
						pData[iIndex] = segment[i]->m_Avg[k-1];
				}
			}
		}


		char* pszData = (char*)pData;
		poBand->RasterIO(GF_Write, 0, 0, width, height, pszData, width, height, GDT_Float32, 0, 0);
		delete pData;
	}

	/* Once we're done, close properly the dataset */
	GDALClose( (GDALDatasetH) poDstDS );
	

	// use opencv to show image

	Mat ret = imread(strFile.GetBuffer(1024), CV_LOAD_IMAGE_UNCHANGED);
	strFile.ReleaseBuffer();

	if (ret.data != NULL)
	{
		imshow("result", ret);
	}
	result_data = ret.clone();
}
#endif

//template <class T>
//Segment* ImgData<T>::FindMatchviaArea(Segment* pSeg)
//{
//	if (pSeg != NULL && pSeg->m_iIndex != -1)
//	{
//		float fEd_min = 1e8;
//		int index = -1;
//		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
//		{
//			// calc
//			Segment* p = pSeg->m_neighbours[j].pSeg;
//			if (p == NULL || p->m_bBoundary)
//				continue;
//
//			// to change CalcEuclideanDistance to whatever you want
//			//float fValue = CalcEuclideanDistance(pSeg, p);
//
//			float fValue = CalcShapefactor(pSeg, p);
//			// find min
//			if (fValue < fEd_min && fValue < condition.shapefactor)
//			{
//				fEd_min = fValue;
//				index = p->m_iIndex;
//			}
//		}
//		if (index >= 0)
//		{
//			//TRACE(_T("%d match %d\n"), pSeg->m_iIndex, index);
//			return segment[index];
//		}
//	}
//
//	return NULL;
//}
//
//template <class T>
//float ImgData<T>::CalcShapefactor(Segment* pSeg1, Segment* pSeg2)
//{
//	float sf = 1e8;
//
//	std::vector<Pixel> vecArea;
//	vecArea.insert(vecArea.end(), pSeg1->m_area.begin(), pSeg1->m_area.end());
//	vecArea.insert(vecArea.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
//
//	if (vecArea.empty())
//		return sf;
//
//	int iSize = vecArea.size();
//
//	int min_row = pSeg1->min_row < pSeg2->min_row ? pSeg1->min_row : pSeg2->min_row;
//	int min_col = pSeg1->min_col < pSeg2->min_col ? pSeg1->min_col : pSeg2->min_col;
//
//	int max_row = pSeg1->max_row > pSeg2->max_row ? pSeg1->max_row : pSeg2->max_row;
//	int max_col = pSeg1->max_col > pSeg2->max_col ? pSeg1->max_col : pSeg2->max_col;
//	
//	sf = (float)(iSize) / (float)((max_row - min_row + 1) * (max_col - min_col + 1));
//	return sf;
//}
#ifdef MULTI_THREAD
/*
//template <class T>
//void ImgData<T>::ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end)
//{
//	//GetFromQue(row_start, row_end, col_start, col_end);
//
//	//SYSTEM_INFO systeminfo;
//	//::GetSystemInfo(&systeminfo);
//	//CString strInfo;
//	//strInfo.Format(_T("CPU total count %d. Thread %d is using num %d(%x)\n"), systeminfo.dwNumberOfProcessors, ::GetCurrentThreadId(), systeminfo.dwActiveProcessorMask, systeminfo.dwActiveProcessorMask);
//	//::OutputDebugString(strInfo);
//
//	//int iCpu = 1;
//	//for (int i = 0; i < row_start; i++)
//	//{
//	//	iCpu = iCpu << 1;
//	//}
//	//DWORD dMask = ::SetThreadAffinityMask(::GetCurrentThread(), iCpu);
//
//	//::GetSystemInfo(&systeminfo);
//	//strInfo.Format(_T("After SetThreadAffinityMask. CPU total count %d. Thread %d is using num %d(%x), preMask is %d(%x)\n"), systeminfo.dwNumberOfProcessors, ::GetCurrentThreadId(), systeminfo.dwActiveProcessorMask, systeminfo.dwActiveProcessorMask, dMask, dMask);
//	//::OutputDebugString(strInfo);
//	while (true)
//	{
//		queLock.Lock();
//		if (queBlock.empty())
//		{
//			queLock.Unlock();
//			break;
//		}
//		Blockpair bp = queBlock.front();
//		queBlock.pop();
//		queLock.Unlock();
//		int i = bp.i;
//		int j = bp.j;
//
//		row_start = i == 0 ? i * rInterval: i * rInterval + 1;
//		row_end = (i == ROW_BLOCK - 1) ? height: (i + 1) * rInterval;
//
//		col_start = j == 0 ? j * cInterval: j * cInterval + 1;
//		col_end = (j == COL_BLOCK - 1) ? width: (j + 1) * cInterval;
//
//		CString str;
//		str.Format(_T("Process of [(%d, %d), (%d, %d)] begins~!!\n"), row_start, row_end, col_start, col_end);
//		::OutputDebugString(str);
//
//		int i1 = ::GetTickCount();
//		Segment** pSegBlock = RetriveBlock(row_start, row_end, col_start, col_end);
//
//		// calc with segBlock
//		ProcessBlock(pSegBlock, row_start, row_end, col_start, col_end);
//
//		RestoreBlock(pSegBlock, row_start, row_end, col_start, col_end);
//
//		DestroyBlock(pSegBlock, row_start, row_end, col_start, col_end);
//		int i2 = ::GetTickCount();
//
//		str.Format(_T("Process of [(%d, %d), (%d, %d)] cost time %d(ms)\n"), row_start, row_end, col_start, col_end, i2 - i1);
//		::OutputDebugString(str);
//	}
//	//// [start, end)
//	//int iRound = 0;
//	//while (true)
//	//{
//	//	int i1 = ::GetTickCount();
//	//	bool bMatch = false;
//	//	for (int i = row_start; i < row_end; i++)
//	//	{
//	//		for (int j = col_start; j < col_end; j++)
//	//		{
//	//			int iIndex = i * width + j;
//	//			if (segment[iIndex] == NULL || segment[iIndex]->m_bBoundary)
//	//			{
//	//				continue;
//	//			}
//
//	//			int iIndex1 = FindMatch(iIndex);
//
//	//			if (iIndex1 != -1)
//	//			{
//	//				int iIndex2 = FindMatch(iIndex1, iIndex);
//	//				if (iIndex2 != -1 && iIndex == iIndex2)
//	//				{
//	//					MergeSegment(iIndex, iIndex1);
//	//					bMatch = true;
//	//				}
//	//			}
//	//		}
//	//	}
//
//	//	if (!bMatch)
//	//	{
//	//		CString str;
//	//		str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] exit!!!!\n"), ++iRound, row_start, row_end, col_start, col_end);
//	//		::OutputDebugString(str);
//	//		break;
//	//	}
//
//	//	int i2 = ::GetTickCount();
//	//	CString str;
//	//	str.Format(_T("Round: %d of [(%d, %d), (%d, %d)] cost time %d(ms)\n"), ++iRound, row_start, row_end, col_start, col_end, i2 - i1);
//	//	::OutputDebugString(str);
//	//}
//}

template <class T>
void ImgData<T>::ThreadProcessAll(int& row_start, int& row_end, int& col_start, int& col_end)
{

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
	//delete pSeg2;
	//pSeg2 = NULL;
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
*/
void ImgData::ThreadFind()
{
	while (!bQuitFind)
	{
		queLock.Lock();
		if (queBlock.empty())
		{
			queLock.Unlock();
			std::map<DWORD, HANDLE>::iterator iter = mapFindEvent.find(::GetCurrentThreadId());
			ASSERT(iter != mapFindEvent.end());
			//break;
			//CString ss;
			//ss.Format(_T("Thread : %d now wait for mergehandle.\n"), ::GetCurrentThreadId());
			//::OutputDebugString(ss);
			::SetEvent(iter->second);
			::WaitForSingleObject(mergeHandle, INFINITE);
			//ss.Format(_T("Thread : %d now continues to work.\n"), ::GetCurrentThreadId());
			//::OutputDebugString(ss);
			continue;
		}
		Blockpair bp = queBlock.front();
		queBlock.pop();
		queLock.Unlock();

		int iWidth = bp.col_end - bp.col_start;
		int iHeight = bp.row_end - bp.row_start;
		// find this block
		//TRACE(_T("Process:[(%d, %d), (%d, %d)]\n"), bp.row_start, bp.row_end, bp.col_start, bp.col_end);
		for (int i = bp.row_start; i < bp.row_end; i++)
		{
			for (int j = bp.col_start; j < bp.col_end; j++)
			{
				int iIndex = i * width + j;
				if (segment[iIndex] == NULL || !segment[iIndex]->m_bValid)
					continue;
				if (!bForceMerge)
				{
					int iIndex1 = FindMatch(iIndex);
					if (iIndex1 != -1)
					{
						// may cause dead lock here
						int iIndex2 = FindMatch(iIndex1, iIndex);

						if (iIndex2 != -1 && iIndex == iIndex2)
						{
							//MergeSegment(iIndex, iIndex1);
							vecMergeLock.Lock();
							MergePair mp;
							mp.m = iIndex;
							mp.n = iIndex1;
							//queMerge
							vecMerge.push_back(mp);
							vecMergeLock.Unlock();
						}
					}
				}
				else
				{
					if (segment[iIndex]->m_area.size() < (UINT)condition.min_size)
					{
						int iIndex1 = FindMatch(iIndex);
						if (iIndex1 != -1)
						{
							vecMergeLock.Lock();
							MergePair mp;
							mp.m = iIndex;
							mp.n = iIndex1;
							//queMerge
							vecMerge.push_back(mp);
							vecMergeLock.Unlock();
						}
					}
				}
			}
		}

		// set event
		// wait for mergeevent
	}
}

void ImgData::ThreadMerge()
{
	int iTemp = 0;
	while (true)
	{
		::WaitForMultipleObjects(THREAD_COUNT, pFindEventArray, TRUE, INFINITE);
		//vecMergeLock.Lock();
		::ResetEvent(mergeHandle);
		if (!bForceMerge)
		{
			if (vecMerge.empty())
			{
				bQuitFind = true;
				::SetEvent(mergeHandle);
				break;
			}
			iTemp = g_time;
			g_time = ::GetTickCount();
			TRACE(_T("find cost time %d(ms).\n"), g_time - iTemp);

			//if (!bForceMerge && (int)vecMerge.size() < (int)(width * height / 5000))
			//	bForceMerge = true;
		}
		else
		{
			if (vecMerge.empty())
				bForceMerge = false;
		}
		// 
		for (int i = 0; i < (int)vecMerge.size(); i++)
		{
			MergeSegment(vecMerge[i].m, vecMerge[i].n);
		}


		int iWidth = width / COL_BLOCK;
		int iHeight = height / ROW_BLOCK;
		queLock.Lock();
		for (int i = 0; i < ROW_BLOCK; i++)
		{
			for (int j = 0; j < COL_BLOCK; j++)
			{
				Blockpair bp;
				bp.row_start = i * iHeight;
				bp.row_end =  (i + 1) * iHeight >= height ? height : (i + 1) * iHeight;
				bp.col_start = j * iWidth;
				bp.col_end = (j + 1) * iWidth >= width ? width : (j + 1) * iWidth; 
				queBlock.push(bp);
				//TRACE(_T("block(%d, %d) -- [(%d, %d), (%d, %d)]"), i, j, bp.row_start, bp.row_end, bp.col_start, bp.col_end);
			}
		}
		queLock.Unlock();

		///
		//int countFalse = 0;
		//for (int i = 0; i < height; i++)
		//{
		//	for (int j = 0; j < width; j++)
		//	{
		//		if (segment[i * width + j]->m_bValid == false)
		//			countFalse++;
		//	}
		//}
		///
		CString ss;
		ss.Format(_T("Merge %d. %d false\n"), vecMerge.size(), g_count);
		::OutputDebugString(ss);
		
		vecMerge.clear();
		std::vector<MergePair>().swap(vecMerge);
		iTemp = g_time;
		g_time = ::GetTickCount();
		ss.Format(_T("merge cost : %d(ms).\n"), g_time - iTemp);
		::OutputDebugString(ss);
		for (int i = 0; i < THREAD_COUNT; i++)
		{
			::ResetEvent(pFindEventArray[i]);
		}
		::SetEvent(mergeHandle);
	}
}
#endif

// merge seg2's neighbour into seg1
void ImgData::MergeNeighbours(int iIndex1, int iIndex2)
{
	if (iIndex1 == -1 || iIndex2 == -1)
		return;

	Segment* pSeg1 = segment[iIndex1];
	Segment* pSeg2 = segment[iIndex2];


	if (pSeg1 == NULL || pSeg2 ==NULL || !pSeg1->m_bValid || !pSeg2->m_bValid)
		return;

	pSeg1->m_area.insert(pSeg1->m_area.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
	//pSeg2->m_area.clear();

	// update neighbours

	// remove seg2 from seg1's neighbours
	// put seg2 neighbours to seg1 (except seg1)
	// change seg2 neighbours to seg1 

	int n11 = pSeg1->m_neighbours.size();
	int n22 = pSeg2->m_neighbours.size();


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
		if (pSeg == NULL || !pSeg->m_bValid)
			continue;
		// if n is seg1's neighbour
		std::vector<int>::iterator iter1 = std::find(pSeg1->m_neighbours.begin(), pSeg1->m_neighbours.end(), iIndex);

		if (iter1 == pSeg1->m_neighbours.end())
			pSeg1->m_neighbours.push_back(iIndex);


		// check n's neighbour if has seg2 then change to seg1
		iter1 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex1);
		std::vector<int>::iterator iter2 = std::find(pSeg->m_neighbours.begin(), pSeg->m_neighbours.end(), iIndex2);

		if (iter1 != pSeg->m_neighbours.end())
			pSeg->m_neighbours.erase(iter2);
		else
		{
			*iter2 = iIndex1;
		}

	}
}

void ImgData::SetInvalid(int iIndex)
{
	segment[iIndex]->m_bValid = false;
	segment[iIndex]->m_area.clear();
	std::vector<cv::Point>().swap(segment[iIndex]->m_area);
	segment[iIndex]->m_neighbours.clear();
	std::vector<int>().swap(segment[iIndex]->m_neighbours);

	segment[iIndex]->m_vecBrightness.clear();
	std::vector<std::vector<int>>().swap(segment[iIndex]->m_vecBrightness);

	//segment[iIndex]->m_Boundary.clear();
	//std::vector<cv::Point>().swap(segment[iIndex]->m_Boundary);
}

void ImgData::ProcessviaMask(std::map<int, set<int>>& mapMask)
{
	if (tInfo.bTrace)
	{
		CString strInfo;
		strInfo.Format(_T("Process mask file begins.\n"));
		CLog2File::GetInstance()->WriteLog(strInfo);
	}

	for (std::map<int, set<int>>::iterator iter = mapMask.begin(); iter != mapMask.end(); iter++)
	{
		int iSize = (int)iter->second.size();
		if (iSize > 1)
		{
			int iID = iter->first;
			set<int>::iterator iterSet = iter->second.begin();
			int iIndex1 = *iterSet;
			++iterSet;
			for (; iterSet != iter->second.end(); iterSet++)
			{
				int iIndex2 = *iterSet;
				MergeSegment(iIndex1, iIndex2);
			}

			//TRACE(_T("%d area %d\n"), iID, iSize);
		}
	}

	if (tInfo.bTrace)
	{
		CString strInfo;
		strInfo.Format(_T("Process mask file ends.\n"));
		CLog2File::GetInstance()->WriteLog(strInfo);
	}
}

void ImgData::ProcessPre()
{
	for (int k = 0; k < condition.iteration; k++)
	{
#ifdef REVERSE
		for (int i = width * height - 1; i >= 0; i--)
#else
		for (int i = 0; i < width * height; i++)
#endif
		{
			if (segment[i] == NULL || !segment[i]->m_bValid)
				continue;

			int iIndex1 = FindMatchPre(segment[i]->m_iIndex);
			if (iIndex1 != -1)
			{
				int iIndex2 = FindMatchPre(iIndex1);

				if (iIndex2 != -1)
				{
					if (segment[i]->m_iIndex == iIndex2)
					{
						// merge
						//TRACE(_T("Loop %d : %d(i), %d(iIndex1), %d(iIndex2), %.2f(Coefficient)\n"), iTime, i, segment[i]->m_iIndex, iIndex1, ptFilter->CalcCoefficient(segment[i], segment[iIndex1]));
						MergeSegment(segment[i]->m_iIndex, iIndex1);
					}
				}
			}
		}
	}
}


int ImgData::FindMatchPre(int iIndex, int except)
{
	if (iIndex != -1)
	{
		Segment* pSeg = segment[iIndex];
		float fEd_min = 1e8;
		int iFind = -1;

		for (int j = 0; j < (int)pSeg->m_neighbours.size(); j++)
		{
			int iIndexNeighbour = pSeg->m_neighbours[j];

			Segment* p = segment[iIndexNeighbour];
			if (p == NULL || !p->m_bValid)
			{
				continue;
			}
			float fValue = ptFilter_Pre->CalcCoefficient(pSeg, p);
			// find min
#ifdef MULTI_THREAD
			if (bForceMerge)
			{
				if (fValue < fEd_min)
				{
					fEd_min = fValue;
					iFind = p->m_iIndex;
				}
			}
			else
			{
#endif
				if (fValue < fEd_min && fValue < condition.threshold_pre)
				{
					fEd_min = fValue;
					iFind = p->m_iIndex;
				}
				//else if (fValue > condition.threshold)
				//{
				//	TRACE(_T("Not reasonable~!\n"));
				//}
				if (pSeg->m_iIndex == tInfo.idx || p->m_iIndex == tInfo.idx)
				{
					TRACE(_T("FindMatchPre[segment(%d), neighbour(%d), Coefficient(%.2f), threshold(%.2f), bestfit(%d, %.2f)\n"), iIndex, p->m_iIndex, fValue, condition.threshold_pre, iFind, fEd_min);
				}
#ifdef MULTI_THREAD
			}
#endif
		}

		if (iFind >= 0)
		{
			return iFind;
		}
		else
			return -1;
	}

	return -1;
}

void Datafile::PreProcess(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
{
	int iRasterCount = m_pDataset->GetRasterCount();
	if (iRasterCount < 1)
		return;

	// fetch a raster band to get data type

	GDALRasterBand *poBand = m_pDataset->GetRasterBand(1);
	GDALDataType iType = poBand->GetRasterDataType();

	CString strTemp;
	int ipos = m_strFile.ReverseFind(_T('.'));
	strTemp = m_strFile.Left(ipos);
	if (ipos == -1)
		strTemp = m_strFile;
#ifdef USETXT
	CString strOutput;
	strOutput.Format(_T("%s%s"), strTemp, GetSuffix_Pre(con, true));
#else
	CString strOutput;
	strOutput.Format(_T("%s%s"), strTemp, GetSuffix_Pre(con));
#endif
	//m_imgData.Init(con, mapMask, traceinfo);
	m_imgData.PreProcess(vecSeed, con, mapMask, traceinfo);
	m_imgData.Output(strOutput, m_pDataset);
//	switch (iType)
//	{
//	case GDT_UInt16:
//		{
//			ImgData<unsigned short> usData;
//			// load data
//			usData.LoadData(m_pDataset);
//			usData.PreProcess(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			usData.Output(strOutput);
//#else
//			usData.Output(strOutput, m_pDataset);
//#endif
//			//usData.UnInitialize();
//			//return usData;
//		}
//		break;
//	case GDT_Int16:
//		{
//			ImgData<short> sData;
//			// load data
//			sData.LoadData(m_pDataset);
//			sData.PreProcess(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			sData.Output(strOutput);
//#else
//			sData.Output(strOutput, m_pDataset);
//#endif
//			//return sData;
//		}
//		break;
//	case GDT_Float32:
//		{
//			ImgData<float> fData;
//			// load data
//			fData.LoadData(m_pDataset);
//			fData.PreProcess(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			fData.Output(strOutput);
//#else
//			fData.Output(strOutput, m_pDataset);
//#endif
//			//return sData;
//		}
//		break;
//	case GDT_Byte:
//	default:
//		{
//			ImgData<byte> bData;
//			// load data
//			bData.LoadData(m_pDataset);
//			bData.PreProcess(vecSeed, con, mapMask, traceinfo);
//#ifdef USETXT
//			bData.Output(strOutput);
//#else
//			bData.Output(strOutput, m_pDataset);
//#endif
//			//return bData;
//		}
//		break;
//	}
}


void ImgData::PreProcess(std::vector<cv::Point>& vecSeed, segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
{
	//traceinfo.bTrace = true;
	////traceinfo.row = 303;
	////traceinfo.col = 393; 
	//traceinfo.row = 300;
	//traceinfo.col = 397; 
	////traceinfo.row = 294;
	////traceinfo.col = 390; 
	UnInitialize();
	SetTraceInfo(traceinfo);
	// segmentation
	SetCondition(con);


	//if (condition.resize)
	//{
	//	cv::Mat mtResize;
	//	//data.convertTo(mtResize, data.type(), 1/255.0);
	//	cv::Size sz;
	//	sz.width = width / 4;
	//	sz.height = height / 4;
	//	cv::resize(data, mtResize, sz, CV_INTER_CUBIC);

	//	//cv::Mat mtRz;
	//	//mtResize.convertTo(mtRz, mtResize.type(), 1/ 255.0);
	//	//imshow(_T("resize"), mtRz);
	//	//::waitKey(0);
	//}
	Initialize();

	InitNeighbours();

	ProcessviaMask(mapMask);

	//ProcessviaSeed(vecSeed);
	ProcessPre();
	bPreprocess = true;
}

CString Datafile::GetSuffix_Pre(segCondition& con, bool bTxt)
{
	/*
	Euclidean_Distance,
	Old_BSMSE,
	New_BSMSE,
	Old_BSMSE_New_Equation,
	New_BSMSE_New_Equation,
	*/
	//CString strSuffix = _T("[Euclidean_Distance");
	CString strSuffix = _T("Pre_[");
	switch (con.calcType)
	{
	case Euclidean_Distance:
		strSuffix += _T("ED");
		break;
	case Old_BSMSE:	//BSMSE
		strSuffix += _T("BSSE");
		break;
	case New_BSMSE:	//BSMSE
		strSuffix += _T("New_BSMSE");
		break;
	case Old_BSMSE_New_Equation:	//BSMSE
		strSuffix += _T("Old_BSMSE_New_Equation");
		break;
	case New_BSMSE_New_Equation:	//BSMSE
		strSuffix += _T("New_BSMSE_New_Equation");
		break;
	case Similarity_Measure:
		strSuffix += _T("Similarity_Measure");
		break;
	case Standard_Deviation:
		strSuffix += _T("SD");
		break;
	case Entropy_Measure:
		strSuffix += _T("ENT");
		break;
	case Root_Squared_Error:
		strSuffix += _T("RSE");
		break;
	case Rooted_Difference_in_Variance:
		strSuffix += _T("RDV");
		break;
	case Rooted_Difference_in_Squared_Error:
		strSuffix += _T("RDSE");
		break;
	case Spectral_Angle_Mapper:
		strSuffix += _T("SAM");
		break;
	default:
		break;
	}

	switch (con.neibType)
	{
	case Four_Neighbour:
		strSuffix += _T("_4");
		break;
	case Eight_Neighbour:
		strSuffix += _T("_8");
		break;
	default:
		break;
	}
	
	CString str;
	str.Format(_T("_%.2f"), con.threshold_pre);
	strSuffix += str;

	str.Format(_T("_%d"), con.iteration);
	strSuffix += str;

	if (bTxt)
		strSuffix += _T("].txt");
	else
		strSuffix += _T("].tif");

	return strSuffix;
}


void ImgData::InitializeForEntropy()
{
	// scale if needed
	for (int k = 0; k < channel; k++)
	{
		int iMin = (int)1e8;
		int iMax = (int)-1e8;
		for (int r = 0; r < height; r++)
		{
			for (int c = 0; c < width; c++)
			{
				int iIndex = r * width + c;
				Segment* pSeg = segment[iIndex];
				int iValue = (int)pSeg->m_Avg[k];
				if (iValue < iMin)
					iMin = iValue;
				if (iValue > iMax)
					iMax = iValue;
			}
		}
		vecMin.push_back(iMin);
		vecMax.push_back(iMax);
	}

	if (condition.scale)
	{
		std::vector<float> vecScale;
		vecScale.assign(channel, 0);
		for (int k = 0; k < channel; k++)
		{
			vecScale[k] = (float)(vecMax[k] - vecMin[k] + 1.0) / (float)BRIGHTNESS;
		}
		for (int r = 0; r < height; r++)
		{
			// col
			for (int c = 0; c < width; c++)
			{
				int iIndex = r * width + c;
				Segment* pSeg = segment[iIndex];

				for (int k = 0; k < channel; k++)
				{
					// ENT
					std::vector<int> vecTemp;
					vecTemp.assign(BRIGHTNESS, 0);
					pSeg->m_vecBrightness.push_back(vecTemp);
					// scale if needed
					int p = (int)((GetValue(r, c, k) - vecMin[k]) / vecScale[k]);
					//CString str;
					//str.Format(_T("data[%d], scale data[%d], scale[%.3f]\n"), data[k][r][c], p, vecScale[k]);
					//::OutputDebugString(str);
					ASSERT(p < BRIGHTNESS && p > -1);
					pSeg->m_vecBrightness[k][p] += 1;
					if (pSeg->m_vecBrightness[k][p] > 1)
						::OutputDebugString(_T("really?\n"));
				}
			}
		}
	}
	else
	{
		for (int r = 0; r < height; r++)
		{
			// col
			for (int c = 0; c < width; c++)
			{
				int iIndex = r * width + c;
				Segment* pSeg = segment[iIndex];

				for (int k = 0; k < channel; k++)
				{
					// ENT
					std::vector<int> vecTemp;
					vecTemp.assign(BRIGHTNESS, 0);
					pSeg->m_vecBrightness.push_back(vecTemp);
					// scale if needed
					int p = (int)GetValue(r, c, k);
					pSeg->m_vecBrightness[k][p] = +1;
				}
			}
		}
	}

}

void ImgData::Mat2Vec(cv::Mat& mat)
{
	//// float
	//data.clear();
	//std::vector<std::vector<std::vector<float>>>().swap(data);
	//if (mat.data == NULL)
	//	return;
	//int iChannel = mat.channels();
	//if (!mat.isContinuous())
	//	return;
	//height = mat.rows;
	//width = mat.cols;
	//for (int b = 0; b < iChannel; b++)
	//{
	//	//TRACE(_T("band : %d\n"), b);
	//	std::vector<std::vector<float>> vecBand;
	//	for (int r = 0; r < height; r++)
	//	{
	//		std::vector<float> vecRow;
	//		for (int c = 0; c < width; c++)
	//		{
	//			float * p = (float*)mat.ptr(r);

	//			float f = (float)p[c * iChannel + b];
	//			vecRow.push_back(f);
	//			//TRACE(_T("%.0f, "), f);
	//		}
	//		vecBand.push_back(vecRow);
	//		//TRACE(_T("\n"));
	//	}
	//	data.push_back(vecBand);
	//}
}

void ImgData::CopyMat(cv::Mat& mat)
{
	if (mat.data != NULL)
	{
		data = mat.clone();
		width = data.cols;
		height = data.rows;
		channel = data.channels();
	}
	else
	{
		width = height = channel = 0;
	}
}

float ImgData::GetValue(int r, int c, int b)
{
	if (!data.isContinuous())
		return 0.0;
	if (r < 0 || r > height)
		return 0.0;
	if (c < 0 || c > width)
		return 0.0;
	if (b < 0 || b > channel)
		return 0.0;

	float * p = (float*)data.ptr(r);

	float f = (float)p[c * channel + b];
	return f;
}

bool ImgData::Initialize()
{
	if (tInfo.bTrace)
		CLog2File::GetInstance()->CreateLogFile();
	segment = new Segment*[width * height];
	// each pixel is assigned as a distinct segment
	if (data.data != NULL)
	{
		int iNum = 0;
		// row
		for (int r = 0; r < height; r++)
		{
			// col
			for (int c = 0; c < width; c++)
			{
				Segment* pSeg = new Segment(r, c);
			
				for (int b = 0; b < channel; b++)
				{
					// Avg
					pSeg->m_Avg.push_back(GetValue(r, c, b));
					// Variance
					//pSeg->m_Variance.push_back(0.0f);
					pSeg->m_StandardDeviation.push_back(0.0f);
				}
				pSeg->m_Entropy = 0.0;
				pSeg->m_iIndex = iNum++;
				//pSeg->min_row = pSeg->max_row = r;
				//pSeg->min_col = pSeg->max_col = c;
				segment[pSeg->m_iIndex] = pSeg;

				if (tInfo.bTrace && tInfo.row == r && tInfo.col == c)
				{
					CString strInfo;
					strInfo.Format(_T("\nsegmentation start image size width[%d] x height[%d]\n\n\n"), width, height);
					CLog2File::GetInstance()->WriteLog(strInfo);
					
					strInfo.Format(_T("Original segment ID : %d[%d, %d]\n"), (r * width + c), r, c);
					CLog2File::GetInstance()->WriteLog(strInfo);
					for (int k = 0; k < channel; k++)
					{
						strInfo.Format(_T("Original mean value[%d] : %.3f\n"), k, pSeg->m_Avg[k]);
						CLog2File::GetInstance()->WriteLog(strInfo);
					}
				}
			}
		}
	}
#ifdef MULTI_THREAD
	pFindEventArray = new HANDLE[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		pFindEventArray[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	mergeHandle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	bQuitFind = false;
	bForceMerge = false;
#endif

	/*
	Euclidean_Distance,
	Old_BSMSE,
	New_BSMSE,
	Old_BSMSE_New_Equation,
	New_BSMSE_New_Equation,
	Similarity_Measure
	*/
	switch (condition.pre_calcType)
	{
	case Euclidean_Distance:
		ptFilter_Pre = new EuclideanDistance(condition);
		break;
	case Old_BSMSE:
		ptFilter_Pre = new OldBSMSE(condition);
		break;
	case New_BSMSE:
		ptFilter_Pre = new NewBSMSE(condition);
		break;
	case Old_BSMSE_New_Equation:
		ptFilter_Pre = new OldBSMSE_NewEquation(condition);
		break;
	case New_BSMSE_New_Equation:
		ptFilter_Pre = new NewBSMSE_NewEquation(condition);
		break;
	//case Similarity_Measure:
	//	ptFilter = new SimilarityMeasure(condition);
	//	break;
	case Standard_Deviation:
		ptFilter_Pre = new StandardDeviation(condition);
		break;
	case Spectral_Angle_Mapper:
		ptFilter_Pre = new SAM(condition);
		break;
	case Entropy_Measure:
		ptFilter_Pre = new ENT(condition);
		break;
	case Root_Squared_Error:
		ptFilter_Pre = new RSE(condition);
		break;
	case Rooted_Difference_in_Variance:
		ptFilter_Pre = new RDV(condition);
		break;
	case Rooted_Difference_in_Squared_Error:
		ptFilter_Pre = new RDSE(condition);
		break;
	default:
		ptFilter_Pre = NULL;
		break;
	}
	ASSERT(ptFilter_Pre != NULL);

	switch (condition.calcType)
	{
	case Euclidean_Distance:
		ptFilter = new EuclideanDistance(condition);
		break;
	case Old_BSMSE:
		ptFilter = new OldBSMSE(condition);
		break;
	case New_BSMSE:
		ptFilter = new NewBSMSE(condition);
		break;
	case Old_BSMSE_New_Equation:
		ptFilter = new OldBSMSE_NewEquation(condition);
		break;
	case New_BSMSE_New_Equation:
		ptFilter = new NewBSMSE_NewEquation(condition);
		break;
	//case Similarity_Measure:
	//	ptFilter = new SimilarityMeasure(condition);
	//	break;
	case Standard_Deviation:
		ptFilter = new StandardDeviation(condition);
		break;
	case Spectral_Angle_Mapper:
		ptFilter = new SAM(condition);
		break;
	case Entropy_Measure:
		ptFilter = new ENT(condition);
		break;
	case Root_Squared_Error:
		ptFilter = new RSE(condition);
		break;
	case Rooted_Difference_in_Variance:
		ptFilter = new RDV(condition);
		break;
	case Rooted_Difference_in_Squared_Error:
		ptFilter = new RDSE(condition);
		break;
	default:
		ptFilter = NULL;
		break;
	}
	ASSERT(ptFilter != NULL);

	if (condition.calcType == Entropy_Measure)
	{
		InitializeForEntropy();
	}
	return true;
}

void ImgData::Init(segCondition& con, map<int, set<int>>& mapMask, TraceInfo& traceinfo)
{
	SetTraceInfo(traceinfo);
	// segmentation
	SetCondition(con);

	Initialize();

	InitNeighbours();

	ProcessviaMask(mapMask);
}