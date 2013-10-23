// T2GDlg.h : 头文件
//

#pragma once
#include <vector>
#include <algorithm>

// CT2GDlg 对话框
class CT2GDlg : public CDialog
{
// 构造
public:
	CT2GDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_T2G_DIALOG };

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
	afx_msg void OnBnClickedButtonOpenT();
	afx_msg void OnBnClickedButtonOpenG();
	afx_msg void OnBnClickedButtonStart();
	CString m_strT;
	CString m_strG;
	CString m_strOut;
	std::vector<double> Tx;
	std::vector<double> Ty;
	std::vector<double> Tz;
	std::vector<double> Gx;
	std::vector<double> Gy;
	std::vector<double> Gz;
	std::vector<double> Rx;
	std::vector<double> Ry;
	std::vector<double> Rz;
	std::vector<double> Rd;
};
