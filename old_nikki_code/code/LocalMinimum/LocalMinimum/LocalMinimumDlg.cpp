// LocalMinimumDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LocalMinimum.h"
#include "LocalMinimumDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CLocalMinimumDlg 对话框




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


// CLocalMinimumDlg 消息处理程序

BOOL CLocalMinimumDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLocalMinimumDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CLocalMinimumDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLocalMinimumDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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