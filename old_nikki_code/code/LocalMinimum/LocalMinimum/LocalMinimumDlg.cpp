// LocalMinimumDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LocalMinimum.h"
#include "LocalMinimumDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLocalMinimumDlg �Ի���




CLocalMinimumDlg::CLocalMinimumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalMinimumDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLocalMinimumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLocalMinimumDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLocalMinimumDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_START, &CLocalMinimumDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CLocalMinimumDlg ��Ϣ�������

BOOL CLocalMinimumDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLocalMinimumDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLocalMinimumDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CLocalMinimumDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLocalMinimumDlg::OnBnClickedButtonOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Txt Files(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		m_strInputPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	//GetDlgItem(IDC_EDIT_XYZ)->SetWindowText(strPath);
	UpdateData(FALSE);
	InitInput(m_strInputPath);
}

void CLocalMinimumDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	CString strNum;
	GetDlgItem(IDC_EDIT_SIZE)->GetWindowText(strNum);
	m_iSize = atoi(strNum);
	m_iSize = 3;
	CString strOut;
	int ipos = m_strInputPath.ReverseFind(_T('\\'));
	strOut = m_strInputPath.Left(ipos + 1) + _T("OutPut.txt");

	if (!m_outfile.Open(strOut,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Failed to Open Dest File!"));
		return;
	}

	m_pComArr1 = new int [m_iSize];
	m_pComArr2 = new int [m_iSize];
	m_pComArr3 = new int [m_iSize];
	m_pComArr4 = new int [m_iSize];
	int ii = (int)m_planarVec.size();
	int jj = (int)m_planarVec[0].size();
	for (int i=0;i<ii;i++)
		for (int j=0;j<jj;j++)
			CompareAround(i,j);

	delete[] m_pComArr1;
	delete[] m_pComArr2;
	delete[] m_pComArr3;
	delete[] m_pComArr4;
	m_pComArr1 = NULL;
	m_pComArr2 = NULL;
	m_pComArr3 = NULL;
	m_pComArr4 = NULL;
	//out put
	for (int i=0;i<ii;i++)
		for (int j=0;j<jj;j++)
		{
			CString strCont;
			strCont.Format("%d\t%d\t%d\n",i+1,j+1,m_planarVec[i][j]);
			m_outfile.WriteString(strCont);
		}
	m_outfile.Close();
}

void CLocalMinimumDlg::InitInput(CString& strInput)
{
	CStdioFile inputfile;
	if (!inputfile.Open(strInput,CFile::modeRead|CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Faild to Open Input File!"));
		return;
	}
	CString strContent;
	int ilen = (int)inputfile.GetLength();
	int iCurrent = 0;
	int iRow = 1;
	while (iCurrent != ilen)
	{
		inputfile.ReadString(strContent);
		CString strTemp = strContent;
		strTemp.Trim();
		InsertIntoVec(strTemp,iRow);
		iCurrent = (int)inputfile.GetPosition();
	}
	m_planarVec.push_back(m_rowVec);
	inputfile.Close();
}

void CLocalMinimumDlg::InsertIntoVec(CString& strContent,int& iRow)
{
	int ilen = strContent.GetLength();
	int ipos = strContent.Find(_T(' '));
	CString strTmp = strContent.Left(ipos+1);
	int iCol = atoi(strTmp);
	strTmp = strContent.Right(ilen - ipos);
	strTmp.Trim();
	ipos = strTmp.Find(_T(' '));
	int iTmp = atoi(strTmp.Left(ipos+1));
	if (iTmp > iRow)
	{
		iRow = iTmp;
		m_planarVec.push_back(m_rowVec);
		m_rowVec.clear();
		TRACE(_T("\r\n"));
	}
	strTmp = strTmp.Right(strTmp.GetLength() - ipos);
	strTmp.Trim();
	//ipos = strTmp.Find(_T(' '));
	int iVal = atoi(strTmp);
	TRACE(_T("\t%d"),iVal);
	m_rowVec.push_back(iVal);
}

bool CLocalMinimumDlg::CompareAround(int iRow, int iCol)
{
	//int iStep = (m_iSize - 1)/2;

	//bool bOut1 = true;
	//bool bOut2 = true;
	//bool bOut3 = true;
	//bool bOut4 = true;
	//// m_planarVec[iRow - iStep][iCol - iStep] m_planarVec[iRow][iCol] m_planarVec[iRow + iStep][iCol + iStep]

	//int j=0;
	//for (int i=-iStep,j=0;i<=iStep;i++,j++)
	//{
	//	//if (iRow - i <0);
	//	/*\*/
	//	m_pComArr1[j] = (iRow+i<0 || iCol+i<0 || iRow+i > (int)m_planarVec.size()-1 || iCol+i >(int)m_planarVec[0].size()-1)?0:m_planarVec[iRow+i][iCol+i];
	//	/*/*/
	//	m_pComArr2[j] = (iRow-i<0 || iCol+i<0 || iRow-i > (int)m_planarVec.size()-1 || iCol+i >(int)m_planarVec[0].size()-1)?0:m_planarVec[iRow-i][iCol+i];
	//	/*-*/
	//	m_pComArr3[j] = (iCol+i<0 || iCol+i >(int)m_planarVec[0].size()-1)?0:m_planarVec[iRow][iCol+i];
	//	/*|*/
	//	m_pComArr4[j] = (iRow+i<0 || iRow+i > (int)m_planarVec.size()-1)?0:m_planarVec[iRow+i][iCol];
	//}

	//for (j=0;j<m_iSize;j++)
	//{
	//	if (j!=iStep)
	//	{
	//		if (m_pComArr1[iStep] > m_pComArr1[j])
	//		{
	//			bOut1 = false;
	//			break;
	//		}
	//	}
	//}

	//for (j=0;j<m_iSize;j++)
	//{
	//	if (j!=iStep)
	//	{
	//		if (m_pComArr2[iStep] > m_pComArr2[j])
	//		{
	//			bOut2 = false;
	//			break;
	//		}
	//	}
	//}

	//for (j=0;j<m_iSize;j++)
	//{
	//	if (j!=iStep)
	//	{
	//		if (m_pComArr3[iStep] > m_pComArr3[j])
	//		{
	//			bOut3 = false;
	//			break;
	//		}
	//	}
	//}

	//for (j=0;j<m_iSize;j++)
	//{
	//	if (j!=iStep)
	//	{
	//		if (m_pComArr4[iStep] > m_pComArr4[j])
	//		{
	//			bOut4 = false;
	//			break;
	//		}
	//	}
	//}

	//if (bOut1 || bOut2 || bOut3 || bOut4)
	//{
	//	if (m_planarVec[iRow][iCol] > 0)
	//	{
	//		CString strCont;
	//		strCont.Format("%d\t%d\t%d\n",iCol+1,iRow+1,m_planarVec[iRow][iCol]);
	//		m_outfile.WriteString(strCont);
	//	}
	//}
	if (iRow-1>=0 && iCol-1>=0)
	{
		if (m_planarVec[iRow-1][iCol-1] > 0 && m_planarVec[iRow-1][iCol-1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow-1][iCol-1]=0;
	}
	if (iRow-1>=0 && iCol+1<=(int)m_planarVec[0].size()-1)
	{
		if (m_planarVec[iRow-1][iCol+1] > 0 && m_planarVec[iRow-1][iCol+1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow-1][iCol+1]=0;
	}
	if (iRow+1<=(int)m_planarVec.size()-1 && iCol-1>=0)
	{
		if (m_planarVec[iRow-1][iCol+1] > 0 && m_planarVec[iRow-1][iCol+1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow+1][iCol-1]=0;
	}
	if (iRow+1<=(int)m_planarVec.size()-1 && iCol+1<=(int)m_planarVec[0].size()-1)
	{
		if (m_planarVec[iRow-1][iCol+1] > 0 && m_planarVec[iRow-1][iCol+1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow+1][iCol+1]=0;
	}

	if (iRow-1>=0)
	{
		if (iCol-1>=0)
		{
			if (m_planarVec[iRow-1][iCol+1] > 0 && m_planarVec[iRow-1][iCol+1] - m_planarVec[iRow][iCol] > 5)
				m_planarVec[iRow-1][iCol-1]=0;
		}
		if (iCol+1<=(int)m_planarVec[0].size()-1)
		{
			if (m_planarVec[iRow-1][iCol+1] > 0 && m_planarVec[iRow-1][iCol+1] - m_planarVec[iRow][iCol] > 5)
				m_planarVec[iRow-1][iCol+1]=0;
		}
		if (m_planarVec[iRow-1][iCol] > 0 && m_planarVec[iRow-1][iCol] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow-1][iCol]=0;
	}
	if (iRow+1<=(int)m_planarVec.size()-1)
	{
		if (iCol-1>=0)
		{
			if (m_planarVec[iRow+1][iCol-1] > 0 && m_planarVec[iRow+1][iCol-1] - m_planarVec[iRow][iCol] > 5)
				m_planarVec[iRow+1][iCol-1]=0;
		}
		if (iCol+1<=(int)m_planarVec[0].size()-1)
		{
			if (m_planarVec[iRow+1][iCol+1] > 0 && m_planarVec[iRow+1][iCol+1] - m_planarVec[iRow][iCol] > 5)
				m_planarVec[iRow+1][iCol+1]=0;
		}
		if (m_planarVec[iRow+1][iCol] > 0 && m_planarVec[iRow+1][iCol] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow+1][iCol]=0;
	}
	if (iCol-1>=0)
	{
		if (m_planarVec[iRow][iCol-1] > 0 && m_planarVec[iRow][iCol-1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow][iCol-1]=0;
	}
	if (iCol+1<=(int)m_planarVec[0].size()-1)
	{
		if (m_planarVec[iRow][iCol+1] > 0 && m_planarVec[iRow][iCol+1] - m_planarVec[iRow][iCol] > 5)
			m_planarVec[iRow][iCol+1]=0;
	}

	return true;
}