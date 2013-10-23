// ReFillDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include <map>
#include <algorithm>

// CReFillDlg �Ի���
class CReFillDlg : public CDialog
{
// ����
public:
	CReFillDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REFILL_DIALOG };

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
