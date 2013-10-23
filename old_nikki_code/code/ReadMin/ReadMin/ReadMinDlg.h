// ReadMinDlg.h : ͷ�ļ�
//

#pragma once

struct HisTradeDataCustomStruct
{
	long		Time;		//ʱ��
	float		Open;
	float		High;
	float		Low;
	float		Close;
	double		Volume;		//�ɽ���
	double		Amount;		//�ɽ����
};

// CReadMinDlg �Ի���
class CReadMinDlg : public CDialog
{
// ����
public:
	CReadMinDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_READMIN_DIALOG };

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
	CString m_strPath;
};
