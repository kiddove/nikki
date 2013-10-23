// Tree top extractionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Tree top extraction.h"
#include "Tree top extractionDlg.h"

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


// CTreetopextractionDlg 对话框




CTreetopextractionDlg::CTreetopextractionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreetopextractionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTreetopextractionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTreetopextractionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTreetopextractionDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_WORK, &CTreetopextractionDlg::OnBnClickedButtonWork)
END_MESSAGE_MAP()


// CTreetopextractionDlg 消息处理程序

BOOL CTreetopextractionDlg::OnInitDialog()
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

void CTreetopextractionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTreetopextractionDlg::OnPaint()
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
HCURSOR CTreetopextractionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTreetopextractionDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	//CString strPath;
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

void CTreetopextractionDlg::OnBnClickedButtonWork()
{
	CString strOut;
	int ipos = m_strInputPath.ReverseFind(_T('\\'));
	strOut = m_strInputPath.Left(ipos + 1) + _T("OutPut.txt");
	CStdioFile outfile;
	if (!outfile.Open(strOut,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Failed to Open Dest File!"));
		return;
	}
	CString strCont;
	// TODO: 在此添加控件通知处理程序代码
	//int ii = (int)m_planarVec.size() - 2;
	//int jj = (int)m_planarVec[0].size() - 2;
	//for (int i=2;i<ii;i++)
	//{
	//	for (int j=2;j<jj;j++)
	//	{
	int ii = (int)m_planarVec.size();
	int jj = (int)m_planarVec[0].size();
	for (int i=0;i<ii;i++)
	{
		for (int j=0;j<jj;j++)
		{
			if (CompareAround(i,j))
			{
				// 输出列,行,值
				TRACE(_T("%d\t%d\t%d\r\n"),j+1,i+1,m_planarVec[i][j]);
				strCont.Format(_T("%d\t%d\t%d\r\n"),j+1,i+1,m_planarVec[i][j]);
				outfile.WriteString(strCont);
			}
		}
	}
	outfile.Close();
}

void CTreetopextractionDlg::InitInput(CString& strInput)
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
void CTreetopextractionDlg::InsertIntoVec(CString& strContent,int& iRow)
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
//bool CTreetopextractionDlg::CompareAround(int iRow, int iCol)
//{
//	if (m_planarVec[iRow][iCol] > m_planarVec[iRow-1][iCol]	// bottom
//	&& m_planarVec[iRow][iCol] > m_planarVec[iRow+1][iCol] // top
//	&& m_planarVec[iRow][iCol] > m_planarVec[iRow][iCol-1] // left
//	&& m_planarVec[iRow][iCol] > m_planarVec[iRow][iCol+1]) // right
//		return true;
//	else if (m_planarVec[iRow][iCol] == m_planarVec[iRow-1][iCol] && m_planarVec[iRow][iCol]>0)
//	{
//		return CompareAround(iCol,iRow-1,iRow,1);
//	}
//	else if (m_planarVec[iRow][iCol] == m_planarVec[iRow+1][iCol] && m_planarVec[iRow][iCol]>0)
//	{
//		return CompareAround(iCol,iRow,iRow+1,1);
//	}
//	else if (m_planarVec[iRow][iCol] == m_planarVec[iRow][iCol-1] && m_planarVec[iRow][iCol]>0)
//	{
//		return CompareAround(iRow,iCol-1,iCol,2);
//	}
//	else if (m_planarVec[iRow][iCol] == m_planarVec[iRow][iCol+1] && m_planarVec[iRow][iCol]>0)
//	{
//		return CompareAround(iRow,iCol,iCol+1,2);
//	}
//	else
//		return false;
//}
bool CTreetopextractionDlg::CompareAround(int iRow, int iCol)
{
	int a00 = 0;
	int a01 = 0;
	int a02 = 0;
	int a10 = 0;
	int a11 = 0;
	int a12 = 0;
	int a20 = 0;
	int a21 = 0;
	int a22 = 0;

	if (iRow - 1 >=0)
	{
		if (iCol - 1 >= 0)
			a00 = m_planarVec[iRow-1][iCol-1];
		a01 = m_planarVec[iRow-1][iCol];
		if (iCol + 1 < m_planarVec[0].size())
			a02 = m_planarVec[iRow-1][iCol+1];
	}
	if (iCol - 1 >= 0)
		a10 = m_planarVec[iRow][iCol-1];
	a11 = m_planarVec[iRow][iCol];
	if (iCol + 1 < m_planarVec[0].size())
		a12 = m_planarVec[iRow][iCol+1];
	if (iRow + 1 < m_planarVec.size())
	{
		if (iCol - 1 >= 0)
			a20 = m_planarVec[iRow+1][iCol-1];
		a21 = m_planarVec[iRow+1][iCol];
		if (iCol + 1 < m_planarVec[0].size())
			a22 = m_planarVec[iRow+1][iCol+1];
	}

	if (a11 > a10 && a11 > a12
	&& a11 > a00 && a11 > a01 && a11 > a02
	&& a11 > a20 && a11 > a21 && a11 > a22)
		return true;
	else if (a11 == a01 && a11>0)
	{
		return CompareAround(iCol,iRow-1,iRow,1);
	}
	else if (a11 == a21 && a11>0)
	{
		return CompareAround(iCol,iRow,iRow+1,1);
	}
	else if (a11 == a10 && a11>0)
	{
		return CompareAround(iRow,iCol-1,iCol,2);
	}
	else if (a11 == a12 && a11>0)
	{
		return CompareAround(iRow,iCol,iCol+1,2);
	}
	else
		return false;
}
//bool CTreetopextractionDlg::CompareAround(int iConst,int iVaria1,int iVaria2,int iMode)
//{
//	switch(iMode)
//	{
//	case 1: // Col Row Row
//		{
//			if (m_planarVec[iVaria1][iConst] > m_planarVec[iVaria1-1][iConst] // top
//			&& m_planarVec[iVaria1][iConst] > m_planarVec[iVaria2+1][iConst] // bottom
//			&& m_planarVec[iVaria1][iConst] > m_planarVec[iVaria1][iConst-1] // left top
//			&& m_planarVec[iVaria1][iConst] > m_planarVec[iVaria2][iConst-1] // left bottom
//			&& m_planarVec[iVaria1][iConst] > m_planarVec[iVaria1][iConst+1] // right top
//			&& m_planarVec[iVaria1][iConst] > m_planarVec[iVaria2][iConst+1]) // right bottom
//				return true;
//			else
//				return false;
//		}
//		break;
//	case 2: // Row Col Col
//		{
//			if (m_planarVec[iConst][iVaria1] > m_planarVec[iConst][iVaria1-1] // left
//			&& m_planarVec[iConst][iVaria1] > m_planarVec[iConst][iVaria2+1] // right
//			&& m_planarVec[iConst][iVaria1] > m_planarVec[iConst-1][iVaria1] // top left
//			&& m_planarVec[iConst][iVaria1] > m_planarVec[iConst+1][iVaria2] // top right
//			&& m_planarVec[iConst][iVaria1] > m_planarVec[iConst-1][iVaria1] // bottom left
//			&& m_planarVec[iConst][iVaria1] > m_planarVec[iConst+1][iVaria2]) // bottom right
//				return true;
//			else
//				return false;
//		}
//		break;
//	default:
//		return false;
//	}
//	return false;
//}
bool CTreetopextractionDlg::CompareAround(int iConst,int iVaria1,int iVaria2,int iMode)
{
	switch(iMode)
	{
	case 1: // Col Row Row
		{
			int a00 = 0;
			int a01 = 0;
			int a02 = 0;
			int a10 = 0;
			int a11 = 0;
			int a12 = 0;
			int a20 = 0;
			int a21 = 0;
			int a22 = 0;
			int a30 = 0;
			int a31 = 0;
			int a32 = 0;

			a11 = m_planarVec[iVaria1][iConst];
			a21 = m_planarVec[iVaria2][iConst];
			if (iConst - 1 > 0)
			{
				if (iVaria1 - 1 > 0)
					a00 = m_planarVec[iVaria1-1][iConst-1];
				a10 = m_planarVec[iVaria1][iConst-1];
				a20 = m_planarVec[iVaria2][iConst-1];
				if (iVaria2 + 1 < m_planarVec.size())
					a30 = m_planarVec[iVaria2+1][iConst-1];
			}
			if (iVaria1 - 1 > 0)
				a01 = m_planarVec[iVaria1-1][iConst];
			if (iVaria2 + 1 < m_planarVec.size())
				a31 = m_planarVec[iVaria2+1][iConst];
			if (iConst + 1 < m_planarVec[0].size())
			{
				if (iVaria1 - 1 > 0)
					a02 = m_planarVec[iVaria1-1][iConst+1];
				a12 = m_planarVec[iVaria1][iConst+1];
				a22 = m_planarVec[iVaria2][iConst+1];
				if (iVaria2 + 1 < m_planarVec.size())
					a32 = m_planarVec[iVaria2+1][iConst+1];
			}
			if (a11 > a00 && a11 > a01 && a11 > a02
			&& a11 > a10 && a11 > a12
			&& a21 > a20 && a21 > a22 
			&& a11 > a30 && a11 > a31 && a11 > a32)
				return true;
			else
				return false;
		}
		break;
	case 2: // Row Col Col
		{
			int a00 = 0;
			int a01 = 0;
			int a02 = 0;
			int a03 = 0;
			int a10 = 0;
			int a11 = 0;
			int a12 = 0;
			int a13 = 0;
			int a20 = 0;
			int a21 = 0;
			int a22 = 0;
			int a23 = 0;

			a11 = m_planarVec[iConst][iVaria1];
			a12 = m_planarVec[iConst][iVaria2];

			if (iConst - 1 >= 0)
			{
				if (iVaria1 - 1 >= 0)
					a00 = m_planarVec[iConst-1][iVaria1-1];
				a01 = m_planarVec[iConst-1][iVaria1];
				a02 = m_planarVec[iConst-1][iVaria2];
				if (iVaria2 + 1 < m_planarVec[0].size())
					a03 = m_planarVec[iConst-1][iVaria2+1];

			}
			if (iVaria1 - 1 >= 0)
				a10 = m_planarVec[iConst][iVaria1-1];
			if (iVaria2 + 1 < m_planarVec[0].size())
				a13 = m_planarVec[iConst][iVaria2+1];
			if (iConst + 1 < m_planarVec.size())
			{
				if (iVaria1 - 1 >= 0)
					a20 = m_planarVec[iConst+1][iVaria1-1];
				a21 = m_planarVec[iConst+1][iVaria1];
				a22 = m_planarVec[iConst+1][iVaria2];
				if (iVaria2 + 1 < m_planarVec[0].size())
					a23 = m_planarVec[iConst+1][iVaria2+1];

			}
			if (a11 > a00 && a11 > a01 && a11 > a02 && a11 > a03
			&& a11 > a10 && a11 > a13
			&& a11 > a20 && a11 > a21 && a11 > a22 && a11 > a23)
				return true;
			else
				return false;
		}
		break;
	default:
		return false;
	}
	return false;
}