// SegmetationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Segmetation.h"
#include "SegmetationDlg.h"

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


// CSegmetationDlg dialog




CSegmetationDlg::CSegmetationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmetationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSegmetationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSegmetationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEED_FILE, &CSegmetationDlg::OnBnClickedButtonSeedFile)
	ON_BN_CLICKED(IDC_BUTTON_IMG_FILE, &CSegmetationDlg::OnBnClickedButtonImgFile)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SEED, &CSegmetationDlg::OnBnClickedButtonLoadSeed)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMG, &CSegmetationDlg::OnBnClickedButtonLoadImg)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CSegmetationDlg::OnBnClickedButtonProcess)
END_MESSAGE_MAP()


// CSegmetationDlg message handlers

BOOL CSegmetationDlg::OnInitDialog()
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
	SetTipInfo();
	GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSegmetationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSegmetationDlg::OnPaint()
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
HCURSOR CSegmetationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSegmetationDlg::OnBnClickedButtonSeedFile()
{
	// TODO: Add your control notification handler code here
	char szFilter[] = _T("Seed Files(*.txt)|*.txt|All Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, NULL, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		m_strSeedFile = dlg.GetPathName();
	}

	GetDlgItem(IDC_EDIT_SEED_FILE)->SetWindowText(m_strSeedFile);
}

void CSegmetationDlg::OnBnClickedButtonImgFile()
{
	// TODO: Add your control notification handler code here
	//char szFilter[] = _T("Image Files(*.tif)|*.tif|All Files(*.*)|*.*||");
	char szFilter[] = _T("Image Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, NULL, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		m_strImgFile = dlg.GetPathName();
	}

	GetDlgItem(IDC_EDIT_IMG_FILE)->SetWindowText(m_strImgFile);
}

void CSegmetationDlg::OnBnClickedButtonLoadSeed()
{
	// TODO: Add your control notification handler code here
	// space seperated two float

	if (m_strSeedFile.IsEmpty())
	{
		AfxMessageBox(_T("Please Choose a Seed file first~!"));
	}
	
	CStdioFile fSeed;
	if (fSeed.Open(m_strSeedFile, CFile::modeRead))
	{
		m_vecSeed.clear();
		CString strContent;
		while (fSeed.ReadString(strContent))
		{
			strContent.Trim();
			CString str;
			int i = 0;
			//while (AfxExtractSubString(str, strContent, i, _T(' ')))
			//{
			//	TRACE(_T("%d, %s\n"), i, str);
			//	i++;
			//}
			Pixel p;

			// start from bottom-left, col comes first
			// if start from top-left, row comes first
			AfxExtractSubString(str, strContent, i, _T('\t'));
			p.col = (int)atof(str);
			i++;
			AfxExtractSubString(str, strContent, i, _T('\t'));
			p.row = (int)atof(str);

			m_vecSeed.push_back(p);
		}
		fSeed.Close();

		AfxMessageBox(_T("Seed File Loaded~!"));
	}
	else
	{
		// to be continued..
		// log the error
	}
}

void CSegmetationDlg::OnBnClickedButtonLoadImg()
{
	// TODO: Add your control notification handler code here
	m_datafile.Close();
	if (m_datafile.OpenFile(m_strImgFile))
	{
		AfxMessageBox(_T("Image File Loaded~!"));
		GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
	}
}

void CSegmetationDlg::OnBnClickedButtonProcess()
{
	// TODO: Add your control notification handler code here

	CString str;
	GetDlgItem(IDC_EDIT_THRESHOLD)->GetWindowText(str);
	m_con.threshold = (float)atof(str);

	if (m_con.threshold <= 0.0000001)
	{
		AfxMessageBox(_T("Invalid Threshold"));
		return;
	}

	GetDlgItem(IDC_EDIT_MIN_SIZE)->GetWindowText(str);
	m_con.min_size = atoi(str);

	if (m_con.min_size < 2)
	{
		AfxMessageBox(_T("Invalid Minimum Size."));
		return;
	}

	int iCheck_ED = IsDlgButtonChecked(IDC_CHECK_EUCLIDEAN_DISTANCE);
	int iCheck_NC = IsDlgButtonChecked(IDC_CHECK_NEW_CRITERIA);

	if (iCheck_ED)
		m_con.calcType = Euclidean_Distance;
	else if (iCheck_NC)
		m_con.calcType = New_Criteria;
	else
		m_con.calcType = None;

	if (m_con.calcType == None)
	{
		AfxMessageBox(_T("Please choose Euclidean Distance Or New Criteria."));
		return;
	}
	int iCheck4 = IsDlgButtonChecked(IDC_CHECK_4NEIGHBOUR);
	int iCheck8 = IsDlgButtonChecked(IDC_CHECK_8NEIGHBOUR);

	if (iCheck4)
		m_con.neibType = Four_Neighbour;
	else if (iCheck8)
		m_con.neibType = Eight_Neighbour;
	else
		m_con.neibType = None_Neighbour;

	if (m_con.neibType == None_Neighbour)
	{
		AfxMessageBox(_T("Please choose 4 Neighbours Or 8 Neighbours."));
		return;
	}

	m_datafile.Process(m_vecSeed, m_con);

	AfxMessageBox("Finished~!");
}


BOOL CSegmetationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT CSegmetationDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case WM_COMMAND:
		{
			HWND hCtl = (HWND)lParam;
			int wmId = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);
			if (lParam)
			{
				switch (wmId)
				{
				case IDC_CHECK_EUCLIDEAN_DISTANCE:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_EUCLIDEAN_DISTANCE))
								{
									m_con.calcType = Euclidean_Distance;
									UnCkeck(IDC_CHECK_NEW_CRITERIA);
								}
							}
							break;
						default:
							break;
						}
					}
					break;
				case IDC_CHECK_NEW_CRITERIA:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_NEW_CRITERIA))
								{
									m_con.calcType = New_Criteria;
									UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;
				case IDC_CHECK_4NEIGHBOUR:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_4NEIGHBOUR))
								{
									m_con.neibType = Four_Neighbour;
									UnCkeck(IDC_CHECK_8NEIGHBOUR);
								}
							}
							break;
						default:
							break;
						}
					}
					break;
				case IDC_CHECK_8NEIGHBOUR:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_8NEIGHBOUR))
								{
									m_con.neibType = Eight_Neighbour;
									UnCkeck(IDC_CHECK_4NEIGHBOUR);
								}
							}
							break;
						default:
							break;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void CSegmetationDlg::SetCheck(int iCtrl)
{
	CButton* pCheck = (CButton*)GetDlgItem(iCtrl);
	pCheck->SetCheck(TRUE);
}

void CSegmetationDlg::UnCkeck(int iCtrl)
{
	CButton* pCheck = (CButton*)GetDlgItem(iCtrl);
	pCheck->SetCheck(FALSE);
}

void CSegmetationDlg::SetTipInfo()
{
	// tool tip
	EnableToolTips(TRUE);
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_PROCESS), _T("press to process"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_IMG_FILE), _T("press to choose an image file"));
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_MIN_SIZE), _T("segment size smaller than this number will be forced to be merged into neighbour segments"));
	m_tooltip.SetTipBkColor(RGB(255, 255, 255));
	m_tooltip.SetTipTextColor(RGB(0, 0, 0));
	m_tooltip.SetDelayTime(500);
}

