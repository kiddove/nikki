// ReFillDlg.h : 头文件
//

#pragma once
#include <vector>
#include <map>
#include <algorithm>

// CReFillDlg 对话框
class CReFillDlg : public CDialog
{
// 构造
public:
	CReFillDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_REFILL_DIALOG };

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
	afx_msg void OnBnClickedButtonRefill();
private:
	CString m_strInputPath;
	//std::vector<std::vector<int>> m_planarVecG;
	//std::vector<std::vector<int>> m_planarVecC;
	std::vector<int> m_rowVecJ;
	std::vector<int> m_rowVecI;
	std::vector<int> m_rowVecG;
	std::vector<int> m_rowVecC;
	void InitInput(CString& strInput);
	void InsertIntoVec(CString& strContent,int& iRow);
	std::vector<int> m_RowPos;
	std::vector<int> m_CValue;
	void Output();
	CStdioFile m_outfile;
};
