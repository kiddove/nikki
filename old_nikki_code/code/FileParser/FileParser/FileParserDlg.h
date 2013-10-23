// FileParserDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include "afxcmn.h"

// CFileParserDlg �Ի���
class CFileParserDlg : public CDialog
{
// ����
public:
	CFileParserDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEPARSER_DIALOG };

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
	// afx_msg void OnBnClickedButtonOpenLas();
	afx_msg void OnBnClickedButtonOpenNdf();
	afx_msg void OnBnClickedButtonAddTime();
private:
	// CString m_strLasPath;
	CString m_strNdfPath;
	CString m_strIdxPath;
	//CString m_strCsdPath;
	CString m_strTimePath;
public:
	//afx_msg void OnBnClickedButtonCsd();
public:
	int SearchInVec(std::vector<double>& timevec,double dmintime,double dmaxtime,int& iStartPos);
	// Progress Control
	CProgressCtrl m_Progress;
	afx_msg void OnBnClickedButtonTxt();
	void ReadTimeFile(std::vector<double>& v_gpstime,double& dMin,double& dMax);
};
