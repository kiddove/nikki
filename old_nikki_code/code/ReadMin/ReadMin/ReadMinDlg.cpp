// ReadMinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReadMin.h"
#include "ReadMinDlg.h"

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


// CReadMinDlg 对话框




CReadMinDlg::CReadMinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadMinDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReadMinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CReadMinDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CReadMinDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CReadMinDlg::OnBnClickedButtonRead)
END_MESSAGE_MAP()


// CReadMinDlg 消息处理程序

BOOL CReadMinDlg::OnInitDialog()
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

void CReadMinDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CReadMinDlg::OnPaint()
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
HCURSOR CReadMinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CReadMinDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	// CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Min Files(*.min)|*.min|All Files (*.*)|*.*||"));
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
	// GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	// UpdateData(FALSE);
}

void CReadMinDlg::OnBnClickedButtonRead()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile minfile;
	if (!minfile.Open(m_strPath,CFile::modeRead | CFile::shareDenyWrite))
	{
		CString str;
		str.Format(_T("Open Faild:%d"),GetLastError());
		return;
	}
	HisTradeDataCustomStruct minstruct;
	UINT nLen = (UINT)minfile.GetLength();
	UINT nRead = 0;
	ASSERT(nLen == sizeof(HisTradeDataCustomStruct) * 240);
	while (nRead < nLen)
	{
		memset(&minstruct,0,sizeof(HisTradeDataCustomStruct));
		nRead += minfile.Read(&minstruct,sizeof(HisTradeDataCustomStruct));
		
		if (minstruct.High < minstruct.Low)
		{
			CString str;
			str.Format(_T("Time:%d\r\nHigh:%.2f\r\nLow:%.2f"),minstruct.Time,minstruct.High,minstruct.Low);
			AfxMessageBox(str);
			break;
		}
	}
	minfile.Close();
}
