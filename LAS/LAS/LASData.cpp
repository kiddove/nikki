#include "StdAfx.h"
#include "LASData.h"

LASData::LASData(void)
{
	ASSERT( 243 == sizeof(PUBLIC_HEADER_BLOCK) );
	ASSERT( 54 == sizeof(VARIABLE_LENGTH_RECORDS) );
	ASSERT( 20 == sizeof(POINT_DATA_RECORD_0) );
	ASSERT( 28 == sizeof(POINT_DATA_RECORD_1) );
	ASSERT( 26 == sizeof(POINT_DATA_RECORD_2) );
	ASSERT( 34 == sizeof(POINT_DATA_RECORD_3) );
	ASSERT( 57 == sizeof(POINT_DATA_RECORD_4) );
	ASSERT( 63 == sizeof(POINT_DATA_RECORD_5) );
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
	// open output file
	//try
	//{
	//	if (!m_LASDataFile.Open("./SearchResult.txt", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
	//	{
	//		DWORD dwError = ::GetLastError();
	//		//TRACE(_T("open OutputData14 file failed.(Error:%d)\n"), dwError);
	//		CString strErr;
	//		strErr.Format(_T("open SearchResult file failed.(Error:%d)\n"), dwError);
	//		WriteLog(strErr);
	//	}
	//}
	//catch(CFileException* e)
	//{
	//	TCHAR szError[MAX_PATH_LEN];
	//	e->GetErrorMessage(szError, MAX_PATH_LEN);
	//	//TRACE(_T("CFileException catched when open OutputData14 file. [%s]\n"), szError);
	//	CString strErr;
	//	strErr.Format(_T("CFileException catched when open SearchResult file. [%s]\n"), szError);
	//	WriteLog(strErr);
	//}
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
							PUBLIC_HEADER_BLOCK phb;
							memset(&phb, 0, sizeof(PUBLIC_HEADER_BLOCK));
							f.Read(&phb, sizeof(PUBLIC_HEADER_BLOCK));
							//phb.iPoint_Data_Format_ID
							//phb.iPoint_Data_Record_Length
							int iLen = GetStructLength(phb.iPoint_Data_Format_ID);
							ASSERT(iLen == phb.iPoint_Data_Record_Length);
							char *pBuffer = new char[iLen];

							//if (phb.iNumber_of_Varable_Length_Records)
							f.Seek(phb.iOffset_to_point_data, CFile::begin);
							ASSERT(f.GetLength() == phb.iOffset_to_point_data + phb.iNumber_of_point_records * phb.iPoint_Data_Record_Length);

							CString strInfo;
							strInfo.Format(_T("Return = %d X Y Z\n"), m_iReturn_Number);
							m_LASDataFile.WriteString(strInfo);
							for (UINT i = 0; i < phb.iNumber_of_point_records; i++)
							//for (UINT i = 0; i < 10; i++)
							{
								memset(pBuffer, 0, iLen);
								f.Read(pBuffer, iLen);

								CPOINT_DATA_RECORD pdr(pBuffer, phb.iPoint_Data_Format_ID);

								// search & print
								if (IsValid(&pdr))
								{
									iCount++;
									Output(&pdr, &phb);
								}
							}
							delete pBuffer;
							f.Close();
							//CString strInfo;
							//strInfo.Format(_T("\ntotal %d record in file : %s\n\n\n"), iCount, strFile);
							//m_LASDataFile.WriteString(strInfo);
							//TRACE(_T("\n"));
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

				if (m_LASDataFile.m_hFile != INVALID_HANDLE_VALUE)
					m_LASDataFile.Close();
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
	if (pData->m_pData->Return_Number == m_iReturn_Number)
		return true;
	return false;
}

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