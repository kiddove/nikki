// SegmetationDlg.h : header file
//

#pragma once
#include "Datafile.h"


// CSegmetationDlg dialog
class CSegmetationDlg : public CDialog
{
// Construction
public:
	CSegmetationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SEGMETATION_DIALOG };

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
	afx_msg void OnBnClickedButtonSeedFile();

private:
	CString m_strSeedFile;
	CString m_strImgFile;

	std::vector<Pixel> m_vecSeed;
	Datafile m_datafile;

	segCondition m_con;
public:
	afx_msg void OnBnClickedButtonImgFile();
	afx_msg void OnBnClickedButtonLoadSeed();
	afx_msg void OnBnClickedButtonLoadImg();
	afx_msg void OnBnClickedButtonProcess();
};
