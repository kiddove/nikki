// LasReaderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LasReader.h"
#include "LasReaderDlg.h"


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


// CLasReaderDlg 对话框




CLasReaderDlg::CLasReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLasReaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLasReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}

BEGIN_MESSAGE_MAP(CLasReaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLasReaderDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CLasReaderDlg::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, &CLasReaderDlg::OnBnClickedButtonXyz)
END_MESSAGE_MAP()


// CLasReaderDlg 消息处理程序

BOOL CLasReaderDlg::OnInitDialog()
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

void CLasReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLasReaderDlg::OnPaint()
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
HCURSOR CLasReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLasReaderDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Las Files(*.las)|*.las|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		strPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	UpdateData(FALSE);
}

void CLasReaderDlg::OnBnClickedButtonRead()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strLasPath,strXyzPaht;
	GetDlgItem(IDC_EDIT_LAS_PATH)->GetWindowText(strLasPath);
	GetDlgItem(IDC_EDIT_XYZ)->GetWindowText(strXyzPaht);

	CLasFile lasfile(strLasPath);
	lasfile.ParseFile();

	//CStdioFile xyzfile;
	//if (!xyzfile.Open(strXyzPaht,CFile::modeRead))
	//{
	//	AfxMessageBox(_T("can not open xyz file"));
	//	return;
	//}
	//xyz2las(&xyzfile,&lasfile);
	//lasfile.lasfile.Close();
	//lasfile.txtfile.Close();
	//xyzfile.Close();
}

void CLasReaderDlg::OnBnClickedButtonXyz()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Xyz Files(*.xyz)|*.xyz|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		strPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	GetDlgItem(IDC_EDIT_XYZ)->SetWindowText(strPath);
	UpdateData(FALSE);
}

void CLasReaderDlg::xyz2las(CStdioFile* pstdfile,CLasFile* plasfile)
{
	CString strCo;
	DWORD dw;
	CString strfloat,strFormat,strFind;
	ULONG nLen = pstdfile->GetLength();
	pstdfile->ReadString(strCo);
	pstdfile->ReadString(strCo);
	pstdfile->ReadString(strCo);
	std::map<CString,UINT>::iterator iterend = plasfile->m_map.end();
	std::map<CString,UINT>::iterator iter = iterend;
	while (1)
	{
		pstdfile->ReadString(strCo);

		int ipos = strCo.Find(' ');
		strfloat = strCo.Left(ipos);
		int idel = strCo.Delete(0,ipos + 2);
		double df=atof(strfloat);
		strFormat.Format("%.2f",df);
		strFind = strFormat + _T("-");

		ipos = strCo.Find(' ');
		strfloat = strCo.Left(ipos);
		idel = strCo.Delete(0,ipos + 2);
		df=atof(strfloat);
		strFormat.Format("%.2f",df);
		strFind += strFormat;
		strFind += _T("-");

		df=atof(strCo);
		strFormat.Format("%.2f",df);
		strFind += strFormat;

		iter = plasfile->m_map.find(strFind);
		if (iter != iterend)
		{
			UINT nIndex = iter->second;
			plasfile->ReadFile(nIndex);
		}

		dw = pstdfile->GetPosition();
		if (dw >= nLen)
			break;
	}
	//pstdfile->ReadString(strCo);
}