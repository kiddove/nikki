// LasReader.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLasReaderApp:
// �йش����ʵ�֣������ LasReader.cpp
//

class CLasReaderApp : public CWinApp
{
public:
	CLasReaderApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLasReaderApp theApp;