// FileReaderDlg.h : ͷ�ļ�
//

#pragma once


// CFileReaderDlg �Ի���
class CFileReaderDlg : public CDialog
{
// ����
public:
	CFileReaderDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEREADER_DIALOG };

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
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedButtonCsd();
	afx_msg void OnBnClickedButtonBrowserCsd();
	afx_msg void OnBnClickedButtonBrowserNdf();
	afx_msg void OnBnClickedButtonNdf();
};
