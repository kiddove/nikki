// SegmetationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Segmentation.h"
#include "SegmentationDlg.h"
#include "ImageReader.h"

#include "StatisticDlg.h"


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


// CSegmentationDlg dialog




CSegmentationDlg::CSegmentationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmentationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDlg = NULL;
}

void CSegmentationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSegmentationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEED_FILE, &CSegmentationDlg::OnBnClickedButtonSeedFile)
	ON_BN_CLICKED(IDC_BUTTON_IMG_FILE, &CSegmentationDlg::OnBnClickedButtonImgFile)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SEED, &CSegmentationDlg::OnBnClickedButtonLoadSeed)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMG, &CSegmentationDlg::OnBnClickedButtonLoadImg)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CSegmentationDlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_MASK_FILE, &CSegmentationDlg::OnBnClickedButtonMaskFile)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MASK, &CSegmentationDlg::OnBnClickedButtonLoadMask)
	ON_BN_CLICKED(IDC_BUTTON_PREPROCESS, &CSegmentationDlg::OnBnClickedButtonPreprocess)
	ON_BN_CLICKED(IDC_BUTTON_STATISTIC, &CSegmentationDlg::OnBnClickedButtonStatistic)
END_MESSAGE_MAP()


// CSegmentationDlg message handlers

BOOL CSegmentationDlg::OnInitDialog()
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
	ZeroMemory(&m_traceinfo, sizeof(TraceInfo));
	SetTipInfo();
	GetDlgItem(IDC_BUTTON_PREPROCESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(FALSE);
	//GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
	//GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSegmentationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSegmentationDlg::OnPaint()
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
HCURSOR CSegmentationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSegmentationDlg::OnBnClickedButtonSeedFile()
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

void CSegmentationDlg::OnBnClickedButtonImgFile()
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

void CSegmentationDlg::OnBnClickedButtonLoadSeed()
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
			//Pixel p;
			cv::Point p;

			// start from bottom-left, col comes first
			// if start from top-left, row comes first
			AfxExtractSubString(str, strContent, i, _T(' '));
			p.y = (int)atof(str) - 1;
			//p.col = (int)atof(str);
			i++;
			AfxExtractSubString(str, strContent, i, _T(' '));
			p.x = (int)atof(str) - 1;
			//p.row = (int)atof(str);

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

void CSegmentationDlg::OnBnClickedButtonLoadImg()
{
	// TODO: Add your control notification handler code here
	m_datafile.Close();
	if (m_datafile.OpenFile(m_strImgFile))
	{
		AfxMessageBox(_T("Image File Loaded~!"));
		GetDlgItem(IDC_BUTTON_PREPROCESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
	}
}

void CSegmentationDlg::OnBnClickedButtonProcess()
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
#ifdef MULTI_THREAD
	GetDlgItem(IDC_EDIT_MIN_SIZE)->GetWindowText(str);
	m_con.min_size = atoi(str);

	//if (m_con.min_size < 1)
	//{
	//	AfxMessageBox(_T("Invalid Minimum Size."));
	//	return;
	//}
#endif

	//int iCheck_ED = IsDlgButtonChecked(IDC_CHECK_EUCLIDEAN_DISTANCE);
	//int iCheck_OLDBSMSE = IsDlgButtonChecked(IDC_CHECK_OLDBSMSE);
	//int iCheck_NEWBSMSE = IsDlgButtonChecked(IDC_CHECK_NEWBSMSE);
	//int iCheck_OLDBSMSE_NEWEQUATION = IsDlgButtonChecked(IDC_CHECK_OLDBSMSE_NEWEQUATION);
	//int iCheck_NEWBSMSE_NEWEQUATION = IsDlgButtonChecked(IDC_CHECK_NEWBSMSE_NEWEQUATION);
	//int iCheck_SM = IsDlgButtonChecked(IDC_CHECK_SIMILARITY_MEASURE);

	//if (iCheck_ED)
	//	m_con.calcType = Euclidean_Distance;
	//else if (iCheck_OLDBSMSE)
	//	m_con.calcType = Old_BSMSE;
	//else if (iCheck_NEWBSMSE)
	//	m_con.calcType = New_BSMSE;
	//else if (iCheck_OLDBSMSE_NEWEQUATION)
	//	m_con.calcType = Old_BSMSE_New_Equation;
	//else if (iCheck_NEWBSMSE_NEWEQUATION)
	//	m_con.calcType = New_BSMSE_New_Equation;
	//else if (iCheck_SM)
	//	m_con.calcType = Similarity_Measure;
	//else
	//	m_con.calcType = None;

	if (IsDlgButtonChecked(IDC_CHECK_ED))
		m_con.calcType = Euclidean_Distance;
	else if (IsDlgButtonChecked(IDC_CHECK_SAM))
		m_con.calcType = Spectral_Angle_Mapper;
	else if (IsDlgButtonChecked(IDC_CHECK_SD))
		m_con.calcType = Standard_Deviation;
	else if (IsDlgButtonChecked(IDC_CHECK_RSE))
		m_con.calcType = Root_Squared_Error;
	else if (IsDlgButtonChecked(IDC_CHECK_RDV))
		m_con.calcType = Rooted_Difference_in_Variance;
	else if (IsDlgButtonChecked(IDC_CHECK_RDSE))
		m_con.calcType = Rooted_Difference_in_Squared_Error;
	else
		m_con.calcType = None;

	if (m_con.calcType == None)
	{
		AfxMessageBox(_T("Please choose one method."));
		return;
	}
	//int iCheckResize = IsDlgButtonChecked(IDC_CHECK_RESIZE);
	//if (iCheckResize)
	//	m_datafile.m_bResize = true;
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

	//GetDlgItem(IDC_EDIT_ITERATION)->GetWindowText(str);
	//m_con.iteration = atoi(str);
	//if (m_con.iteration < 1)
	//{
	//	AfxMessageBox(_T("Invalid iteration"));
	//	return;
	//}

	//if (iCheck_SM)
	//{
	//	GetDlgItem(IDC_EDIT_ALPHA)->GetWindowText(str);
	//	m_con.alpha = (float)atof(str);

	//	if (m_con.alpha < 0.000001f || m_con.alpha > 1.000001f)
	//	{
	//		AfxMessageBox(_T("Invalid alpha"));
	//		return;
	//	}

	//	GetDlgItem(IDC_EDIT_BETA)->GetWindowText(str);
	//	m_con.beta = (float)atof(str);

	//	if (m_con.beta < 0.000001f || m_con.beta > 1.000001f)
	//	{
	//		AfxMessageBox(_T("Invalid beta"));
	//		return;
	//	}

	//	if (m_con.alpha + m_con.beta > 1.000001f)
	//	{
	//		AfxMessageBox(_T("alpha + beta > 1.0"));
	//		return;
	//	}
	//}

	//// trace
	//int iCheckTrace = IsDlgButtonChecked(IDC_CHECK_TRACE);
	//if (iCheckTrace)
	//{
	//	m_traceinfo.bTrace = true;
	//	GetDlgItem(IDC_EDIT_ROW)->GetWindowText(str);
	//	m_traceinfo.row = atoi(str);
	//	GetDlgItem(IDC_EDIT_COL)->GetWindowText(str);
	//	m_traceinfo.col = atoi(str);
	//}
	//else
	//	memset(&m_traceinfo, 0, sizeof(TraceInfo));

	m_datafile.Process(m_vecSeed, m_con, m_mapMask, m_traceinfo);

	AfxMessageBox("Finished~!");
}


BOOL CSegmentationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


LRESULT CSegmentationDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
				case IDC_CHECK_ED:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_ED))
								{
									m_con.calcType = Euclidean_Distance;
									UnCkeck(IDC_CHECK_SAM);
									UnCkeck(IDC_CHECK_SD);
									UnCkeck(IDC_CHECK_RSE);
									UnCkeck(IDC_CHECK_RDV);
									UnCkeck(IDC_CHECK_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_SAM:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_SAM))
								{
									m_con.calcType = Spectral_Angle_Mapper;
									UnCkeck(IDC_CHECK_ED);
									UnCkeck(IDC_CHECK_SD);
									UnCkeck(IDC_CHECK_RSE);
									UnCkeck(IDC_CHECK_RDV);
									UnCkeck(IDC_CHECK_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_SD:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_SD))
								{
									m_con.calcType = Standard_Deviation;
									UnCkeck(IDC_CHECK_SAM);
									UnCkeck(IDC_CHECK_ED);
									UnCkeck(IDC_CHECK_RSE);
									UnCkeck(IDC_CHECK_RDV);
									UnCkeck(IDC_CHECK_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_RSE:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_RSE))
								{
									m_con.calcType = Root_Squared_Error;
									UnCkeck(IDC_CHECK_SAM);
									UnCkeck(IDC_CHECK_SD);
									UnCkeck(IDC_CHECK_ED);
									UnCkeck(IDC_CHECK_RDV);
									UnCkeck(IDC_CHECK_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_RDV:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_RDV))
								{
									m_con.calcType = Rooted_Difference_in_Variance;
									UnCkeck(IDC_CHECK_SAM);
									UnCkeck(IDC_CHECK_SD);
									UnCkeck(IDC_CHECK_RSE);
									UnCkeck(IDC_CHECK_ED);
									UnCkeck(IDC_CHECK_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_RDSE:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_RDSE))
								{
									m_con.calcType = Rooted_Difference_in_Squared_Error;
									UnCkeck(IDC_CHECK_SAM);
									UnCkeck(IDC_CHECK_SD);
									UnCkeck(IDC_CHECK_RSE);
									UnCkeck(IDC_CHECK_ED);
									UnCkeck(IDC_CHECK_RDV);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

					case IDC_CHECK_PRE_ED:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_ED))
								{
									m_con.pre_calcType = Euclidean_Distance;
									UnCkeck(IDC_CHECK_PRE_SAM);
									UnCkeck(IDC_CHECK_PRE_SD);
									UnCkeck(IDC_CHECK_PRE_RSE);
									UnCkeck(IDC_CHECK_PRE_RDV);
									UnCkeck(IDC_CHECK_PRE_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_PRE_SAM:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_SAM))
								{
									m_con.pre_calcType = Spectral_Angle_Mapper;
									UnCkeck(IDC_CHECK_PRE_ED);
									UnCkeck(IDC_CHECK_PRE_SD);
									UnCkeck(IDC_CHECK_PRE_RSE);
									UnCkeck(IDC_CHECK_PRE_RDV);
									UnCkeck(IDC_CHECK_PRE_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_PRE_SD:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_SD))
								{
									m_con.pre_calcType = Standard_Deviation;
									UnCkeck(IDC_CHECK_PRE_SAM);
									UnCkeck(IDC_CHECK_PRE_ED);
									UnCkeck(IDC_CHECK_PRE_RSE);
									UnCkeck(IDC_CHECK_PRE_RDV);
									UnCkeck(IDC_CHECK_PRE_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_PRE_RSE:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_RSE))
								{
									m_con.pre_calcType = Root_Squared_Error;
									UnCkeck(IDC_CHECK_PRE_SAM);
									UnCkeck(IDC_CHECK_PRE_SD);
									UnCkeck(IDC_CHECK_PRE_ED);
									UnCkeck(IDC_CHECK_PRE_RDV);
									UnCkeck(IDC_CHECK_PRE_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_PRE_RDV:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_RDV))
								{
									m_con.pre_calcType = Rooted_Difference_in_Variance;
									UnCkeck(IDC_CHECK_PRE_SAM);
									UnCkeck(IDC_CHECK_PRE_SD);
									UnCkeck(IDC_CHECK_PRE_RSE);
									UnCkeck(IDC_CHECK_PRE_ED);
									UnCkeck(IDC_CHECK_PRE_RDSE);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

				case IDC_CHECK_PRE_RDSE:
					{
						switch (wmEvent)
						{
						case BN_CLICKED:
							{
								if (IsDlgButtonChecked(IDC_CHECK_PRE_RDSE))
								{
									m_con.pre_calcType = Rooted_Difference_in_Squared_Error;
									UnCkeck(IDC_CHECK_PRE_SAM);
									UnCkeck(IDC_CHECK_PRE_SD);
									UnCkeck(IDC_CHECK_PRE_RSE);
									UnCkeck(IDC_CHECK_PRE_ED);
									UnCkeck(IDC_CHECK_PRE_RDV);
								}
							}
							break;
						default:
							break;
						}
					}
					break;

					//case IDC_CHECK_EUCLIDEAN_DISTANCE:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_EUCLIDEAN_DISTANCE))
					//				{
					//					m_con.calcType = Euclidean_Distance;
					//					UnCkeck(IDC_CHECK_OLDBSMSE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_NEWBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_SIMILARITY_MEASURE);
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
					//case IDC_CHECK_OLDBSMSE:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_OLDBSMSE))
					//				{
					//					m_con.calcType = Old_BSMSE;
					//					UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_NEWBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_SIMILARITY_MEASURE);
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
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
					//case IDC_CHECK_SIMILARITY_MEASURE:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_SIMILARITY_MEASURE))
					//				{
					//					m_con.calcType = Similarity_Measure;
					//					UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_NEWBSMSE_NEWEQUATION);

					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(TRUE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(TRUE);
					//				}
					//				else
					//				{
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
					//case IDC_CHECK_NEWBSMSE:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_NEWBSMSE))
					//				{
					//					m_con.calcType = New_BSMSE;
					//					UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_NEWBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_SIMILARITY_MEASURE);
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
					//case IDC_CHECK_OLDBSMSE_NEWEQUATION:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_OLDBSMSE_NEWEQUATION))
					//				{
					//					m_con.calcType = New_BSMSE;
					//					UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_SIMILARITY_MEASURE);
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
					//case IDC_CHECK_NEWBSMSE_NEWEQUATION:
					//	{
					//		switch (wmEvent)
					//		{
					//		case BN_CLICKED:
					//			{
					//				if (IsDlgButtonChecked(IDC_CHECK_NEWBSMSE_NEWEQUATION))
					//				{
					//					m_con.calcType = New_BSMSE;
					//					UnCkeck(IDC_CHECK_EUCLIDEAN_DISTANCE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE);
					//					UnCkeck(IDC_CHECK_NEWBSMSE);
					//					UnCkeck(IDC_CHECK_OLDBSMSE_NEWEQUATION);
					//					UnCkeck(IDC_CHECK_SIMILARITY_MEASURE);
					//					GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);
					//					GetDlgItem(IDC_EDIT_BETA)->EnableWindow(FALSE);
					//				}
					//			}
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	break;
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


void CSegmentationDlg::SetCheck(int iCtrl)
{
	CButton* pCheck = (CButton*)GetDlgItem(iCtrl);
	pCheck->SetCheck(TRUE);
}

void CSegmentationDlg::UnCkeck(int iCtrl)
{
	CButton* pCheck = (CButton*)GetDlgItem(iCtrl);
	pCheck->SetCheck(FALSE);
}

void CSegmentationDlg::SetTipInfo()
{
	// tool tip
	EnableToolTips(TRUE);
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_PROCESS), _T("press to process"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_IMG_FILE), _T("press to choose an image file"));
	//m_tooltip.AddTool(GetDlgItem(IDC_EDIT_MIN_SIZE), _T("segment size smaller than this number will be forced to be merged into neighbour segments"));
	//m_tooltip.AddTool(GetDlgItem(IDC_CHECK_OLDBSMSE), _T("Square Root of Band Sum Mean Squared Error"));
	//m_tooltip.AddTool(GetDlgItem(IDC_CHECK_SIMILARITY_MEASURE), _T("Similarity measure with shape penalty"));
	//m_tooltip.AddTool(GetDlgItem(IDC_EDIT_ALPHA), _T("0 < alpha < 1 and alpha + beta <= 1"));
	//m_tooltip.AddTool(GetDlgItem(IDC_EDIT_BETA), _T("0 < beta < 1 and alpha + beta <= 1"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_ED), _T("Euclidean Distance"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_ED), _T("Euclidean Distance"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_SD), _T("Standard Deviation"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_SD), _T("Standard Deviation"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_SAM), _T("Spectral Angle Mapper"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_SAM), _T("Spectral Angle Mapper"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_RSE), _T("Root Squared Error"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_RSE), _T("Root Squared Error"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_RDV), _T("Rooted Difference in Variance"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_RDV), _T("Rooted Difference in Variance"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_RDSE), _T("Rooted Difference in Squared Error"));
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_PRE_RDSE), _T("Rooted Difference in Squared Error"));
	/*
		None = 0,
	Euclidean_Distance,
	Old_BSMSE,
	New_BSMSE,
	Old_BSMSE_New_Equation,
	New_BSMSE_New_Equation,
	Similarity_Measure,
	Standard_Deviation,
	Spectral_Angle_Mapper,
	Entropy_Measure,
	Root_Squared_Error,
	Rooted_Difference_in_Variance,
	Rooted_Difference_in_Squared_Error
	*/
	m_tooltip.SetTipBkColor(RGB(255, 255, 255));
	m_tooltip.SetTipTextColor(RGB(0, 0, 0));
	m_tooltip.SetDelayTime(500);
}



void CSegmentationDlg::OnBnClickedButtonMaskFile()
{
	// TODO: Add your control notification handler code here
	char szFilter[] = _T("Mask Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, NULL, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		m_strMAskFile = dlg.GetPathName();
	}

	GetDlgItem(IDC_EDIT_MASK_FILE)->SetWindowText(m_strMAskFile);
}


void CSegmentationDlg::OnBnClickedButtonLoadMask()
{
	// TODO: Add your control notification handler code here
	if (m_strMAskFile.IsEmpty())
	{
		AfxMessageBox(_T("Please Choose a Mask file first~!"));
	}

	bool b4Neighbour = false;
	int iCheck4 = IsDlgButtonChecked(IDC_CHECK_4NEIGHBOUR);
	int iCheck8 = IsDlgButtonChecked(IDC_CHECK_8NEIGHBOUR);

	if (!iCheck4 && !iCheck8)
	{
		AfxMessageBox("Please choose 4 Neighbours or 8 Neighbours.");
		return;
	}
	m_mapMask.clear();
	MaskReader ir;
	if (!ir.LoadFile(m_strMAskFile.GetBuffer(1024)))
	{
		AfxMessageBox(_T("Load Mask File failed."));
		return;
	}
	imshow(_T("mask"), ir.m_data);
	m_strMAskFile.ReleaseBuffer();

	if (iCheck4)
		b4Neighbour = true;
	switch (ir.m_data.depth())
	{
	case CV_8U:
		ir.GetMask<unsigned char>(m_mapMask, 255, b4Neighbour);
		//ir.test<int>();
		break;
	default:
		break;
	}
	//imshow(_T("mask"), ir.m_data);
	AfxMessageBox(_T("Mask File Loaded~!"));
}


void CSegmentationDlg::OnBnClickedButtonPreprocess()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDIT_THRESHOLD_PRE)->GetWindowText(str);
	m_con.threshold_pre = (float)atof(str);

	if (m_con.threshold_pre <= 0.0000001)
	{
		AfxMessageBox(_T("Invalid Threshold(Pre)"));
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

	//int iCheckResize = IsDlgButtonChecked(IDC_CHECK_RESIZE);
	//if (iCheckResize)
	//	m_datafile.m_bResize = true;

	GetDlgItem(IDC_EDIT_ITERATION)->GetWindowText(str);
	m_con.iteration = atoi(str);
	if (m_con.iteration < 1)
	{
		AfxMessageBox(_T("Invalid iteration"));
		return;
	}

	if (IsDlgButtonChecked(IDC_CHECK_PRE_ED))
		m_con.pre_calcType = Euclidean_Distance;
	else if (IsDlgButtonChecked(IDC_CHECK_PRE_SAM))
		m_con.pre_calcType = Spectral_Angle_Mapper;
	else if (IsDlgButtonChecked(IDC_CHECK_PRE_SD))
		m_con.pre_calcType = Standard_Deviation;
	else if (IsDlgButtonChecked(IDC_CHECK_PRE_RSE))
		m_con.pre_calcType = Root_Squared_Error;
	else if (IsDlgButtonChecked(IDC_CHECK_PRE_RDV))
		m_con.pre_calcType = Rooted_Difference_in_Variance;
	else if (IsDlgButtonChecked(IDC_CHECK_PRE_RDSE))
		m_con.pre_calcType = Rooted_Difference_in_Squared_Error;
	else
		m_con.pre_calcType = None;

	m_datafile.PreProcess(m_vecSeed, m_con, m_mapMask, m_traceinfo);
	//GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
	AfxMessageBox("PreProcess Finished~!");
}


void CSegmentationDlg::OnBnClickedButtonStatistic()
{
	// TODO: Add your control notification handler code here
	bool bSuc = m_datafile.m_Image.Statistic();

	//CStatisticDlg dlg;
	//dlg.SetImage(&m_datafile.m_Image);
	//dlg.DoModal();
	if (bSuc)
	{// modeless
		if (m_pDlg == NULL)
		{
			m_pDlg = new CStatisticDlg;
			m_pDlg->SetImage(&m_datafile.m_Image);
			// do not put the modeless dialog topmost
			m_pDlg->Create(IDD_DIALOG_STATISTIC, CWnd::GetDesktopWindow());
		}
		m_pDlg->ShowWindow(TRUE);
	}
}
