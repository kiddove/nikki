#pragma once
#include "IDXFile.h"
class CNDFFile
{
public:
	CNDFFile(LPCTSTR lpNDFFilePath = NULL);
	~CNDFFile(void);
protected:
	CString m_strNDFFilePath;
	CString m_strIDXFilePath;
	CString m_strTXTFilePath;
	IDXHeader m_idxheader;
	IDXRecord m_idxrecord;
public:
	void ParseFile();
};
