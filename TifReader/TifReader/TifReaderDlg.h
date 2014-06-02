// TifReaderDlg.h : header file
//
#include <vector>

#pragma once

// CTifReaderDlg dialog
class CTifReaderDlg : public CDialog
{
// Construction
public:
	CTifReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TIFREADER_DIALOG };

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

	CString m_strPath;

	void ReadSingleFile1(CString strFile, int margin);
	void ReadSingleFile2(CString strFile, int interval, int margin);
	void ReadSingleFile3(CString strFile);

	void ReadSingleFile4(CString strFile);

	std::vector<CString> m_vecFile;
	int m_iStandsize;

	int m_iBlocksize;

	std::vector<std::vector<float>> m_vecValue;
public:
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonAvg();
	afx_msg void OnBnClickedButtonPer();
};
