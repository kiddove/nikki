#include "stdafx.h"
#include "Log2File.h"


CLog2File::CLog2File(void)
{
	m_bCreated = false;
}


CLog2File::~CLog2File(void)
{
	if (m_bCreated)
		m_LogFile.Close();
}

CLog2File* CLog2File::GetInstance()
{
	static CLog2File _Inst;
	return &_Inst;
}

bool CLog2File::CreateLogFile()
{
	if (m_bCreated)
		return true;

	TCHAR szFilename[256] = {0};
	::GetModuleFileName(NULL, szFilename, 255);
	CString str(szFilename);
	if (str.GetLength() > 0)
	{
		int iPos = str.ReverseFind(_T('.'));
		if (iPos > -1)
		{
			str = str.Left(iPos);
			str += _T(".log");

			if (m_LogFile.Open(str, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareDenyNone))
			{
				m_bCreated = true;
				m_LogFile.SeekToEnd();
				return true;
			}
		}
	}
	return false;
}
void CLog2File::WriteLog(CString strLog)
{
	if (!m_bCreated)
		return;

	//CString strTime;
	//COleDateTime dt = COleDateTime::GetCurrentTime();
	//strTime = dt.Format(_T("%Y-%m-%d %H:%M:%S --- "));
	//
	//strLog = strTime + strLog;
	m_LogFile.WriteString(strLog);
	m_LogFile.Flush();
}
