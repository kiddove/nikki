// LasAndDemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LasAndDem.h"
#include "LasAndDemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CLasAndDemDlg dialog




CLasAndDemDlg::CLasAndDemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLasAndDemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLasAndDemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLasAndDemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_READ_LAS, &CLasAndDemDlg::OnBnClickedButtonReadLas)
	ON_BN_CLICKED(IDC_BUTTON_READ_DEM, &CLasAndDemDlg::OnBnClickedButtonReadDem)
	ON_BN_CLICKED(IDC_BUTTON_GEN_ADJ, &CLasAndDemDlg::OnBnClickedButtonGenAdj)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_ADJ, &CLasAndDemDlg::OnBnClickedButtonLoadAdj)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CLasAndDemDlg::OnBnClickedButtonCalc)
END_MESSAGE_MAP()


// CLasAndDemDlg message handlers

BOOL CLasAndDemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_Data.Init();
#ifdef DEM
	GetDlgItem(IDC_BUTTON_READ_DEM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GEN_ADJ)->EnableWindow(FALSE);
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLasAndDemDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLasAndDemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLasAndDemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLasAndDemDlg::OnBnClickedButtonReadLas()
{
	// TODO: Add your control notification handler code here
	m_Data.ReadLAS();
	::MessageBox(NULL, "Load LAS Ok!", _T("INFO"), MB_OK);
#ifdef DEM
	GetDlgItem(IDC_BUTTON_READ_DEM)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READ_LAS)->EnableWindow(FALSE);
#endif
}

void CLasAndDemDlg::OnBnClickedButtonReadDem()
{
	// TODO: Add your control notification handler code here
#ifdef DEM
	m_Data.ReadDEM();
	::MessageBox(NULL, "Load DEM Ok!", _T("INFO"), MB_OK);
	GetDlgItem(IDC_BUTTON_GEN_ADJ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READ_DEM)->EnableWindow(FALSE);
#endif
}

void CLasAndDemDlg::OnBnClickedButtonGenAdj()
{
	// TODO: Add your control notification handler code here
#ifdef DEM
	m_Data.Adjust();
	::MessageBox(NULL, "Generate Adjust File Ok!", _T("INFO"), MB_OK);
#endif
}

void CLasAndDemDlg::OnBnClickedButtonLoadAdj()
{
	// TODO: Add your control notification handler code here
#ifdef DEM
	m_Data.LoadAdjust();
	::MessageBox(NULL, "Read Adjust File Ok!", _T("INFO"), MB_OK);
#endif
}

void CLasAndDemDlg::OnBnClickedButtonCalc()
{
	// TODO: Add your control notification handler code here
#ifdef DEM
	m_Data.Calculate();
	::MessageBox(NULL, "Calculate Ok!", _T("INFO"), MB_OK);
#endif
}
