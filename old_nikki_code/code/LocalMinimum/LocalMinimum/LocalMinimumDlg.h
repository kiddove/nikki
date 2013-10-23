// LocalMinimumDlg.h : 头文件
//

#pragma once
#include <vector>
#include <algorithm>

// CLocalMinimumDlg 对话框
class CLocalMinimumDlg : public CDialog
{
// 构造
public:
	CLocalMinimumDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LOCALMINIMUM_DIALOG };

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
	afx_msg void OnBnClickedButtonStart();
private:
	CString m_strInputPath;
	std::vector<std::vector<int>> m_planarVec;
	std::vector<int> m_rowVec;
	void InitInput(CString& strInput);
	void InsertIntoVec(CString& strContent,int& iRow);
	bool CompareAround(int iRow,int iCol);
	int m_iSize;
	CStdioFile m_outfile;
	int* m_pComArr1;
	int* m_pComArr2;
	int* m_pComArr3;
	int* m_pComArr4;
};
