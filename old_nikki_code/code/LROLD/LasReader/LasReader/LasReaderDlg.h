// LasReaderDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "LasFile.h"

// CLasReaderDlg �Ի���
class CLasReaderDlg : public CDialog
{
// ����
public:
	CLasReaderDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LASREADER_DIALOG };

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
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonXyz();
protected:
	CProgressCtrl m_ProgressCtrl;

	void xyz2las(CStdioFile* pstdfile,CLasFile* plasfile);
};
