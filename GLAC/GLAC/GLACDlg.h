
// GLACDlg.h : header file
//

#pragma once
#include "GLAData.h"

// CGLACDlg dialog
class CGLACDlg : public CDialog
{
// Construction
public:
	CGLACDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GLAC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//void testPrint(const char* format, ...);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	GLAData m_Data;
public:
	afx_msg void OnBnClickedButtonRead();
};
