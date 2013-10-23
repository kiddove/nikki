// RemoveDeadEndsDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include <algorithm>

// CRemoveDeadEndsDlg �Ի���
class CRemoveDeadEndsDlg : public CDialog
{
// ����
public:
	CRemoveDeadEndsDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOVEDEADENDS_DIALOG };

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
private:
	CString m_strInputPath;
	std::vector<std::vector<int>> m_planarVec;
	std::vector<int> m_rowVec;
	void InitInput(CString& strInput);
	void InsertIntoVec(CString& strContent,int& iRow);
	void Compare();
	void CompareAround(int iRow,int iCol);
	CStdioFile m_outfile;
public:
	afx_msg void OnBnClickedButtonStart();
};
