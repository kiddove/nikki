// LasReaderDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "LasFile.h"

// CLasReaderDlg 对话框
class CLasReaderDlg : public CDialog
{
// 构造
public:
	CLasReaderDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LASREADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonXyz();
protected:
	CProgressCtrl m_ProgressCtrl;

	void xyz2las(CStdioFile* pstdfile,CLasFile* plasfile);
};
