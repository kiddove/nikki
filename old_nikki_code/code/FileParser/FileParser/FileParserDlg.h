// FileParserDlg.h : 头文件
//

#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include "afxcmn.h"

// CFileParserDlg 对话框
class CFileParserDlg : public CDialog
{
// 构造
public:
	CFileParserDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILEPARSER_DIALOG };

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
	// afx_msg void OnBnClickedButtonOpenLas();
	afx_msg void OnBnClickedButtonOpenNdf();
	afx_msg void OnBnClickedButtonAddTime();
private:
	// CString m_strLasPath;
	CString m_strNdfPath;
	CString m_strIdxPath;
	//CString m_strCsdPath;
	CString m_strTimePath;
public:
	//afx_msg void OnBnClickedButtonCsd();
public:
	int SearchInVec(std::vector<double>& timevec,double dmintime,double dmaxtime,int& iStartPos);
	// Progress Control
	CProgressCtrl m_Progress;
	afx_msg void OnBnClickedButtonTxt();
	void ReadTimeFile(std::vector<double>& v_gpstime,double& dMin,double& dMax);
};
