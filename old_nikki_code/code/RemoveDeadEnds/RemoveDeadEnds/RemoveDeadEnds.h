// RemoveDeadEnds.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoveDeadEndsApp:
// �йش����ʵ�֣������ RemoveDeadEnds.cpp
//

class CRemoveDeadEndsApp : public CWinApp
{
public:
	CRemoveDeadEndsApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRemoveDeadEndsApp theApp;