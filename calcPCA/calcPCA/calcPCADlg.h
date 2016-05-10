
// calcPCADlg.h : header file
//
#include "ImageReader.h"
#pragma once


// CcalcPCADlg dialog
class CcalcPCADlg : public CDialogEx
{
// Construction
public:
	CcalcPCADlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CALCPCA_DIALOG };

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
	afx_msg void OnBnClickedButtonChoose();
	afx_msg void OnBnClickedButtonCalculate();

private:
	CString m_strFolder;

	void processFile(CString strFile);
	void OutputFile(CString strFile, cv::Mat& result, GDALDataset* pDataset);

	unsigned short GetValueU16(cv::Mat& mat, int r, int c, int b);
	void SetValueU16(cv::Mat& mat, int r, int c, int b, unsigned short& fValue);

	cv::Mat ReshapeByOpenCV(cv::Mat& mat);
	cv::Mat ReshapeByMe(cv::Mat& mat);
	void outputMatrix(std::string name, cv::Mat& mat);
	void FillBackZero(cv::Mat& tobeDone, cv::Mat& shouldBe);
};
