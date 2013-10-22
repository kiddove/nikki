#include "StdAfx.h"
#include "GLAData.h"

GLAData::GLAData(void)
{
	int i1 = sizeof(GLA14);
	int i2 = sizeof(GLA01_MAIN);
	int i3 = sizeof(GLA01_LONG);
	int i4 = sizeof(GLA01_SHORT);
	ASSERT(i1 == 10000);
	ASSERT(i2 == 4660);
	ASSERT(i3 == 4660);
	ASSERT(i4 == 4660);
}

GLAData::~GLAData(void)
{
	if (m_LogFile.m_hFile != INVALID_HANDLE_VALUE)
		m_LogFile.Close();
}

bool GLAData::Init()
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

	TCHAR szPath14[MAX_PATH_LEN]={0};
	GetPrivateProfileString(_T("GLA14"), _T("FilePath"), _T(""), szPath14, sizeof(szPath14), strPath);
	m_strData14FileDir.Format(_T("%s"), szPath14);

	//m_bufSize_GLA14 = GetPrivateProfileInt(_T("GLA14"), _T("BufferSize"), 0, strPath);
	memset(szPath14, 0, MAX_PATH_LEN);
	GetPrivateProfileString(_T("GLA01"), _T("FilePath"), _T(""), szPath14, sizeof(szPath14), strPath);
	m_strData01FileDir.Format(_T("%s"), szPath14);


	if (m_strData14FileDir.IsEmpty())
	{
		//TRACE(_T("read config file path failed.(GLA14)\n"));
		WriteLog("read config file path failed.(GLA14)");
		return false;
	}

	if (m_strData01FileDir.IsEmpty())
	{
		//TRACE(_T("read config file path failed.(GLA01)\n"));
		WriteLog("read config file path failed.(GLA01)");
		return false;
	}

	m_strData01FileDir += _T("*.*");
	m_strData14FileDir += _T("*.*");

	m_iLatMin = GetPrivateProfileInt(_T("Latitude"), _T("Min"), 0, strPath);
	m_iLatMax = GetPrivateProfileInt(_T("Latitude"), _T("Max"), 0, strPath);

	m_iLonMin = GetPrivateProfileInt(_T("Longitude"), _T("Min"), 0, strPath);
	m_iLonMax = GetPrivateProfileInt(_T("Longitude"), _T("Max"), 0, strPath);

	return true;
}
bool GLAData::IsInvalid(CString& strFile, int& stLen)
{
	CFile f;
	try
	{
		if (f.Open(strFile, CFile::modeRead))
		{
			int ilen = (int)f.GetLength();
			f.Close();
			int isize = sizeof(stLen);
			if (ilen % isize != 0)
			{
				TRACE(_T("Invalid File length!\n"));
				return false;
			}
			else
				return true;
		}
		else
		{
			DWORD dwError = ::GetLastError();
			TRACE(_T("IsInvalid --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
		return false;
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		TRACE(_T("CFileException catched. [%s]\n"), szError);
		return false;
	}
	return true;
}

void GLAData::GetLength(CString& strFile, int& headLen, int& stLen)
{
	headLen = 0;
	stLen = 0;
	int RecNum = 0;
	CStdioFile stdf;
	try
	{
		if (stdf.Open(strFile, CFile::modeRead))
		{
			CString strRecl, strNumhead;
			stdf.ReadString(strRecl);
			stdf.ReadString(strNumhead);
			stdf.Close();
			strRecl.Trim();
			if (strRecl.Find(_T("Recl")) == 0)
			{
				int po1 = strRecl.Find(_T('='));
				int po2 = strRecl.Find(_T(';'));
				CString sRecl = strRecl.Mid(po1 + 1, po2 - po1 - 1);
				stLen = atoi(sRecl);
			}
			if (strNumhead.Find(_T("Numhead")) == 0)
			{
				int po1 = strNumhead.Find(_T('='));
				int po2 = strNumhead.Find(_T(';'));
				CString sNum = strNumhead.Mid(po1 + 1, po2 - po1 - 1);
				RecNum = atoi(sNum);
				headLen = RecNum * stLen;
			}
		}
		else
		{
			DWORD dwError = ::GetLastError();
			TRACE(_T("open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		TRACE(_T("CFileException catched. [%s]\n"), szError);
	}
}


void GLAData::ReadGLA01()
{
	// output file
	try
	{
		CString strOutput;
		strOutput.Format(_T("./DataFile01%s.txt"), m_strMark);
		if (!m_DataFile01.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("open OutputData01 file failed.(Error:%d)"), dwError);
			WriteLog(strErr);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		CString strErr;
		strErr.Format(_T("CFileException catched when open OutputData01 file. [%s]"), szError);
		WriteLog(strErr);
	}

	CFileFind ff;
	BOOL bFind = ff.FindFile(m_strData01FileDir);

	int iCount = 0;
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();
			CString strName = ff.GetFileName();
			int iPos = strName.Find(_T('_'));
			CString strTemp = strName.Left(iPos);

			if (strTemp.CompareNoCase(_T("GLA01")) == 0)
			{
				if (strName.Find(m_strMark) > 0)
				{
					int it = 0;
					int iHeader = 0, istLen = 0;
					GetLength(strFile, iHeader, istLen);
					if (!this->IsInvalid(strFile, istLen))
						return;

					if (m_bBigEndian)
					{
						CFile f;
						try
						{
							if (f.Open(strFile, CFile::modeRead))
							{
								m_iCount01 = 0;
								int ilen = (int)f.GetLength();
								int iCount =(ilen - iHeader) / istLen;
								char* pBuffer = new char[istLen];
								f.Seek(iHeader, CFile::begin);

								for (int i = 0; i < iCount; i++)
								{
									memset(pBuffer, 0, istLen);
									f.Read(pBuffer, istLen);
									TraversalData01_Big(pBuffer);
								}
								delete pBuffer;
								f.Close();
								CString strInfo;
								strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), m_iCount01, strFile);
								m_DataFile01.WriteString(strInfo);
							}
							else
							{
								DWORD dwError = ::GetLastError();
								TRACE(_T("ReadGLA01 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
							}
						}
						catch(CFileException* e)
						{
							TCHAR szError[MAX_PATH_LEN];
							e->GetErrorMessage(szError, MAX_PATH_LEN);
							TRACE(_T("CFileException catched. [%s]\n"), szError);
						}
					}
					else
					{
						CFile f;
						try
						{
							if (f.Open(strFile, CFile::modeRead))
							{
								m_iCount01 = 0;
								int ilen = (int)f.GetLength();
								char* pBuffer = new char[istLen];
								int iCount =(ilen - iHeader) / istLen;
								f.Seek(iHeader, CFile::begin);
								for (int i = 0; i < iCount; i++)
								{
									memset(pBuffer, 0, istLen);
									f.Read(pBuffer, istLen);
									TraversalData01_Little(pBuffer);
								}
								delete pBuffer;
								f.Close();
								CString strInfo;
								strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), m_iCount01, strFile);
								m_DataFile01.WriteString(strInfo);
							}
							else
							{
								DWORD dwError = ::GetLastError();
								TRACE(_T("ReadGLA01 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
							}
						}
						catch(CFileException* e)
						{
							TCHAR szError[MAX_PATH_LEN];
							e->GetErrorMessage(szError, MAX_PATH_LEN);
							TRACE(_T("CFileException catched. [%s]\n"), szError);
						}
					}
				}
			}
		}
	}
	if (m_DataFile01.m_hFile != INVALID_HANDLE_VALUE)
		m_DataFile01.Close();
	return;
}

// big endian or little endian
void GLAData::ReadGLA14()
{
	m_Search_Rec_Index.clear();
	// output file
	try
	{
		if (!m_DataFile14.Open("./DataFile14.txt", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			DWORD dwError = ::GetLastError();
			//TRACE(_T("open OutputData14 file failed.(Error:%d)\n"), dwError);
			CString strErr;
			strErr.Format(_T("open OutputData14 file failed.(Error:%d)\n"), dwError);
			WriteLog(strErr);
		}
	}
	catch(CFileException* e)
	{
		TCHAR szError[MAX_PATH_LEN];
		e->GetErrorMessage(szError, MAX_PATH_LEN);
		//TRACE(_T("CFileException catched when open OutputData14 file. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched when open OutputData14 file. [%s]\n"), szError);
		WriteLog(strErr);
	}
	CFileFind ff;
	BOOL bFind = ff.FindFile(m_strData14FileDir);

	int iCount = 0;
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();

			// GLA14_633_2123_001_1331_0_01_0001.DAT
			CString strName = ff.GetFileName();
			int iPos = strName.Find(_T('_'));
			CString strTemp = strName.Left(iPos);

			if (strTemp.CompareNoCase(_T("GLA14")) == 0)
			{
				iPos = strName.Find(_T('_'), iPos + 1);
				int iPos2 = strName.Find(_T('_'), iPos+1);
				iPos2 = strName.Find(_T('_'), iPos2+1);
				iPos2 = strName.Find(_T('_'), iPos2+1);
				m_strMark = strName.Mid(iPos, iPos2 - iPos);
				int it = 0;
				int iHeader = 0, istLen = 0;
				GetLength(strFile, iHeader, istLen);
				if (!this->IsInvalid(strFile, istLen))
					return;

				if (m_bBigEndian)
				{
					CFile f;
					try
					{
						if (f.Open(strFile, CFile::modeRead))
						{
							m_iCount14 = 0;
							int ilen = (int)f.GetLength();
							int iCount =(ilen - iHeader) / istLen;
							char* pBuffer = new char[istLen];

							f.Seek(iHeader, CFile::begin);
							for (int i = 0; i < iCount; i++)
							{
								memset(pBuffer, 0, istLen);
								f.Read(pBuffer, istLen);
								TraversalData14_Big(pBuffer);
							}
							delete pBuffer;
							f.Close();

							CString strInfo;
							strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), m_iCount14, strFile);
							m_DataFile14.WriteString(strInfo);
						}
						else
						{
							DWORD dwError = ::GetLastError();
							TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
						}
					}
					catch(CFileException* e)
					{
						TCHAR szError[MAX_PATH_LEN];
						e->GetErrorMessage(szError, MAX_PATH_LEN);
						TRACE(_T("CFileException catched. [%s]\n"), szError);
					}
				}
				else
				{
					CFile f;
					try
					{
						if (f.Open(strFile, CFile::modeRead))
						{
							m_iCount14 = 0;
							int ilen = (int)f.GetLength();
							int iCount =(ilen - iHeader) / istLen;
							char* pBuffer = new char[istLen];				
							f.Seek(iHeader, CFile::begin);

							for (int i = 0; i < iCount; i++)
							{
								memset(pBuffer, 0, istLen);
								f.Read(pBuffer, istLen);
								TraversalData14_Little(pBuffer);
							}
							delete pBuffer;
							f.Close();

							CString strInfo;
							strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), m_iCount14, strFile);
							m_DataFile14.WriteString(strInfo);
						}
						else
						{
							DWORD dwError = ::GetLastError();
							TRACE(_T("ReadGLA14 --- open file failed.(Error:%d, path:%s)\n"), dwError, strFile);
						}
					}
					catch(CFileException* e)
					{
						TCHAR szError[MAX_PATH_LEN];
						e->GetErrorMessage(szError, MAX_PATH_LEN);
						TRACE(_T("CFileException catched. [%s]\n"), szError);
					}
				}
				ReadGLA01();
			}
		}
	}
	if (m_DataFile14.m_hFile != INVALID_HANDLE_VALUE)
		m_DataFile14.Close();

	return;
}
void GLAData::WriteLog(CString strLog)
{
	COleDateTime dt = COleDateTime::GetCurrentTime();
	CString strTime = dt.Format(_T("%Y-%m-%d %H:%M:%S : "));
	CString str;
	str.Format(_T("%s%s\n"), strTime, strLog);
	m_LogFile.WriteString(str);
}

void GLAData::TraversalData14_Little(char *pBuffer)
{
	GLA14* pData = (GLA14*)pBuffer;
	bool bMark = false;

	for (int i = 0; i < 40; i++)
	{
		int iLat = htonl(pData->i_lat[i]);
		int iLon = htonl(pData->i_lon[i]);

		if (m_iLatMin <= iLat && iLat <= m_iLatMax 
			&& m_iLonMin <= iLon && iLon <= m_iLonMax)
		{
			bMark = true;
		}
	}

	if (bMark)
	{
		if (m_DataFile14.m_hFile != INVALID_HANDLE_VALUE)
		{
			CString str;
			str.Format(_T("Record : %d\n"), m_iCount14);
			m_DataFile14.WriteString(str);
		}
		m_iCount14++;
		PrintData14_Little(pBuffer);
	}
}

void GLAData::PrintData14_Little(char* pBuffer)
{
	if (m_DataFile14.m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(_T("Output DataFile14 is invalid!"));
		return;
	}
	GLA14* pData = (GLA14*)pBuffer;
	CString strOutput;
	int iCount = 0;

	//i_rec_ndx
	strOutput.Format(_T("i_rec_ndx : %d\n"), htonl(pData->i_rec_ndx));
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	m_Search_Rec_Index.insert(htonl(pData->i_rec_ndx));

	//i_UTCTime[2];
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_UTCTime[%.2d] : %d\n"), i, htonl(pData->i_UTCTime[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_transtime
	strOutput.Format(_T("i_transtime : %d\n"), htons(pData->i_transtime));
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	//i_Spare1[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_Spare1[%.2d] : %d\n"), i, pData->i_Spare1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_deltagpstmcor
	strOutput.Format(_T("i_deltagpstmcor : %d\n"), htonl(pData->i_deltagpstmcor));
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	//i_dShortTime[39]
	for (int i = 0; i < 39; i++)
	{
		strOutput.Format(_T("i_dShortTime[%.2d] : %d\n"), i, htonl(pData->i_dShortTime[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_lat[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_lat[%.2d] : %d\n"), i, htonl(pData->i_lat[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_lon[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_lon[%.2d] : %d\n"), i, htonl(pData->i_lon[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_elev[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_elev[%.2d] : %d\n"), i, htonl(pData->i_elev[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_campaign[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_campaign[%.2d] : %d\n"), i, pData->i_campaign[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare40
	strOutput.Format(_T("i_spare40 : %d\n"), htons(pData->i_spare40));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_cycTrk
	strOutput.Format(_T("i_cycTrk : %d\n"), htonl(pData->i_cycTrk));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_localSolarTime
	strOutput.Format(_T("i_localSolarTime : %d\n"), htonl(pData->i_localSolarTime));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare41[7]
	for (int i = 0; i < 7; i++)
	{
		strOutput.Format(_T("i_spare41[%.2d] : %d\n"), i, htonl(pData->i_spare41[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_deltaEllip[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_deltaEllip[%.2d] : %d\n"), i, htons(pData->i_deltaEllip[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_beamCoelv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_beamCoelv[%.2d] : %d\n"), i, htonl(pData->i_beamCoelv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_beamAzimuth[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_beamAzimuth[%.2d] : %d\n"), i, htonl(pData->i_beamAzimuth[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_d2refTrk[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_d2refTrk[%.2d] : %d\n"), i, htonl(pData->i_d2refTrk[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_SigBegOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_SigBegOff[%.2d] : %d\n"), i, htonl(pData->i_SigBegOff[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_DEM_hires_src[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_DEM_hires_src[%.2d] : %d\n"), i, pData->i_DEM_hires_src[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_DEMhiresArelv[9][40]
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_DEMhiresArelv[%.2d][%.2d] : %d\n"), i, j, htons(pData->i_DEMhiresArelv[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_ElevBiasCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ElevBiasCorr[%.2d] : %d\n"), i, htons(pData->i_ElevBiasCorr[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare42[4][40]
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_spare42[%.2d][%.2d] : %d\n"), i, j, htons(pData->i_spare42[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_sigmaatt[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_sigmaatt[%.2d] : %d\n"), i, htons(pData->i_sigmaatt[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_Azimuth
	strOutput.Format(_T("i_Azimuth : %d\n"), htonl(pData->i_Azimuth));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_AolAng
	strOutput.Format(_T("i_AolAng : %d\n"), htonl(pData->i_AolAng));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpintensity_avg
	strOutput.Format(_T("i_tpintensity_avg : %d\n"), htonl(pData->i_tpintensity_avg));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpazimuth_avg
	strOutput.Format(_T("i_tpazimuth_avg : %d\n"), htons(pData->i_tpazimuth_avg));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpeccentricity_avg
	strOutput.Format(_T("i_tpeccentricity_avg : %d\n"), htons(pData->i_tpeccentricity_avg));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpmajoraxis_avg
	strOutput.Format(_T("i_tpmajoraxis_avg : %d\n"), htons(pData->i_tpmajoraxis_avg));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_poleTide[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_poleTide[%.2d] : %d\n"), i, pData->i_poleTide[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_gdHt[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_gdHt[%.2d] : %d\n"), i, htons(pData->i_gdHt[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_erElv[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_erElv[%.2d] : %d\n"), i, htons(pData->i_erElv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spElv[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_spElv[%.2d] : %d\n"), i, htons(pData->i_spElv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ldElv[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_ldElv[%.2d] : %d\n"), i, htons(pData->i_ldElv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare12[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_spare12[%.2d] : %d\n"), i, htons(pData->i_spare12[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_wTrop[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_wTrop[%.2d] : %d\n"), i, htons(pData->i_wTrop[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_dTrop[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_dTrop[%.2d] : %d\n"), i, htons(pData->i_dTrop[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_surfType
	strOutput.Format(_T("i_surfType : %d\n"), pData->i_surfType);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare11[3]
	for (int i = 0; i < 3; i++)
	{
		strOutput.Format(_T("i_spare11[%.2d] : %d\n"), i, pData->i_spare11[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_DEM_elv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_DEM_elv[%.2d] : %d\n"), i, htonl(pData->i_DEM_elv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_refRng[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_refRng[%.2d] : %d\n"), i, htonl(pData->i_refRng[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare47[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare47[%.2d] : %d\n"), i, htonl(pData->i_spare47[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ldRngOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ldRngOff[%.2d] : %d\n"), i, htonl(pData->i_ldRngOff[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_SigEndOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_SigEndOff[%.2d] : %d\n"), i, htonl(pData->i_SigEndOff[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_gpCntRngOff[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_gpCntRngOff[%.2d][%.2d] : %d\n"), i, j, htonl(pData->i_gpCntRngOff[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_reflctUC[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_reflctUC[%.2d] : %d\n"), i, htonl(pData->i_reflctUC[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_reflCor_atm
	strOutput.Format(_T("i_reflCor_atm : %d\n"), htonl(pData->i_reflCor_atm));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_maxSmAmp[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_maxSmAmp[%.2d] : %d\n"), i, htons(pData->i_maxSmAmp[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ocElv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ocElv[%.2d] : %d\n"), i, htons(pData->i_ocElv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_numPk[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_numPk[%.2d] : %d\n"), i, pData->i_numPk[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_kurt1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_kurt1[%.2d] : %d\n"), i, htons(pData->i_kurt1[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_skew1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_skew1[%.2d] : %d\n"), i, htons(pData->i_skew1[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare4[160]
	for (int i = 0; i < 160; i++)
	{
		strOutput.Format(_T("i_spare4[%.3d] : %d\n"), i, pData->i_spare4[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Gamp[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_Gamp[%.2d][%.2d] : %d\n"), i, j, htonl(pData->i_Gamp[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_Garea[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_Garea[%.2d][%.2d] : %d\n"), i, j, htonl(pData->i_Garea[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_gsinma[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_gsinma[%.2d][%.2d] : %d\n"), i, j, htonl(pData->i_gsinma[i][j]));
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_nPeaks1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_nPeaks1[%.2d] : %d\n"), i, pData->i_nPeaks1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_LandVar[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_LandVar[%.2d] : %d\n"), i, htons(pData->i_LandVar[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ElvuseFlg[5]
	for (int i = 0; i < 5; i++)
	{
		strOutput.Format(_T("i_ElvuseFlg[%.2d] : %d\n"), i, pData->i_ElvuseFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_atm_avail
	strOutput.Format(_T("i_atm_avail : %d\n"), pData->i_atm_avail);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare16[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_spare16[%.2d] : %d\n"), i, pData->i_spare16[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_cld1_mswf
	strOutput.Format(_T("i_cld1_mswf : %d\n"), pData->i_cld1_mswf);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_MRC_af
	strOutput.Format(_T("i_MRC_af : %d\n"), pData->i_MRC_af);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare9[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare9[%.2d] : %d\n"), pData->i_spare9[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_ElvFlg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ElvFlg[%.2d] : %d\n"), i, pData->i_ElvFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_rng_UQF[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_rng_UQF[%.2d] : %d\n"), i, htons(pData->i_rng_UQF[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare49[10]
	for (int i = 0; i < 10; i++)
	{
		strOutput.Format(_T("i_spare49[%.2d] : %d\n"), i, pData->i_spare49[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_timecorflg
	strOutput.Format(_T("i_timecorflg : %d\n"), htons(pData->i_timecorflg));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_APID_AvFlg[8]
	for (int i = 0; i < 8; i++)
	{
		strOutput.Format(_T("i_APID_AvFlg[%.2d] : %d\n"), i, pData->i_APID_AvFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_AttFlg2[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_AttFlg2[%.2d] : %d\n"), i, pData->i_AttFlg2[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare5
	strOutput.Format(_T("i_spare5 : %d\n"), pData->i_spare5);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_FrameQF
	strOutput.Format(_T("i_FrameQF : %d\n"), pData->i_FrameQF);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_OrbFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_OrbFlg[%.2d] : %d\n"), i, pData->i_OrbFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_rngCorrFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_rngCorrFlg[%.2d] : %d\n"), i, pData->i_rngCorrFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_CorrStatFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_CorrStatFlg[%.2d] : %d\n"), i, pData->i_CorrStatFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare15[8]
	for (int i = 0; i < 8; i++)
	{
		strOutput.Format(_T("i_spare15[%.2d] : %d\n"), i, pData->i_spare15[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Attflg1
	strOutput.Format(_T("i_Attflg1 : %d\n"), htons(pData->i_Attflg1));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Spare6[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_Spare6[%.2d] : %d\n"), i, pData->i_Spare6[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare44[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare44[%.3d] : %d\n"), i, pData->i_spare44[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satNdx[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satNdx[%.2d] : %d\n"), i, pData->i_satNdx[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satElevCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satElevCorr[%.2d] : %d\n"), i, htons(pData->i_satElevCorr[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_satCorrFlg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satCorrFlg[%.2d] : %d\n"), i, pData->i_satCorrFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satNrgCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satNrgCorr[%.2d] : %d\n"), i, htons(pData->i_satNrgCorr[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare13[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare13[%.2d] : %d\n"), i, htons(pData->i_spare13[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_gval_rcv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_gval_rcv[%.2d] : %d\n"), i, htons(pData->i_gval_rcv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_RecNrgAll[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_RecNrgAll[%.2d] : %d\n"), i, htons(pData->i_RecNrgAll[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_FRir_cldtop[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_cldtop[%.2d] : %d\n"), i, htons(pData->i_FRir_cldtop[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_FRir_gaFlag[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_gaFlag[%.2d] : %d\n"), i, pData->i_FRir_gaFlag[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_atm_char_flag
	strOutput.Format(_T("i_atm_char_flag : %d\n"), htons(pData->i_atm_char_flag));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_atm_char_conf
	strOutput.Format(_T("i_atm_char_conf : %d\n"), htons(pData->i_atm_char_conf));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare48[36]
	for (int i = 0; i < 36; i++)
	{
		strOutput.Format(_T("i_spare48[%.2d] : %d\n"), i, pData->i_spare48[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_FRir_intsig[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_intsig[%.2d] : %d\n"), i, htons(pData->i_FRir_intsig[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare14[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare14[%.3d] : %d\n"), i, pData->i_spare14[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Surface_temp
	strOutput.Format(_T("i_Surface_temp : %d\n"), htons(pData->i_Surface_temp));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Surface_pres
	strOutput.Format(_T("i_Surface_pres : %d\n"), htons(pData->i_Surface_pres));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Surface_relh
	strOutput.Format(_T("i_Surface_relh : %d\n"), htons(pData->i_Surface_relh));
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_maxRecAmp[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_maxRecAmp[%.2d] : %d\n"), i, htons(pData->i_maxRecAmp[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_sDevNsOb1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_sDevNsOb1[%.2d] : %d\n"), i, htons(pData->i_sDevNsOb1[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare8[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_spare8[%.2d] : %d\n"), i, pData->i_spare8[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_isRngOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_isRngOff[%.2d] : %d\n"), i, htonl(pData->i_isRngOff[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_pctSAT[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_pctSAT[%.2d] : %d\n"), i, pData->i_pctSAT[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_TxNrg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_TxNrg[%.2d] : %d\n"), i, htons(pData->i_TxNrg[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_eqElv[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_eqElv[%.2d] : %d\n"), i, htons(pData->i_eqElv[i]));
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare7[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare7[%.3d] : %d\n"), i, pData->i_spare7[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	m_DataFile14.WriteString("\n------------------------------------------------------------\n\n");
	ASSERT(iCount == 105);
}

void GLAData::TraversalData01_Little(char *pBuffer)
{
	// main=1, long=2, or short=3
	GLA01_SHORT* pData = (GLA01_SHORT*)pBuffer;
	short iType = htons(pData->i_gla01_rectype);

	if (iType == 3)
	{
		// short
		
		int iIndex = htonl(pData->i_rec_ndx);

		std::set<int>::iterator iter = m_Search_Rec_Index.find(iIndex);
		if (iter != m_Search_Rec_Index.end())
		{
			if (m_DataFile01.m_hFile != INVALID_HANDLE_VALUE)
			{
				CString str;
				str.Format(_T("Record : %d\n"), m_iCount01);
				m_DataFile01.WriteString(str);
			}
			m_iCount01++;
			PrintData01_Little(pBuffer);
		}
	}
}

void GLAData::PrintData01_Little(char* pBuffer)
{
	if (m_DataFile01.m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(_T("Output DataFile01 is invalid!"));
		return;
	}
	GLA01_SHORT* pData = (GLA01_SHORT*)pBuffer;
	CString strOutput;
	int iCount = 0;

	//i_rec_ndx
	strOutput.Format(_T("i_rec_ndx : %d\n"), htonl(pData->i_rec_ndx));
	m_DataFile01.WriteString(strOutput);
	iCount ++;

	//i_UTCTime[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_UTCTime[%.2d] : %d\n"), i, htonl(pData->i_UTCTime[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_gla01_rectype
	strOutput.Format(_T("i_gla01_rectype : %d\n"), htons(pData->i_gla01_rectype));
	m_DataFile01.WriteString(strOutput);
	iCount++;

	//i_spare1
	strOutput.Format(_T("i_spare1 : %d\n"), htons(pData->i_spare1));
	m_DataFile01.WriteString(strOutput);
	iCount++;

	//i_filtnum[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_filtnum[%.2d] : %d\n"), i, pData->i_filtnum[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_shot_ctr[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_shot_ctr[%.2d] : %d\n"), i, htons(pData->i_shot_ctr[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//istatflags[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("istatflags[%.2d] : %d\n"), i, htonl(pData->istatflags[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_gainSet1064[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_gainSet1064[%.2d] : %d\n"), i, htons(pData->i_gainSet1064[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsPeakVal[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsPeakVal[%.2d] : %d\n"), i, htons(pData->i_4nsPeakVal[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_8nsPeakVal[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_8nsPeakVal[%.2d] : %d\n"), i, htons(pData->i_8nsPeakVal[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsBgMead[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsBgMead[%.2d] : %d\n"), i, (unsigned short)(htons(pData->i_4nsBgMead[i])));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsBgSDEV[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsBgSDEV[%.2d] : %d\n"), i, (unsigned short)(htons(pData->i_4nsBgSDEV[i])));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_samp_pad[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_samp_pad[%.2d] : %d\n"), i, htons(pData->i_samp_pad[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_comp_type[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_comp_type[%.2d] : %d\n"), i, pData->i_comp_type[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_rng_wf[200][20]
	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			strOutput.Format(_T("i_rng_wf[%.3d][%.2d] : %d\n"), i, j, pData->i_rng_wf[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_gainStatus[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_gainStatus[%.2d] : %d\n"), i, pData->i_gainStatus[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_NumCoinc[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_NumCoinc[%.2d] : %d\n"), i, pData->i_NumCoinc[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_rawPkHt[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_rawPkHt[%.2d] : %d\n"), i, pData->i_rawPkHt[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_spare2[184]
	for (int i = 0; i < 184; i++)
	{
		strOutput.Format(_T("i_spare2[%.2d] : %d\n"), i, pData->i_spare2[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	m_DataFile01.WriteString("\n------------------------------------------------------------\n\n");
	ASSERT(iCount == 19);
}

void GLAData::TraversalData14_Big(char *pBuffer)
{
	GLA14* pData = (GLA14*)pBuffer;
	bool bMark = false;

	for (int i = 0; i < 40; i++)
	{
		int iLat = pData->i_lat[i];
		int iLon = pData->i_lon[i];

		if (m_iLatMin <= iLat && iLat <= m_iLatMax 
			&& m_iLonMin <= iLon && iLon <= m_iLonMax)
		{
			bMark = true;
		}
	}

	if (bMark)
	{
		if (m_DataFile14.m_hFile != INVALID_HANDLE_VALUE)
		{
			CString str;
			str.Format(_T("Record : %d\n"), m_iCount14);
			m_DataFile14.WriteString(str);
		}
		m_iCount14++;
		PrintData14_Big(pBuffer);
	}
}
void GLAData::PrintData14_Big(char* pBuffer)
{
	if (m_DataFile14.m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(_T("Output DataFile14 is invalid!"));
		return;
	}
	GLA14* pData = (GLA14*)pBuffer;
	CString strOutput;
	int iCount = 0;

	//i_rec_ndx
	strOutput.Format(_T("i_rec_ndx : %d\n"), pData->i_rec_ndx);
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	m_Search_Rec_Index.insert(pData->i_rec_ndx);

	//i_UTCTime[2];
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_UTCTime[%.2d] : %d\n"), i, pData->i_UTCTime[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_transtime
	strOutput.Format(_T("i_transtime : %d\n"), pData->i_transtime);
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	//i_Spare1[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_Spare1[%.2d] : %d\n"), i, pData->i_Spare1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_deltagpstmcor
	strOutput.Format(_T("i_deltagpstmcor : %d\n"), pData->i_deltagpstmcor);
	m_DataFile14.WriteString(strOutput);
	iCount ++;

	//i_dShortTime[39]
	for (int i = 0; i < 39; i++)
	{
		strOutput.Format(_T("i_dShortTime[%.2d] : %d\n"), i, pData->i_dShortTime[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_lat[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_lat[%.2d] : %d\n"), i, pData->i_lat[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_lon[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_lon[%.2d] : %d\n"), i, pData->i_lon[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_elev[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_elev[%.2d] : %d\n"), i, pData->i_elev[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_campaign[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_campaign[%.2d] : %d\n"), i, pData->i_campaign[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare40
	strOutput.Format(_T("i_spare40 : %d\n"), pData->i_spare40);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_cycTrk
	strOutput.Format(_T("i_cycTrk : %d\n"), pData->i_cycTrk);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_localSolarTime
	strOutput.Format(_T("i_localSolarTime : %d\n"), pData->i_localSolarTime);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare41[7]
	for (int i = 0; i < 7; i++)
	{
		strOutput.Format(_T("i_spare41[%.2d] : %d\n"), i, pData->i_spare41[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_deltaEllip[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_deltaEllip[%.2d] : %d\n"), i, pData->i_deltaEllip[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_beamCoelv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_beamCoelv[%.2d] : %d\n"), i, pData->i_beamCoelv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_beamAzimuth[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_beamAzimuth[%.2d] : %d\n"), i, pData->i_beamAzimuth[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_d2refTrk[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_d2refTrk[%.2d] : %d\n"), i, pData->i_d2refTrk[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_SigBegOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_SigBegOff[%.2d] : %d\n"), i, pData->i_SigBegOff[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_DEM_hires_src[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_DEM_hires_src[%.2d] : %d\n"), i, pData->i_DEM_hires_src[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_DEMhiresArelv[9][40]
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_DEMhiresArelv[%.2d][%.2d] : %d\n"), i, j, pData->i_DEMhiresArelv[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_ElevBiasCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ElevBiasCorr[%.2d] : %d\n"), i, pData->i_ElevBiasCorr[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare42[4][40]
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_spare42[%.2d][%.2d] : %d\n"), i, j, pData->i_spare42[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_sigmaatt[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_sigmaatt[%.2d] : %d\n"), i, pData->i_sigmaatt[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_Azimuth
	strOutput.Format(_T("i_Azimuth : %d\n"), pData->i_Azimuth);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_AolAng
	strOutput.Format(_T("i_AolAng : %d\n"), pData->i_AolAng);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpintensity_avg
	strOutput.Format(_T("i_tpintensity_avg : %d\n"), pData->i_tpintensity_avg);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpazimuth_avg
	strOutput.Format(_T("i_tpazimuth_avg : %d\n"), pData->i_tpazimuth_avg);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpeccentricity_avg
	strOutput.Format(_T("i_tpeccentricity_avg : %d\n"), pData->i_tpeccentricity_avg);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_tpmajoraxis_avg
	strOutput.Format(_T("i_tpmajoraxis_avg : %d\n"), pData->i_tpmajoraxis_avg);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_poleTide[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_poleTide[%.2d] : %d\n"), i, pData->i_poleTide[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_gdHt[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_gdHt[%.2d] : %d\n"), i, pData->i_gdHt[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_erElv[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_erElv[%.2d] : %d\n"), i, pData->i_erElv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spElv[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_spElv[%.2d] : %d\n"), i, pData->i_spElv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ldElv[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_ldElv[%.2d] : %d\n"), i, pData->i_ldElv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare12[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_spare12[%.2d] : %d\n"), i, pData->i_spare12[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_wTrop[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_wTrop[%.2d] : %d\n"), i, pData->i_wTrop[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_dTrop[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_dTrop[%.2d] : %d\n"), i, pData->i_dTrop[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_surfType
	strOutput.Format(_T("i_surfType : %d\n"), pData->i_surfType);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare11[3]
	for (int i = 0; i < 3; i++)
	{
		strOutput.Format(_T("i_spare11[%.2d] : %d\n"), i, pData->i_spare11[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_DEM_elv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_DEM_elv[%.2d] : %d\n"), i, pData->i_DEM_elv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_refRng[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_refRng[%.2d] : %d\n"), i, pData->i_refRng[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare47[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare47[%.2d] : %d\n"), i, pData->i_spare47[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ldRngOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ldRngOff[%.2d] : %d\n"), i, pData->i_ldRngOff[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_SigEndOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_SigEndOff[%.2d] : %d\n"), i, pData->i_SigEndOff[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_gpCntRngOff[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_gpCntRngOff[%.2d][%.2d] : %d\n"), i, j, pData->i_gpCntRngOff[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_reflctUC[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_reflctUC[%.2d] : %d\n"), i, pData->i_reflctUC[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_reflCor_atm
	strOutput.Format(_T("i_reflCor_atm : %d\n"), pData->i_reflCor_atm);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_maxSmAmp[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_maxSmAmp[%.2d] : %d\n"), i, pData->i_maxSmAmp[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ocElv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ocElv[%.2d] : %d\n"), i, pData->i_ocElv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_numPk[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_numPk[%.2d] : %d\n"), i, pData->i_numPk[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_kurt1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_kurt1[%.2d] : %d\n"), i, pData->i_kurt1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_skew1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_skew1[%.2d] : %d\n"), i, pData->i_skew1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare4[160]
	for (int i = 0; i < 160; i++)
	{
		strOutput.Format(_T("i_spare4[%.3d] : %d\n"), i, pData->i_spare4[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Gamp[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_Gamp[%.2d][%.2d] : %d\n"), i, j, pData->i_Gamp[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_Garea[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_Garea[%.2d][%.2d] : %d\n"), i, j, pData->i_Garea[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_gsinma[6][40]
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			strOutput.Format(_T("i_gsinma[%.2d][%.2d] : %d\n"), i, j, pData->i_gsinma[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_nPeaks1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_nPeaks1[%.2d] : %d\n"), i, pData->i_nPeaks1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_LandVar[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_LandVar[%.2d] : %d\n"), i, pData->i_LandVar[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_ElvuseFlg[5]
	for (int i = 0; i < 5; i++)
	{
		strOutput.Format(_T("i_ElvuseFlg[%.2d] : %d\n"), i, pData->i_ElvuseFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_atm_avail
	strOutput.Format(_T("i_atm_avail : %d\n"), pData->i_atm_avail);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare16[4]
	for (int i = 0; i < 4; i++)
	{
		strOutput.Format(_T("i_spare16[%.2d] : %d\n"), i, pData->i_spare16[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_cld1_mswf
	strOutput.Format(_T("i_cld1_mswf : %d\n"), pData->i_cld1_mswf);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_MRC_af
	strOutput.Format(_T("i_MRC_af : %d\n"), pData->i_MRC_af);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare9[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare9[%.2d] : %d\n"), pData->i_spare9[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_ElvFlg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_ElvFlg[%.2d] : %d\n"), i, pData->i_ElvFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_rng_UQF[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_rng_UQF[%.2d] : %d\n"), i, pData->i_rng_UQF[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare49[10]
	for (int i = 0; i < 10; i++)
	{
		strOutput.Format(_T("i_spare49[%.2d] : %d\n"), i, pData->i_spare49[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_timecorflg
	strOutput.Format(_T("i_timecorflg : %d\n"), pData->i_timecorflg);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_APID_AvFlg[8]
	for (int i = 0; i < 8; i++)
	{
		strOutput.Format(_T("i_APID_AvFlg[%.2d] : %d\n"), i, pData->i_APID_AvFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_AttFlg2[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_AttFlg2[%.2d] : %d\n"), i, pData->i_AttFlg2[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare5
	strOutput.Format(_T("i_spare5 : %d\n"), pData->i_spare5);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_FrameQF
	strOutput.Format(_T("i_FrameQF : %d\n"), pData->i_FrameQF);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_OrbFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_OrbFlg[%.2d] : %d\n"), i, pData->i_OrbFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_rngCorrFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_rngCorrFlg[%.2d] : %d\n"), i, pData->i_rngCorrFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_CorrStatFlg[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_CorrStatFlg[%.2d] : %d\n"), i, pData->i_CorrStatFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare15[8]
	for (int i = 0; i < 8; i++)
	{
		strOutput.Format(_T("i_spare15[%.2d] : %d\n"), i, pData->i_spare15[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Attflg1
	strOutput.Format(_T("i_Attflg1 : %d\n"), pData->i_Attflg1);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Spare6[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_Spare6[%.2d] : %d\n"), i, pData->i_Spare6[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_spare44[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare44[%.3d] : %d\n"), i, pData->i_spare44[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satNdx[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satNdx[%.2d] : %d\n"), i, pData->i_satNdx[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satElevCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satElevCorr[%.2d] : %d\n"), i, pData->i_satElevCorr[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_satCorrFlg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satCorrFlg[%.2d] : %d\n"), i, pData->i_satCorrFlg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_satNrgCorr[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_satNrgCorr[%.2d] : %d\n"), i, pData->i_satNrgCorr[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare13[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_spare13[%.2d] : %d\n"), i, pData->i_spare13[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_gval_rcv[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_gval_rcv[%.2d] : %d\n"), i, pData->i_gval_rcv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_RecNrgAll[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_RecNrgAll[%.2d] : %d\n"), i, pData->i_RecNrgAll[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_FRir_cldtop[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_cldtop[%.2d] : %d\n"), i, pData->i_FRir_cldtop[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_FRir_gaFlag[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_gaFlag[%.2d] : %d\n"), i, pData->i_FRir_gaFlag[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_atm_char_flag
	strOutput.Format(_T("i_atm_char_flag : %d\n"), pData->i_atm_char_flag);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_atm_char_conf
	strOutput.Format(_T("i_atm_char_conf : %d\n"), pData->i_atm_char_conf);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_spare48[36]
	for (int i = 0; i < 36; i++)
	{
		strOutput.Format(_T("i_spare48[%.2d] : %d\n"), i, pData->i_spare48[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_FRir_intsig[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_FRir_intsig[%.2d] : %d\n"), i, pData->i_FRir_intsig[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare14[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare14[%.3d] : %d\n"), i, pData->i_spare14[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_Surface_temp
	strOutput.Format(_T("i_Surface_temp : %d\n"), pData->i_Surface_temp);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Surface_pres
	strOutput.Format(_T("i_Surface_pres : %d\n"), pData->i_Surface_pres);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_Surface_relh
	strOutput.Format(_T("i_Surface_relh : %d\n"), pData->i_Surface_relh);
	m_DataFile14.WriteString(strOutput);
	iCount++;

	//i_maxRecAmp[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_maxRecAmp[%.2d] : %d\n"), i, pData->i_maxRecAmp[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_sDevNsOb1[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_sDevNsOb1[%.2d] : %d\n"), i, pData->i_sDevNsOb1[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare8[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_spare8[%.2d] : %d\n"), i, pData->i_spare8[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_isRngOff[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_isRngOff[%.2d] : %d\n"), i, pData->i_isRngOff[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_pctSAT[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_pctSAT[%.2d] : %d\n"), i, pData->i_pctSAT[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	//i_TxNrg[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_TxNrg[%.2d] : %d\n"), i, pData->i_TxNrg[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_eqElv[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_eqElv[%.2d] : %d\n"), i, pData->i_eqElv[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount++;

	//i_spare7[120]
	for (int i = 0; i < 120; i++)
	{
		strOutput.Format(_T("i_spare7[%.3d] : %d\n"), i, pData->i_spare7[i]);
		m_DataFile14.WriteString(strOutput);
	}
	iCount ++;

	m_DataFile14.WriteString("\n------------------------------------------------------------\n\n");
	ASSERT(iCount == 105);
}

void GLAData::TraversalData01_Big(char *pBuffer)
{
	// main=1, long=2, or short=3
	GLA01_SHORT* pData = (GLA01_SHORT*)pBuffer;
	short iType = htons(pData->i_gla01_rectype);

	if (iType == 3)
	{
		// short
		int iIndex = pData->i_rec_ndx;

		std::set<int>::iterator iter = m_Search_Rec_Index.find(iIndex);
		if (iter != m_Search_Rec_Index.end())
		{
			if (m_DataFile01.m_hFile != INVALID_HANDLE_VALUE)
			{
				CString str;
				str.Format(_T("Record : %d\n"), m_iCount01);
				m_DataFile01.WriteString(str);
			}
			m_iCount01++;
			PrintData01_Big(pBuffer);
		}
	}
}

void GLAData::PrintData01_Big(char* pBuffer)
{
	if (m_DataFile01.m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(_T("Output DataFile01 is invalid!"));
		return;
	}
	GLA01_SHORT* pData = (GLA01_SHORT*)pBuffer;
	CString strOutput;
	int iCount = 0;

	//i_rec_ndx
	strOutput.Format(_T("i_rec_ndx : %d\n"), pData->i_rec_ndx);
	m_DataFile01.WriteString(strOutput);
	iCount ++;

	//i_UTCTime[2]
	for (int i = 0; i < 2; i++)
	{
		strOutput.Format(_T("i_UTCTime[%.2d] : %d\n"), i, pData->i_UTCTime[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_gla01_rectype
	strOutput.Format(_T("i_gla01_rectype : %d\n"), pData->i_gla01_rectype);
	m_DataFile01.WriteString(strOutput);
	iCount++;

	//i_spare1
	strOutput.Format(_T("i_spare1 : %d\n"), pData->i_spare1);
	m_DataFile01.WriteString(strOutput);
	iCount++;

	//i_filtnum[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_filtnum[%.2d] : %d\n"), i, pData->i_filtnum[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_shot_ctr[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_shot_ctr[%.2d] : %d\n"), i, pData->i_shot_ctr[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//istatflags[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("istatflags[%.2d] : %d\n"), i, pData->istatflags[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_gainSet1064[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_gainSet1064[%.2d] : %d\n"), i, pData->i_gainSet1064[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsPeakVal[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsPeakVal[%.2d] : %d\n"), i, pData->i_4nsPeakVal[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_8nsPeakVal[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_8nsPeakVal[%.2d] : %d\n"), i, pData->i_8nsPeakVal[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsBgMead[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsBgMead[%.2d] : %d\n"), i, (unsigned short)(pData->i_4nsBgMead[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_4nsBgSDEV[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_4nsBgSDEV[%.2d] : %d\n"), i, (unsigned short)(pData->i_4nsBgSDEV[i]));
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_samp_pad[40]
	for (int i = 0; i < 40; i++)
	{
		strOutput.Format(_T("i_samp_pad[%.2d] : %d\n"), i, pData->i_samp_pad[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount++;

	//i_comp_type[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_comp_type[%.2d] : %d\n"), i, pData->i_comp_type[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_rng_wf[200][20]
	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			strOutput.Format(_T("i_rng_wf[%.3d][%.2d] : %d\n"), i, j, pData->i_rng_wf[i][j]);
			m_DataFile14.WriteString(strOutput);
		}
	}
	iCount++;

	//i_gainStatus[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_gainStatus[%.2d] : %d\n"), i, pData->i_gainStatus[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_NumCoinc[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_NumCoinc[%.2d] : %d\n"), i, pData->i_NumCoinc[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_rawPkHt[20]
	for (int i = 0; i < 20; i++)
	{
		strOutput.Format(_T("i_rawPkHt[%.2d] : %d\n"), i, pData->i_rawPkHt[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	//i_spare2[184]
	for (int i = 0; i < 184; i++)
	{
		strOutput.Format(_T("i_spare2[%.2d] : %d\n"), i, pData->i_spare2[i]);
		m_DataFile01.WriteString(strOutput);
	}
	iCount ++;

	m_DataFile01.WriteString("\n------------------------------------------------------------\n\n");
	ASSERT(iCount == 19);
}