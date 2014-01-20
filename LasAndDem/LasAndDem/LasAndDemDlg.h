// LasAndDemDlg.h : header file
//

#pragma once
#include "LASData.h"

// CLasAndDemDlg dialog
class CLasAndDemDlg : public CDialog
{
// Construction
public:
	CLasAndDemDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LASANDDEM_DIALOG };

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

	LASData m_Data;
public:
	afx_msg void OnBnClickedButtonReadLas();
	afx_msg void OnBnClickedButtonReadDem();
	afx_msg void OnBnClickedButtonGenAdj();
	afx_msg void OnBnClickedButtonLoadAdj();
	afx_msg void OnBnClickedButtonCalc();
};
