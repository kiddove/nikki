#pragma once
#include "structdefine.h"
class CCSDFile
{
public:
	CCSDFile(LPCTSTR lpCSDFilePath = NULL,LPCTSTR lpTXTFilePath = NULL);
	~CCSDFile(void);
protected:
	CString m_strCSDFilePath;
	CString m_strTXTFilePath;
public:
	bool ParseFile();
	bool WriteHeader(CStdioFile& txtfile,CSDHeader& csdheader);
	bool WriteData(CStdioFile& txtfile,CSDData& csddata);
};
