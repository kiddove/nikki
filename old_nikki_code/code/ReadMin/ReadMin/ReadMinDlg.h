// ReadMinDlg.h : 头文件
//

#pragma once

struct HisTradeDataCustomStruct
{
	long		Time;		//时间
	float		Open;
	float		High;
	float		Low;
	float		Close;
	double		Volume;		//成交量
	double		Amount;		//成交金额
};

// CReadMinDlg 对话框
class CReadMinDlg : public CDialog
{
// 构造
public:
	CReadMinDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_READMIN_DIALOG };

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
	CString m_strPath;
};
