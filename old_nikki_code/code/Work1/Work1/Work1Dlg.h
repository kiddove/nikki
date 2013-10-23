// Work1Dlg.h : 头文件
//

#pragma once
#include <vector>
#include <algorithm>
using namespace std;
// CWork1Dlg 对话框
class CWork1Dlg : public CDialog
{
// 构造
public:
	CWork1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WORK1_DIALOG };

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
	afx_msg void OnBnClickedButtonY();
	vector<double> VecY;
	vector<vector<double>> m_Matrix;
	void ReadFromTxt(CString& strPath,vector<double>& vec);
	void InvMatrix(vector<vector<double>>& matrix);
	void Run();
	void ReportError(CString& strErr)
	{
		GetDlgItem(IDC_STATIC)->SetWindowText(strErr);
		UpdateData(FALSE);
	}
	int m_iK;
	CString m_strPath;
	vector<double> VecT;
	double Sum(vector<double>& vec);
	double Sum2(vector<double>& vec);
	double Sum3(vector<double>& vec);
	double Sum4(vector<double>& vec);
	vector<vector<double>> C1;
	vector<vector<double>> C2;
	vector<vector<double>> C3;
	vector<vector<double>> Lambda;
	vector<vector<double>> Mju;
	vector<vector<double>> Eta;

	vector<vector<double>> veca;
	vector<vector<double>> vecb;
	vector<vector<double>> vecc;

	vector<vector<double>> vecx0;
	vector<vector<double>> vecx1;
	vector<vector<double>> vecx2;

	vector<vector<double>> vecs;

	void MultiMatrix(vector<vector<double>>& VecResult,vector<vector<double>>& VecA,vector<vector<double>>& VecB);

	afx_msg void OnBnClickedButtonRun();
};
