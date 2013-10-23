#include "StdAfx.h"
#include "IDXFile.h"

CIDXFile::CIDXFile(LPCTSTR lpNDFFilePath /* = NULL */)
{
	m_strIDXFilePath = lpNDFFilePath;
}

CIDXFile::~CIDXFile(void)
{
}

void CIDXFile::ParseFile()
{
	TRACE("%d---%d----%d",sizeof(IDXHeader),sizeof(IDXRecord),sizeof(time_t));

}