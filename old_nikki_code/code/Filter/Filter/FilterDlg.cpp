// FilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Filter.h"
#include "FilterDlg.h"

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


// CFilterDlg 对话框




CFilterDlg::CFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CFilterDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_FILETER, &CFilterDlg::OnBnClickedButtonFileter)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序

BOOL CFilterDlg::OnInitDialog()
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

void CFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFilterDlg::OnPaint()
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
HCURSOR CFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFilterDlg::OnBnClickedButtonOpen()
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
		m_strPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	GetDlgItem(IDC_EDIT_TXT_FILE)->SetWindowText(m_strPath);
	UpdateData(FALSE);
}

void CFilterDlg::OnBnClickedButtonFileter()
{
	// TODO: 在此添加控件通知处理程序代码
	CStdioFile txtfile,txtoutfile;
	CString strout;
	int ipos = m_strPath.ReverseFind(_T('\\'));
	strout = m_strPath.Left(ipos+1);
	strout += _T("z_fileter.txt");
	txtfile.Open(m_strPath,CFile::modeRead|CFile::shareDenyWrite);
	txtoutfile.Open(strout,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite);
	CString strContent;
	while (txtfile.GetPosition()!=txtfile.GetLength())
	{
		txtfile.ReadString(strContent);
		if (_T('0') < strContent.GetAt(0) && _T('9') > strContent.GetAt(0))
		{
			int ipos = strContent.Find(_T('\t'));
			CString strTemp = strContent.Right(strContent.GetLength() - ipos - 1);
			ipos = strTemp.Find(_T("\t"));
			strTemp = strTemp.Right(strTemp.GetLength() - ipos - 1);
			ipos = strTemp.Find(_T("\t"));
			strTemp = strTemp.Left(ipos);
			double dd = atof(strTemp);
			if (dd > 160.0)
				txtoutfile.WriteString(strContent);
		}
		else
			txtoutfile.WriteString(strContent);
	}

	txtfile.Close();
	txtoutfile.Close();
}
