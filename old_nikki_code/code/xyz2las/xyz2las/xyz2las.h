// xyz2las.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cxyz2lasApp:
// �йش����ʵ�֣������ xyz2las.cpp
//

class Cxyz2lasApp : public CWinApp
{
public:
	Cxyz2lasApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cxyz2lasApp theApp;