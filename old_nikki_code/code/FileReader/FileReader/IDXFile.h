#pragma once
#include "structdefine.h"
class CIDXFile
{
public:
	CIDXFile(LPCTSTR lpNDFFilePath = NULL);
	~CIDXFile(void);
	void ParseFile();
protected:
	CString m_strIDXFilePath;
};
