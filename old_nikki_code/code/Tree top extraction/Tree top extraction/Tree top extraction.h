// Tree top extraction.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTreetopextractionApp:
// �йش����ʵ�֣������ Tree top extraction.cpp
//

class CTreetopextractionApp : public CWinApp
{
public:
	CTreetopextractionApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTreetopextractionApp theApp;