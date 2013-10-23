// WorkDlg.h : 头文件
//

#pragma once
#include <vector>
#include <algorithm>

// CWorkDlg 对话框
class CWorkDlg : public CDialog
{
// 构造
public:
	CWorkDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WORK_DIALOG };

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
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonStart();
private:
	CString m_strInputPath;
	std::vector<std::vector<int>> m_planarVec;
	std::vector<int> m_rowVec;
	void InitInput(CString& strInput);
	void InsertIntoVec(CString& strContent,int& iRow);
	bool CompareAround(int iRow,int iCol);
};
