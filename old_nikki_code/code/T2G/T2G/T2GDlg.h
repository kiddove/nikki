// T2GDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include <algorithm>

// CT2GDlg �Ի���
class CT2GDlg : public CDialog
{
// ����
public:
	CT2GDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_T2G_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
