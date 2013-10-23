// ReFillDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReFill.h"
#include "ReFillDlg.h"

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


// CReFillDlg 对话框




CReFillDlg::CReFillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReFillDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReFillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CReFillDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CReFillDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_REFILL, &CReFillDlg::OnBnClickedButtonRefill)
END_MESSAGE_MAP()


// CReFillDlg 消息处理程序

BOOL CReFillDlg::OnInitDialog()
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

void CReFillDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CReFillDlg::OnPaint()
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
HCURSOR CReFillDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CReFillDlg::OnBnClickedButtonOpen()
{
	m_rowVecJ.clear();
	m_rowVecI.clear();
	m_rowVecG.clear();
	m_rowVecC.clear();
	m_RowPos.clear();
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

void CReFillDlg::OnBnClickedButtonRefill()
{
	// TODO: 在此添加控件通知处理程序代码
	CStdioFile ff;
	CString strOut;
	int ipos = m_strInputPath.ReverseFind(_T('\\'));
	strOut = m_strInputPath.Left(ipos + 1) + _T("MaxValue.txt");
	// CStdioFile m_outfile;
	if (!ff.Open(strOut,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Failed to Open Dest File!"));
		return;
	}
	for (int k=0;k<=243;k++)
	{
		m_RowPos.clear();
		m_CValue.clear();
		int nCount=(int)std::count(m_rowVecG.begin(),m_rowVecG.end(),k);
		if (nCount == 0)
			continue;
		std::vector<int>::iterator iter = m_rowVecG.begin();
		while (iter!=m_rowVecG.end())
		{
			iter = std::find(iter,m_rowVecG.end(),k);
			if (iter!=m_rowVecG.end())
			{
				int iPos = (int)std::distance(m_rowVecG.begin(),iter);
				m_RowPos.push_back(iPos);
				m_CValue.push_back(m_rowVecC[iPos]);
				iter++;
			}
		}
		int iMaxCount = 0;
		int iMaxValue = 0;
		if (k != 0)
		{
			for (int i=1;i<=16;i++)
			{
				int iCount=(int)std::count(m_CValue.begin(),m_CValue.end(),i);
				CString strContent;
				strContent.Format(_T("G=%d,C=%d,Count:%d\n"),k,i,iCount);
				ff.WriteString(strContent);
				if (iCount > iMaxCount)
				{
					iMaxCount = iCount;
					iMaxValue = i;
				}
			}
		}
		for (int i=0;i<(int)m_RowPos.size();i++)
		{
			m_rowVecC[m_RowPos[i]] = iMaxValue;
		}
	}
	ff.Close();
	Output();
}

void CReFillDlg::Output()
{
	CString strOut;
	int ipos = m_strInputPath.ReverseFind(_T('\\'));
	strOut = m_strInputPath.Left(ipos + 1) + _T("OutPut.txt");
	// CStdioFile m_outfile;
	if (!m_outfile.Open(strOut,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Failed to Open Dest File!"));
		return;
	}
	for (int i=0;i<(int)m_rowVecC.size();i++)
	{
		CString strCont;
		strCont.Format("%d    %d    %d    %d\n",m_rowVecJ[i],m_rowVecI[i],m_rowVecG[i],m_rowVecC[i]);
		m_outfile.WriteString(strCont);
	}
	m_outfile.Close();
}
void CReFillDlg::InitInput(CString& strInput)
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
	// m_planarVecG.push_back(m_rowVecG);
	// m_planarVecC.push_back(m_rowVecC);
	inputfile.Close();
}

void CReFillDlg::InsertIntoVec(CString& strContent,int& iRow)
{
	int ilen = strContent.GetLength();
	int ipos = strContent.Find(_T(' '));
	CString strTmp = strContent.Left(ipos+1);
	int iCol = atoi(strTmp);
	m_rowVecJ.push_back(iCol);
	strTmp = strContent.Right(ilen - ipos);
	strTmp.Trim();
	ipos = strTmp.Find(_T(' '));
	int iTmp = atoi(strTmp.Left(ipos+1));
	//if (iTmp > iRow)
	//{
	//	iRow = iTmp;
	//	m_planarVecG.push_back(m_rowVecG);
	//	m_rowVecG.clear();
	//	//TRACE(_T("\r\n"));
	//}
	m_rowVecI.push_back(iTmp);
	strTmp = strTmp.Right(strTmp.GetLength() - ipos);
	strTmp.Trim();
	//ipos = strTmp.Find(_T(' '));
	int iVal = atoi(strTmp);
	//TRACE(_T("\t%d"),iVal);
	m_rowVecG.push_back(iVal);
	strTmp = strTmp.Right(strTmp.GetLength() - ipos);
	strTmp.Trim();
	//ipos = strTmp.Find(_T(' '));
	iTmp = atoi(strTmp);
	m_rowVecC.push_back(iTmp);
}