// calGLCMDlg.h : header file
//

#pragma once
#include "GLCMData.h"

// CcalGLCMDlg dialog
class CcalGLCMDlg : public CDialog
{
// Construction
public:
	CcalGLCMDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CALGLCM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonChooseFolder();
	CString m_strPath;
	afx_msg void OnBnClickedButtonCalc();
	CalCondition m_con;
	CStdioFile m_Output;
};
