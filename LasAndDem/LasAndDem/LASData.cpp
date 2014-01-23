#include "StdAfx.h"
#include "LASData.h"

LASData::LASData(void)
{
	//ASSERT( 243 == sizeof(PUBLIC_HEADER_BLOCK) );
	//ASSERT( 54 == sizeof(VARIABLE_LENGTH_RECORDS) );
	//ASSERT( 20 == sizeof(POINT_DATA_RECORD_0) );
	//ASSERT( 28 == sizeof(POINT_DATA_RECORD_1) );
	//ASSERT( 26 == sizeof(POINT_DATA_RECORD_2) );
	//ASSERT( 34 == sizeof(POINT_DATA_RECORD_3) );
	//ASSERT( 57 == sizeof(POINT_DATA_RECORD_4) );
	//ASSERT( 63 == sizeof(POINT_DATA_RECORD_5) );
}

LASData::~LASData(void)
{
	if (m_LogFile.m_hFile != INVALID_HANDLE_VALUE)
		m_LogFile.Close();
}

bool LASData::Init()
{
	// Get Current Directory
	TCHAR szPath[MAX_PATH_LEN]={0};
	GetModuleFileName(NULL, szPath, MAX_PATH_LEN);
	(_tcsrchr(szPath, _T('\\')))[1] = 0;
	CString strPath(szPath);
	strPath += CONFIG_FILE_NAME;
	m_strIniFilePath = strPath;

	try
	{
		if (!m_LogFile.Open(_T("./Log.txt"), CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone | CFile::modeNoTruncate))
		{
			DWORD dwError = ::GetLastError();
			TRACE(_T("open log file failed.(Error:%d)\n"), dwError);
		}
		//int iLen = m_LogFile.GetLength();
		m_LogFile.Seek(0, CFile::end);
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		TRACE(_T("CFileException catched when open log file. [%s]\n"), szError);
	}

	m_bBigEndian = IsBigEndian();

	memset(szPath, 0, MAX_PATH_LEN);
	GetPrivateProfileString(_T("LAS"), _T("FilePath"), _T(""), szPath, sizeof(szPath), strPath);
	m_strLASDataFileDir.Format(_T("%s"), szPath);

	if (m_strLASDataFileDir.IsEmpty())
	{
		//TRACE(_T("read config file path failed.(GLA14)\n"));
		WriteLog("read config file path failed.(LAS)");
		return false;
	}

	m_strLASDataFileDir += _T("*.*");

	m_iReturn_Number = GetPrivateProfileInt(_T("LAS"), _T("ReturnNumber"), 0, strPath);

	return true;
}

void LASData::WriteLog(CString strLog)
{
	COleDateTime dt = COleDateTime::GetCurrentTime();
	CString strTime = dt.Format(_T("%Y-%m-%d %H:%M:%S : "));
	CString str;
	str.Format(_T("%s%s\n"), strTime, strLog);
	m_LogFile.WriteString(str);
}

void LASData::ReadLAS()
{
	// file is little endian
	CFileFind ff;
	BOOL bFind = ff.FindFile(m_strLASDataFileDir);

	int iCount = 0;
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();
			CString str = strFile.Right(3);

			CString strName = ff.GetFileName();
			int ii = strName.Find(_T('.'));
			CString strSuffix = strName.Left(ii);
			if (str.CompareNoCase(_T("las")) == 0)
			{
#ifdef NEEDTXT
				try
				{
					CString strOutput;
					strOutput.Format(_T("SearchResult_%s.txt"), strSuffix);
					if (!m_LASDataFile.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
					{
						DWORD dwError = ::GetLastError();
						//TRACE(_T("open OutputData14 file failed.(Error:%d)\n"), dwError);
						CString strErr;
						strErr.Format(_T("open SearchResult(%s) file failed.(Error:%d)\n"), strSuffix, dwError);
						WriteLog(strErr);
					}
				}
				catch(CFileException* e)
				{
					TCHAR szError[MAX_PATH_LEN];
					e->GetErrorMessage(szError, MAX_PATH_LEN);
					//TRACE(_T("CFileException catched when open OutputData14 file. [%s]\n"), szError);
					CString strErr;
					strErr.Format(_T("CFileException catched when open SearchResult file. [%s]\n"), szError);
					WriteLog(strErr);
				}
#endif

#ifdef DEM
				int iDot = strFile.Find(_T('.'));
				m_strDEMDataFile = strFile.Left(iDot);
				m_strDEMDataFile += _T(".dem");
#endif
				if (m_bBigEndian)
				{
					// needs to transfer
				}
				else
				{
					int iCount = 0;
					CFile f;
					try
					{
						if (f.Open(strFile, CFile::modeRead))
						{
							m_LasOriginal.clear();
							//PUBLIC_HEADER_BLOCK phb;
							memset(&m_phb, 0, sizeof(PUBLIC_HEADER_BLOCK));
							f.Read(&m_phb, sizeof(PUBLIC_HEADER_BLOCK));
							//phb.iPoint_Data_Format_ID
							//phb.iPoint_Data_Record_Length
							int iLen = GetStructLength(m_phb.iPoint_Data_Format_ID);
							ASSERT(iLen == m_phb.iPoint_Data_Record_Length);
							char *pBuffer = new char[iLen];

							//if (phb.iNumber_of_Varable_Length_Records)
							f.Seek(m_phb.iOffset_to_point_data, CFile::begin);
							ASSERT(f.GetLength() == m_phb.iOffset_to_point_data + m_phb.iNumber_of_point_records * m_phb.iPoint_Data_Record_Length);

							CString strInfo;
#ifdef RETURNNUM
							strInfo.Format(_T("Return = %d X Y Z\n"), m_iReturn_Number);
							m_LASDataFile.WriteString(strInfo);
#endif
							for (UINT i = 0; i < m_phb.iNumber_of_point_records; i++)
							//for (UINT i = 0; i < 100; i++)
							{
								memset(pBuffer, 0, iLen);
								f.Read(pBuffer, iLen);

								CPOINT_DATA_RECORD pdr(pBuffer, m_phb.iPoint_Data_Format_ID);

								// search & print
								if (IsValid(&pdr))
								{
									iCount++;
									LasStorage ls;
									//(pData->m_pData->X * pHeader->dX_scale_factor + pHeader->dX_offset)
									ls.x = pdr.m_pData->X * m_phb.dX_scale_factor + m_phb.dX_offset;
									ls.y = pdr.m_pData->Y * m_phb.dY_scale_factor + m_phb.dY_offset;
									ls.z = pdr.m_pData->Z * m_phb.dZ_scale_factor + m_phb.dZ_offset;
									ls.r = pdr.m_pData->Number_of_Returns;
									m_LasOriginal.push_back(ls);
#ifdef NEEDTXT
									Output(&pdr, &phb);
#endif
								}
							}
							delete pBuffer;
							f.Close();
#ifdef RETURNNUM
							CString strInfo;
							strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), iCount, strFile);
							m_LASDataFile.WriteString(strInfo);
							TRACE(_T("\n"));
#endif
						}
						else
						{
							DWORD dwError = ::GetLastError();
							CString strErr;
							strErr.Format(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
							WriteLog(strErr);
							//TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
						}
					}
					catch(CFileException* e)
					{
						TCHAR szError[MAX_PATH_LEN];
						e->GetErrorMessage(szError, MAX_PATH_LEN);
						//TRACE(_T("CFileException catched. [%s]\n"), szError);
						CString strErr;
						strErr.Format(_T("CFileException catched. [%s]\n"), szError);
						WriteLog(strErr);
					}
				}
#ifdef NEEDTXT
				if (m_LASDataFile.m_hFile != INVALID_HANDLE_VALUE)
					m_LASDataFile.Close();
#endif
			}
		}
	}
}

int LASData::GetStructLength(int type)
{
	int ilen = 0;
	switch (type)
	{
	case 0:
		ilen = sizeof(POINT_DATA_RECORD_0);
		break;
	case 1:
		ilen = sizeof(POINT_DATA_RECORD_1);
		break;
	case 2:
		ilen = sizeof(POINT_DATA_RECORD_2);
		break;
	case 3:
		ilen = sizeof(POINT_DATA_RECORD_3);
		break;
	case 4:
		ilen = sizeof(POINT_DATA_RECORD_4);
		break;
	case 5:
		ilen = sizeof(POINT_DATA_RECORD_5);
		break;
	default:
		break;
	}
	return ilen;
}

bool LASData::IsValid(CPOINT_DATA_RECORD* pData)
{
#ifdef RETURNNUM
	if (pData->m_pData->Return_Number == m_iReturn_Number)
		return true;
	return false;
#else
	return true;
#endif
}
#ifdef NEEDTXT
void LASData::Output(CPOINT_DATA_RECORD* pData, PUBLIC_HEADER_BLOCK* pHeader)
{
	// Only X,Y,Z
	// value = (x,y,z) * scale + offset
	if (m_LASDataFile.m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(_T("Output SearchResult is invalid!"));
		return;
	}
	CString strOutput;
	/*strOutput.Format(_T("X:%.8f, Y:%.8f, Z:%.8f\n"), (pData->m_pData->X * pHeader->dX_scale_factor + pHeader->dX_offset)
		,(pData->m_pData->Y * pHeader->dY_scale_factor + pHeader->dY_offset)
		,(pData->m_pData->Z * pHeader->dZ_scale_factor + pHeader->dZ_offset));*/

	strOutput.Format(_T("%.3f,%.3f,%.3f\n"), ((double)pData->m_pData->X * pHeader->dX_scale_factor + pHeader->dX_offset)
		,((double)pData->m_pData->Y * pHeader->dY_scale_factor + pHeader->dY_offset)
		,((double)pData->m_pData->Z * pHeader->dZ_scale_factor + pHeader->dZ_offset));

	m_LASDataFile.WriteString(strOutput);

	//m_LASDataFile.WriteString("\n------------------------------------------------------------\n\n");
}
#endif

#ifdef DEM
void LASData::ReadDEM()
{
	CStdioFile f;
	memset(&m_DemHeader, 0, sizeof(DemHeader));

	// y from max to min
	// x from min to max
	// first line is y_max [x_min, x_max]
	// coller is lower left

	try
	{
		if (f.Open(m_strDEMDataFile, CFile::modeRead))
		{
			int iFileLen = f.GetLength();
			CString strContent;
			f.ReadString(strContent);

			// ncols
			CString strValue = strContent.Right(strContent.GetLength() - strlen(_T("ncols")));
			strValue.Trim();
			m_DemHeader.nCols = atoi(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			// nrows
			strContent.Empty();
			f.ReadString(strContent);
			strValue = strContent.Right(strContent.GetLength() - strlen(_T("nrows")));
			strValue.Trim();
			m_DemHeader.nRows = atoi(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			// xllcorner
			strContent.Empty();
			f.ReadString(strContent);
			strValue = strContent.Right(strContent.GetLength() - strlen(_T("xllcorner")));
			strValue.Trim();
			m_DemHeader.xllcorner = atof(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			// yllcorner
			strContent.Empty();
			f.ReadString(strContent);
			strValue = strContent.Right(strContent.GetLength() - strlen(_T("yllcorner")));
			strValue.Trim();
			m_DemHeader.yllcorner = atof(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			// cellsize
			strContent.Empty();
			f.ReadString(strContent);
			strValue = strContent.Right(strContent.GetLength() - strlen(_T("cellsize")));
			strValue.Trim();
			m_DemHeader.cellsize = atoi(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			// NODATA_value
			strContent.Empty();
			f.ReadString(strContent);

			int iRead = f.Seek(0, CFile::current);
			int count_y = 0;
			int count = 0;
			m_DemOriginal.clear();

			double x = m_DemHeader.xllcorner;
			double y = m_DemHeader.yllcorner + m_DemHeader.cellsize * m_DemHeader.nCols;
			while (iFileLen - iRead > 0)
			{
				// records
				strContent.Empty();
				f.ReadString(strContent);
				iRead = f.Seek(0, CFile::current);
				count = 0;
				x = m_DemHeader.xllcorner;

				// spilt

				TCHAR* token = strtok(strContent.GetBuffer(), _T(" "));
				while (token)
				{
					count++;
					
					// token is the value;
					DemStorage ds;
					ds.x = x;
					ds.y = y;
					ds.z = atof(token);
					m_DemOriginal.push_back(ds);
					token = strtok(NULL, _T(" "));
					x += m_DemHeader.cellsize;
				}
				ASSERT(count == m_DemHeader.nRows);
				y -= m_DemHeader.cellsize;
				count_y++;
			}

			ASSERT(m_DemHeader.nCols == count_y);

			f.Close();
		}
		else
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, m_strDEMDataFile);
			WriteLog(strErr);
			//TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		WriteLog(strErr);
	}
}

void LASData::Adjust()
{
	// m_LasOriginal, m_DemOriginal -> m_LasAdjust;
	for (int i = 0; i < m_LasOriginal.size(); i++)
	{
		// calculate the postion in m_DemOriginal
		int x = (int)(m_LasOriginal[i].x - (int)m_DemHeader.xllcorner) / m_DemHeader.cellsize;
		int y = m_DemHeader.nCols - (int)((fabs)(m_LasOriginal[i].y - (int)m_DemHeader.yllcorner)) / m_DemHeader.cellsize;
		int position = x + y * m_DemHeader.nCols;

		AdjustStorage as;
		as.x = m_LasOriginal[i].x;
		as.y = m_LasOriginal[i].y;
		as.r = m_LasOriginal[i].r;
		as.z = m_LasOriginal[i].z - m_DemOriginal[position].z;
		if (as.z > 0.0 && as.z < 50.0)
			m_Adjust.push_back(as);
	}

	// sort
	std::sort(m_Adjust.begin(), m_Adjust.end(), Comparer());
	// ouput to a txt file in [LAS] folder
	CString strFile;
	int iDot = m_strDEMDataFile.Find(_T('.'));
	strFile = m_strDEMDataFile.Left(iDot);
	strFile += _T(".adj");

	// binary or txt
#ifdef ADJTXT
	// txt
	CStdioFile f;

	try
	{
		if (f.Open(strFile, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			// write las header first
			CString strOut;
			strOut.Format(_T("%.3f\t%.3f\t%.3f\t%.3f\n"), m_phb.dMin_X, m_phb.dMin_Y, m_phb.dMax_X, m_phb.dMax_Y);
			f.WriteString(strOut);
			for (std::vector<AdjustStorage>::iterator iter = m_Adjust.begin(); iter != m_Adjust.end(); iter++)
			{
				strOut.Format(_T("%.2f\t%.2f\t%.2f\t%d\n"), iter->x, iter->y, iter->z, iter->r);
				f.WriteString(strOut);
			}
			f.Close();

			WritePrivateProfileString(_T("LAS"), _T("AdjustFilePath"), strFile, m_strIniFilePath);
		}
		else
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReAdjustadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
			WriteLog(strErr);
			//TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		WriteLog(strErr);
	}
#else
	// binary
	CFile f;

	try
	{
		if (f.Open(strFile, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			for (int i = 0; i < m_LasOriginal.size(); i++)
			{

			}
		}
		else
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReAdjustadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
			WriteLog(strErr);
			//TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		WriteLog(strErr);
	}
#endif
}

void LASData::LoadAdjust()
{
	m_arrAdjust.clear();
	TCHAR szPath[MAX_PATH_LEN]={0};
	memset(szPath, 0, MAX_PATH_LEN);
	GetPrivateProfileString(_T("LAS"), _T("AdjustFilePath"), _T(""), szPath, sizeof(szPath), m_strIniFilePath);

	CString strFile(szPath);
	memset(&m_resultHeader, 0, sizeof(Result_Header));

	m_resultHeader.delta_x = GetPrivateProfileInt(_T("LAS"), _T("DeltaX"), 0, m_strIniFilePath);
	m_resultHeader.delta_y = GetPrivateProfileInt(_T("LAS"), _T("DeltaY"), 0, m_strIniFilePath);

#ifdef ADJTXT
	// txt
	CStdioFile f;

	try
	{
		if (f.Open(strFile, CFile::modeRead | CFile::shareDenyNone))
		{
			// x/ty/tz/tr
			CString strContent;
			int iFileLen = f.GetLength();

			f.ReadString(strContent);
			TCHAR* token = strtok(strContent.GetBuffer(), _T("\t"));
			m_resultHeader.min_x = atof(token);
			token = strtok(NULL, _T("\t"));
			m_resultHeader.min_y = atof(token);
			token = strtok(NULL, _T("\t"));
			m_resultHeader.max_x = atof(token);
			token = strtok(NULL, _T("\t"));
			m_resultHeader.max_y = atof(token);
			strContent.ReleaseBuffer();
			int iRead = f.Seek(0, CFile::current);

			int count_x = (int)(m_resultHeader.max_x - m_resultHeader.min_x) / m_resultHeader.delta_x + 1;
			int count_y = (int)(m_resultHeader.max_y - m_resultHeader.min_y) / m_resultHeader.delta_y + 1;

			ASSERT(count_x == count_y);
			std::vector<std::vector<AdjustStorage>> temp;
			temp.assign(count_y, std::vector<AdjustStorage>());
			m_arrAdjust.assign(count_x, temp);

			ASSERT(m_arrAdjust.size() == count_y);
			ASSERT(m_arrAdjust[0].size() == count_x);

			while (iFileLen - iRead > 0)
			{
				strContent.Empty();
				f.ReadString(strContent);
				iRead = f.Seek(0, CFile::current);

				AdjustStorage as;
				TCHAR* token = strtok(strContent.GetBuffer(), _T("\t"));
				as.x = atof(token);
				token = strtok(NULL, _T("\t"));
				as.y = atof(token);
				token = strtok(NULL, _T("\t"));
				as.z = atof(token);
				token = strtok(NULL, _T("\t"));
				as.r = atoi(token);
				strContent.ReleaseBuffer();

				int x = (int)(as.x - m_resultHeader.min_x) / m_resultHeader.delta_x;
				int y = (int)(m_resultHeader.max_y - as.y) / m_resultHeader.delta_y;
				m_arrAdjust[y][x].push_back(as);

				//m_Adjust.push_back(as);
			}
			f.Close();
		}
		else
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReAdjustadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
			WriteLog(strErr);
			//TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		WriteLog(strErr);
	}
#endif
}

//void LASData::Calculate()
//{
//	if (m_arrAdjust.empty())
//		return;
//
//	CString strFile_avg, strFile_per;
//	TCHAR szPath[MAX_PATH_LEN]={0};
//	memset(szPath, 0, MAX_PATH_LEN);
//	GetPrivateProfileString(_T("LAS"), _T("AdjustFilePath"), _T(""), szPath, sizeof(szPath), m_strIniFilePath);
//
//	CString strFile(szPath);
//
//	int iDot = strFile.Find(_T('.'));
//	strFile_avg = strFile.Left(iDot);
//	strFile_per = strFile.Left(iDot);
//	strFile_avg += _T(".avg");
//	strFile_per += _T(".per");
//
//	CStdioFile f_avg, f_per;
//
//	try
//	{
//		if (!f_avg.Open(strFile_avg, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
//		{
//			CString strErr;
//			strErr.Format(_T("CreateFile Failed[%s]!\n"), strFile_avg);
//			WriteLog(strErr);
//		}
//		/*
//		ncols         2501
//		nrows         2501
//		xllcorner     209999.7779319
//		yllcorner     5509999.3899256
//		cellsize      1
//		NODATA_value  -9999
//		*/
//		CString strOut;
//		strOut.Format(_T("ncols         %d\n"), m_arrAdjust[0].size());
//		f_avg.WriteString(strOut);
//		strOut.Format(_T("nrows         %d\n"), m_arrAdjust.size());
//		f_avg.WriteString(strOut);
//		strOut.Format(_T("xllcorner     %.2f\n"), m_resultHeader.min_x);
//		f_avg.WriteString(strOut);
//		strOut.Format(_T("yllcorner     %.2f\n"), m_resultHeader.min_y);
//		f_avg.WriteString(strOut);
//		strOut.Format(_T("cellsize      %d\n"), m_resultHeader.delta_x);
//		f_avg.WriteString(strOut);
//		strOut.Format(_T("NODATA_value  %d\n"), 0);
//		f_avg.WriteString(strOut);
//
//	}
//	catch(CFileException* e)
//	{
//		TCHAR szError[MAX_PATH_LEN];
//		e->GetErrorMessage(szError, MAX_PATH_LEN);
//		//TRACE(_T("CFileException catched. [%s]\n"), szError);
//		CString strErr;
//		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
//		WriteLog(strErr);
//	}
//
//	try
//	{
//		if (!f_per.Open(strFile_per, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
//		{
//			CString strErr;
//			strErr.Format(_T("CreateFile Failed[%s]!\n"), strFile_per);
//			WriteLog(strErr);
//		}
//
//		CString strOut;
//		strOut.Format(_T("ncols         %d\n"), m_arrAdjust[0].size());
//		f_per.WriteString(strOut);
//		strOut.Format(_T("nrows         %d\n"), m_arrAdjust.size());
//		f_per.WriteString(strOut);
//		strOut.Format(_T("xllcorner     %.2f\n"), m_resultHeader.min_x);
//		f_per.WriteString(strOut);
//		strOut.Format(_T("yllcorner     %.2f\n"), m_resultHeader.min_y);
//		f_per.WriteString(strOut);
//		strOut.Format(_T("cellsize      %d\n"), m_resultHeader.delta_x);
//		f_per.WriteString(strOut);
//		strOut.Format(_T("NODATA_value  %d\n"), 0);
//		f_per.WriteString(strOut);
//	}
//	catch(CFileException* e)
//	{
//		TCHAR szError[MAX_PATH_LEN];
//		e->GetErrorMessage(szError, MAX_PATH_LEN);
//		//TRACE(_T("CFileException catched. [%s]\n"), szError);
//		CString strErr;
//		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
//		WriteLog(strErr);
//	}
//
//	// y
//	for (int i = 0; i< m_arrAdjust.size(); i++)
//	{
//		// x
//		CString strAvg;
//		CString strPer;
//
//		for (int j = 0; j < m_arrAdjust[i].size(); j++)
//		{
//			double sum_z = 0.0;
//			int rcount = 0;
//			int iCount = m_arrAdjust[i][j].size();
//			//ASSERT(iCount > 0);
//			if (iCount > 0)
//			{
//				for (int k = 0; k < iCount; k++)
//				{
//					// avg of z
//					// ret=1 per
//					if (m_arrAdjust[i][j][k].r == 1)
//						rcount++;
//					sum_z += m_arrAdjust[i][j][k].z;
//				}
//
//				double avg_z = sum_z / iCount;
//				double dp = ((double)rcount / (double)iCount) * 100.0;
//				int percentage = int(dp);
//				// output
//				//m_resultHeader
//				double ddddd = m_resultHeader.min_x + i * m_resultHeader.delta_x;
//
//				//CString strOut;
//				//strOut.Format(_T("%.2f,%.2f,%.2f\n"), m_resultHeader.min_x + i * m_resultHeader.delta_x, m_resultHeader.min_y + j * m_resultHeader.delta_y, avg_z);
//				//f_avg.WriteString(strOut);
//
//				//strOut.Format(_T("%.2f,%.2f,%d\n"), m_resultHeader.min_x + i * m_resultHeader.delta_x, m_resultHeader.min_y + j * m_resultHeader.delta_y, percentage);
//				//f_per.WriteString(strOut);
//				CString str;
//				str.Format(_T("%.2f "), avg_z);
//				strAvg += str;
//
//				str.Format(_T("%d "), percentage);
//				strPer += str;
//			}
//			else
//			{
//				//CString strOut;
//				//strOut.Format(_T("%.2f,%.2f,%.2f\n"), m_resultHeader.min_x + i * m_resultHeader.delta_x, m_resultHeader.min_y + j * m_resultHeader.delta_y, 0.0);
//				//f_avg.WriteString(strOut);
//
//				//strOut.Format(_T("%.2f,%.2f,%d\n"), m_resultHeader.min_x + i * m_resultHeader.delta_x, m_resultHeader.min_y + j * m_resultHeader.delta_y, 0);
//				//f_per.WriteString(strOut);
//				CString str;
//				str.Format(_T("%.2f "), 0.0);
//				strAvg += str;
//
//				str.Format(_T("%d "), 0);
//				strPer += str;
//			}
//		}
//		strAvg.TrimRight();
//		strAvg += _T("\n");
//
//		strPer.TrimRight();
//		strPer += _T("\n");
//
//		f_avg.WriteString(strAvg);
//		f_per.WriteString(strPer);
//	}
//
//	f_avg.Close();
//	f_per.Close();
//
//}
void LASData::Calculate()
{
	if (m_arrAdjust.empty())
		return;

	//CString strFile_avg, strFile_per;
	CString strFile_Height;
	TCHAR szPath[MAX_PATH_LEN]={0};
	memset(szPath, 0, MAX_PATH_LEN);
	GetPrivateProfileString(_T("LAS"), _T("AdjustFilePath"), _T(""), szPath, sizeof(szPath), m_strIniFilePath);

	CString strFile(szPath);

	int iDot = strFile.Find(_T('.'));
	strFile_Height = strFile.Left(iDot);
	strFile_Height += _T(".hgt");

	CStdioFile f_height;

	try
	{
		if (!f_height.Open(strFile_Height, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			CString strErr;
			strErr.Format(_T("CreateFile Failed[%s]!\n"), strFile_Height);
			WriteLog(strErr);
		}
		/*
		ncols         2501
		nrows         2501
		xllcorner     209999.7779319
		yllcorner     5509999.3899256
		cellsize      1
		NODATA_value  -9999
		*/
		CString strOut;
		strOut.Format(_T("ncols         %d\n"), m_arrAdjust[0].size());
		f_height.WriteString(strOut);
		strOut.Format(_T("nrows         %d\n"), m_arrAdjust.size());
		f_height.WriteString(strOut);
		strOut.Format(_T("xllcorner     %.2f\n"), m_resultHeader.min_x);
		f_height.WriteString(strOut);
		strOut.Format(_T("yllcorner     %.2f\n"), m_resultHeader.min_y);
		f_height.WriteString(strOut);
		strOut.Format(_T("cellsize      %d\n"), m_resultHeader.delta_x);
		f_height.WriteString(strOut);
		strOut.Format(_T("NODATA_value  %d\n"), 0);
		f_height.WriteString(strOut);

	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		WriteLog(strErr);
	}

	
	// y
	for (int i = 0; i< m_arrAdjust.size(); i++)
	{
		// x
		CString strHeight;
		for (int j = 0; j < m_arrAdjust[i].size(); j++)
		{
			double sum_z = 0.0;
			int iCount = m_arrAdjust[i][j].size();
			std::vector<double> z_value;
			//ASSERT(iCount > 0);
			if (iCount > 0)
			{
				for (int k = 0; k < iCount; k++)
				{
					// avg of z
					sum_z += m_arrAdjust[i][j][k].z;
					z_value.push_back(m_arrAdjust[i][j][k].z);
				}

				double d6 = 1.0;
				if (iCount >= 10)
				{
					double avg_z = sum_z / iCount;
					// calculate height
					// step 1 sort asc;
					std::sort(z_value.begin(), z_value.end(), Compare_Double());
					// step 2 avg_z

					// step 3 sd
					int iSize = z_value.size();
					double dSum2 = 0.0;
					double dSum3 = 0.0;
					double dSum4 = 0.0;
					for (int i = 0; i < iSize; i++)
					{
						dSum2 += (z_value[i] - avg_z) * (z_value[i] - avg_z);
						dSum3 += (z_value[i] - avg_z) * (z_value[i] - avg_z) * (z_value[i] - avg_z);
						dSum4 += (z_value[i] - avg_z) * (z_value[i] - avg_z) * (z_value[i] - avg_z) * (z_value[i] - avg_z);
					}
					double sd = sqrt(dSum2 / (double)(iSize - 1));
					// step 4 kurtosis
					double kurtosis = dSum4 / ((double)(iSize - 1) * sd * sd * sd * sd); 
					// step 5 skew 
					double skew = dSum3 / ((double)(iSize - 1) * sd * sd * sd);
					// step 6 P90
					int iPos = (int)(0.5 + (double)iSize * 0.9);
					double p90 = z_value[iPos - 1];
					// step 7 D6
					double dTemp = 0.6 * (iSize + 1);
					int iTemp = (int)dTemp;
					dTemp = dTemp - iTemp;
					d6 = z_value[iTemp - 1] + dTemp * (z_value[iTemp] - z_value[iTemp - 1]);
				}

				CString str;
				str.Format(_T("%.2f "), d6);
				strHeight += str;
			}
			else
			{
				CString str;
				str.Format(_T("%.2f "), 0.0);
				strHeight += str;
			}
		}
		strHeight.TrimRight();
		strHeight += _T("\n");

		f_height.WriteString(strHeight);
	}

	f_height.Close();
}
#endif

