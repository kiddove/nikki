// LocalMinimumDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include <algorithm>

// CLocalMinimumDlg �Ի���
class CLocalMinimumDlg : public CDialog
{
// ����
public:
	CLocalMinimumDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LOCALMINIMUM_DIALOG };

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
