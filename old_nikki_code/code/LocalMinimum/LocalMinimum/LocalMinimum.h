// LocalMinimum.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLocalMinimumApp:
// �йش����ʵ�֣������ LocalMinimum.cpp
//

class CLocalMinimumApp : public CWinApp
{
public:
	CLocalMinimumApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLocalMinimumApp theApp;