// calGLCMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "calGLCM.h"
#include "calGLCMDlg.h"

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


// CcalGLCMDlg dialog




CcalGLCMDlg::CcalGLCMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CcalGLCMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcalGLCMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcalGLCMDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_FOLDER, &CcalGLCMDlg::OnBnClickedButtonChooseFolder)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CcalGLCMDlg::OnBnClickedButtonCalc)
END_MESSAGE_MAP()


// CcalGLCMDlg message handlers

BOOL CcalGLCMDlg::OnInitDialog()
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

	//std::vector<int> v1;
	//v1.push_back(1);
	//v1.push_back(3);
	//v1.push_back(5);
	//v1.push_back(7);

	//std::vector<int> v2;
	//v2.push_back(2);
	//v2.push_back(24);
	//v2.push_back(6);
	//v2.push_back(8);

	//v1.insert(v1.end(), v2.begin(), v2.end());

	//GLCMData gData(NULL);
	//gData.LoadFromFile(_T("E:\\files\\tif\\HFSP1p14.tif"));
	//gData.calGLCM(m_con);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CcalGLCMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CcalGLCMDlg::OnPaint()
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
HCURSOR CcalGLCMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CcalGLCMDlg::OnBnClickedButtonChooseFolder()
{
	// TODO: Add your control notification handler code here
       UpdateData(TRUE);

       char szDir[MAX_PATH];
       BROWSEINFO bi;

       ITEMIDLIST *pidl;

       bi.hwndOwner = this->m_hWnd;
       bi.pidlRoot = NULL;
       bi.pszDisplayName = szDir;
       bi.lpszTitle = _T("Choose a Folder");
       bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
       bi.lpfn = NULL;
       bi.lParam = 0;
       bi.iImage = 0;
       pidl = SHBrowseForFolder(&bi);

       if(pidl == NULL)  return;

       if(!SHGetPathFromIDList(pidl, szDir))   return;

       else  m_strPath = szDir;

       UpdateData(FALSE);
}

void CcalGLCMDlg::OnBnClickedButtonCalc()
{
	// TODO: Add your control notification handler code here
	m_con.angle = 0;
	CString str;
	GetDlgItem(IDC_EDIT_DISTANCE)->GetWindowText(str);
	m_con.distance = atoi(str);
	if (m_con.distance < 1)
	{
		AfxMessageBox(_T("Invalid parameter distance."));
		return;
	}
	GetDlgItem(IDC_EDIT_GRAYNUM)->GetWindowText(str);
	m_con.graynum = atoi(str);
	if (m_con.graynum < 1)
	{
		AfxMessageBox(_T("Invalid parameter graynum."));
		return;
	}
	GetDlgItem(IDC_EDIT_X)->GetWindowText(str);
	m_con.x = atoi(str);
	if (m_con.x < 1)
	{
		AfxMessageBox(_T("Invalid x graynum."));
		return;
	}
	GetDlgItem(IDC_EDIT_Y)->GetWindowText(str);
	m_con.y = atoi(str);
	if (m_con.y < 1)
	{
		AfxMessageBox(_T("Invalid y graynum."));
		return;
	}

	TCHAR szFilePath[256] = {0};

	GetModuleFileName(NULL, szFilePath, 256);

	CString strOutput(szFilePath);
	int iPos = strOutput.ReverseFind(_T('\\'));
	strOutput = strOutput.Left(iPos);
	strOutput += _T("\\result.txt");
	m_Output.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);

	CFileFind ff;
	CString strFile = m_strPath + _T("\\*.tif");
	BOOL bFind = ff.FindFile(strFile);

	CString strInfo;
	strInfo.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n"), _T("filename"), _T("entropy"), _T("asm"), _T("contrast"), _T("homogenity"), _T("meanH"), _T("meanV"), _T("varianceH"), _T("varianceV"), _T("corelation"), _T("dissimilarity"));
	m_Output.WriteString(strInfo);
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDirectory() && ! ff.IsDots())
		{
			CString strFilePath;
			strFilePath = ff.GetFilePath();
			GLCMData gData(&m_Output);
			gData.LoadFromFile(strFilePath);
			gData.calGLCM(m_con);
		}
	}

	m_Output.Close();
	AfxMessageBox(_T("All Finished~!"));
}
