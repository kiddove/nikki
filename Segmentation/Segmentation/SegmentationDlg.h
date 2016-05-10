// SegmetationDlg.h : header file
//

#pragma once
#include "Datafile.h"
#include "StatisticDlg.h"

// CSegmentationDlg dialog
class CSegmentationDlg : public CDialog
{
// Construction
public:
	CSegmentationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SEGMENTATION_DIALOG };

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
	CString m_strMAskFile;

	std::vector<cv::Point> m_vecSeed;
	std::map<int, std::set<int>> m_mapMask;
	Datafile m_datafile;

	segCondition m_con;
	CToolTipCtrl m_tooltip;
	TraceInfo m_traceinfo;

	void SetCheck(int iCtrl);
	void UnCkeck(int iCtrl);

	void SetTipInfo();
	CStatisticDlg* m_pDlg;
public:
	afx_msg void OnBnClickedButtonImgFile();
	afx_msg void OnBnClickedButtonLoadSeed();
	afx_msg void OnBnClickedButtonLoadImg();
	afx_msg void OnBnClickedButtonProcess();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonMaskFile();
	afx_msg void OnBnClickedButtonLoadMask();
	afx_msg void OnBnClickedButtonPreprocess();
	afx_msg void OnBnClickedButtonStatistic();
};
