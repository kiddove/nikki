// TifReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TifReader.h"
#include "TifReaderDlg.h"

#include "gdal_priv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLCM_DIS 1  //灰度共生矩阵的统计距离  
#define GLCM_CLASS 8 //计算灰度共生矩阵的图像灰度值等级化  
#define GLCM_ANGLE_HORIZATION 0  //水平  
#define GLCM_ANGLE_VERTICAL   1  //垂直  
#define GLCM_ANGLE_DIGONAL    2  //对角  

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


// CTifReaderDlg dialog




CTifReaderDlg::CTifReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTifReaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTifReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTifReaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_READ, &CTifReaderDlg::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTifReaderDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_AVG, &CTifReaderDlg::OnBnClickedButtonAvg)
	ON_BN_CLICKED(IDC_BUTTON_PER, &CTifReaderDlg::OnBnClickedButtonPer)
END_MESSAGE_MAP()


// CTifReaderDlg message handlers

BOOL CTifReaderDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTifReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTifReaderDlg::OnPaint()
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
HCURSOR CTifReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTifReaderDlg::ReadSingleFile1(CString strFile, int margin)
{
	//CString strOutput;
	//int ipos = strFile.ReverseFind(_T('.'));
	//strOutput = strFile.Left(ipos);
	//strOutput += _T(".txt");

	//CStdioFile f;

	//try
	//{
	//	if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
	//	{
	//		DWORD dwError = ::GetLastError();
	//		CString strErr;
	//		strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
	//		TRACE(strErr);
	//	}
	//}
	//catch(CFileException* e)
	//{
	//	TCHAR szError[256];
	//	e->GetErrorMessage(szError, 256);
	//	//TRACE(_T("CFileException catched. [%s]\n"), szError);
	//	CString strErr;
	//	strErr.Format(_T("CFileException catched. [%s]\n"), szError);
	//	TRACE(strErr);
	//}

	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(strFile.GetBuffer(1024), GA_ReadOnly);
	strFile.ReleaseBuffer();

	if (poDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFile);
		AfxMessageBox(strErr);
		//f.Close();
		return;
	}

	std::vector<float> vecValue;
	int iRasterCount = poDataset->GetRasterCount();
	double adfGeoTransform[6];

	TRACE(_T("Drivers: %s/ %s\n"), poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

	if (poDataset->GetProjectionRef() != NULL)
	{
		TRACE(_T("Projection is \"{%s}\"\n"), poDataset->GetProjectionRef());
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		TRACE(_T("Origin(left, top) = (%.2f, %.2f)\n"), adfGeoTransform[0], adfGeoTransform[3]);
		TRACE(_T("Pixel Size = (%.2f, %.2f)\n"), adfGeoTransform[1], adfGeoTransform[5]);
	}
	
	// fetch a raster band

	GDALRasterBand *poBand;

	int iBlockXSize = poDataset->GetRasterXSize();
	int iBlockYSize = poDataset->GetRasterYSize();
	int bGotMin, bGotMax;//, iBlockXSize, iBlockYSize;
	double adfMinMax[2];

	for (int i = 1; i <= iRasterCount; i++)
	{
		//CString strInfo;
		//strInfo.Format(_T("------------------------------------------\r\nRaster : %d\r\n"), i);
		//f.WriteString(strInfo);
		poBand = poDataset->GetRasterBand(i);
		poBand->GetBlockSize(&iBlockXSize, &iBlockYSize);
		
		TRACE(_T("Block=%d x %d, Type=%s, ColorType=%s\n"), iBlockXSize, iBlockYSize,
			GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);

		if (!(bGotMin && bGotMax))
		{
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		}

		TRACE(_T("Min=%.2f, Max=%.2f\n"), adfMinMax[0], adfMinMax[1]);

		if (poBand->GetOverviewCount() > 0)
			TRACE(_T("Band has %d overviews.\n"), poBand->GetOverviewCount());

		if (poBand->GetColorTable() != NULL)
			TRACE(_T("Band has a color table with %d entries.\n"), poBand->GetColorTable()->GetColorEntryCount());



		// reading raster data
		float *pafScanline;
		int nXSize = poBand->GetXSize();
		int nYSize = poBand->GetYSize();
		pafScanline = (float*)CPLMalloc(sizeof(float)*nXSize*nYSize);
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0);

		// compute && output
		// y * 501 + x -- y,x are index
		//int m_iStandsize = 100;
		int X = nXSize / m_iStandsize;
		int Y = nYSize / m_iStandsize;

		for (int xx = 0; xx < X; xx++)
		{
			for (int yy = 0; yy < Y; yy++)
			{
				float fMax, fMin, fAvg, fSum;
				fMax = -1e8;
				fMin = 1e8;
				fAvg = fSum = 0.0;
				// 0, 100 --- 0, 100

				TRACE(_T("x[%d, %d];y[%d, %d]\r\n"), xx * m_iStandsize, (xx+1)*m_iStandsize, yy* m_iStandsize, (yy+1)*m_iStandsize);

				int iNum = 0;
				for (int x = xx * m_iStandsize + margin; x < (xx + 1) * m_iStandsize - margin + 1; x++)
				{
					for (int y = yy * m_iStandsize + margin; y < (yy + 1) * m_iStandsize - margin + 1; y++)
					{
						if (_isnan(pafScanline[y * nYSize + x]) == 0)
						{
							if (pafScanline[y * nYSize + x] > fMax)
								fMax = pafScanline[y * nYSize + x];

							if (pafScanline[y * nYSize + x] < fMin)
								fMin = pafScanline[y * nYSize + x];

							fSum += pafScanline[y * nYSize + x];
							iNum++;
						}
					}
				}
				fAvg = fSum / (float)iNum;

				TRACE(_T("%.4f\t%.4f\t%.4f, %.4f\r\n"), fMax, fMin, fAvg, fSum);

				//strInfo.Format(_T("Max : %.6f, Min : %.6f, Avg : %.6f\r\n"), fMax, fMin, fAvg);
				//strInfo.Format(_T("%.6f\r\n"), fAvg);
				//f.WriteString(strInfo);
				vecValue.push_back(fAvg);
			}
		}
		// 0, 100 --- 101, 200

		CPLFree(pafScanline);
		//f.Flush();
	}

	GDALClose(poDataset);
	//f.Close();
	m_vecValue.push_back(vecValue);
}


void CTifReaderDlg::ReadSingleFile2(CString strFile, int interval, int margin)
{
	//CString strOutput;
	//int ipos = strFile.ReverseFind(_T('.'));
	//strOutput = strFile.Left(ipos);
	//strOutput += _T(".txt");

	//CStdioFile f;

	//try
	//{
	//	if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
	//	{
	//		DWORD dwError = ::GetLastError();
	//		CString strErr;
	//		strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
	//		TRACE(strErr);
	//	}
	//}
	//catch(CFileException* e)
	//{
	//	TCHAR szError[256];
	//	e->GetErrorMessage(szError, 256);
	//	//TRACE(_T("CFileException catched. [%s]\n"), szError);
	//	CString strErr;
	//	strErr.Format(_T("CFileException catched. [%s]\n"), szError);
	//	TRACE(strErr);
	//}

	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(strFile.GetBuffer(1024), GA_ReadOnly);
	strFile.ReleaseBuffer();

	if (poDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFile);
		AfxMessageBox(strErr);
		//f.Close();
		return;
	}

	std::vector<float> vecValue;
	int iRasterCount = poDataset->GetRasterCount();
	double adfGeoTransform[6];

	TRACE(_T("Drivers: %s/ %s\n"), poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

	if (poDataset->GetProjectionRef() != NULL)
	{
		TRACE(_T("Projection is \"{%s}\"\n"), poDataset->GetProjectionRef());
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		TRACE(_T("Origin(left, top) = (%.2f, %.2f)\n"), adfGeoTransform[0], adfGeoTransform[3]);
		TRACE(_T("Pixel Size = (%.2f, %.2f)\n"), adfGeoTransform[1], adfGeoTransform[5]);
	}
	
	// fetch a raster band

	GDALRasterBand *poBand;

	int iBlockXSize = poDataset->GetRasterXSize();
	int iBlockYSize = poDataset->GetRasterYSize();
	int bGotMin, bGotMax;//, iBlockXSize, iBlockYSize;
	double adfMinMax[2];

	for (int i = 1; i <= iRasterCount; i++)
	{
		//CString strInfo;
		//strInfo.Format(_T("------------------------------------------\r\nRaster : %d\r\n"), i);
		//f.WriteString(strInfo);
		poBand = poDataset->GetRasterBand(i);
		poBand->GetBlockSize(&iBlockXSize, &iBlockYSize);

		TRACE(_T("Block=%d x %d, Type=%s, ColorType=%s\n"), iBlockXSize, iBlockYSize,
			GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);

		if (!(bGotMin && bGotMax))
		{
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		}

		TRACE(_T("Min=%.2f, Max=%.2f\n"), adfMinMax[0], adfMinMax[1]);

		if (poBand->GetOverviewCount() > 0)
			TRACE(_T("Band has %d overviews.\n"), poBand->GetOverviewCount());

		if (poBand->GetColorTable() != NULL)
			TRACE(_T("Band has a color table with %d entries.\n"), poBand->GetColorTable()->GetColorEntryCount());



		// reading raster data
		float *pafScanline;
		int nXSize = poBand->GetXSize();
		int nYSize = poBand->GetYSize();
		pafScanline = (float*)CPLMalloc(sizeof(float)*nXSize*nYSize);
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0);

		// compute && output
		// y * 501 + x -- y,x are index
		//int m_iStandsize = 100;
		int X = nXSize / m_iStandsize;
		int Y = nYSize / m_iStandsize;

		m_iBlocksize = X*Y;

		for (int xx = 0; xx < X; xx++)
		{
			for (int yy = 0; yy < Y; yy++)
			{
				float fMax, fMin, fAvg, fSum;
				fMax = -1e8;
				fMin = 1e8;
				fAvg = fSum = 0.0;
				// 0, 100 --- 0, 100

				TRACE(_T("x[%d, %d];y[%d, %d]\r\n"), xx * m_iStandsize, (xx+1)*m_iStandsize, yy* m_iStandsize, (yy+1)*m_iStandsize);

				int iNum = 0;
				for (int x = xx * m_iStandsize + margin; x < (xx + 1) * m_iStandsize - margin + 1; x+=interval)
				{
					for (int y = yy * m_iStandsize + margin; y < (yy + 1) * m_iStandsize - margin + 1; y+=interval)
					{
						if (_isnan(pafScanline[y * nYSize + x]) == 0)
						{
							if (pafScanline[y * nYSize + x] > fMax)
								fMax = pafScanline[y * nYSize + x];

							if (pafScanline[y * nYSize + x] < fMin)
								fMin = pafScanline[y * nYSize + x];

							fSum += pafScanline[y * nYSize + x];
							iNum++;
						}
					}
				}
				fAvg = fSum / (float)iNum;

				TRACE(_T("%.4f\t%.4f\t%.4f, %.4f\r\n"), fMax, fMin, fAvg, fSum);

				//strInfo.Format(_T("Max : %.6f, Min : %.6f, Avg : %.6f\r\n"), fMax, fMin, fAvg);
				//strInfo.Format(_T("%.6f\r\n"), fAvg);
				//f.WriteString(strInfo);
				vecValue.push_back(fAvg);
			}
		}
		// 0, 100 --- 101, 200

		CPLFree(pafScanline);
		//f.Flush();
	}

	GDALClose(poDataset);
	//f.Close();
	m_vecValue.push_back(vecValue);
}


void CTifReaderDlg::ReadSingleFile3(CString strFile)
{
	//CString strOutput;
	//int ipos = strFile.ReverseFind(_T('.'));
	//strOutput = strFile.Left(ipos);
	//strOutput += _T(".txt");

	//CStdioFile f;

	//try
	//{
	//	if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
	//	{
	//		DWORD dwError = ::GetLastError();
	//		CString strErr;
	//		strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
	//		TRACE(strErr);
	//	}
	//}
	//catch(CFileException* e)
	//{
	//	TCHAR szError[256];
	//	e->GetErrorMessage(szError, 256);
	//	//TRACE(_T("CFileException catched. [%s]\n"), szError);
	//	CString strErr;
	//	strErr.Format(_T("CFileException catched. [%s]\n"), szError);
	//	TRACE(strErr);
	//}

	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(strFile.GetBuffer(1024), GA_ReadOnly);
	strFile.ReleaseBuffer();

	if (poDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFile);
		AfxMessageBox(strErr);
		//f.Close();
		return;
	}

	std::vector<float> vecValue;
	int iRasterCount = poDataset->GetRasterCount();
	double adfGeoTransform[6];

	TRACE(_T("Drivers: %s/ %s\n"), poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

	if (poDataset->GetProjectionRef() != NULL)
	{
		TRACE(_T("Projection is \"{%s}\"\n"), poDataset->GetProjectionRef());
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		TRACE(_T("Origin(left, top) = (%.2f, %.2f)\n"), adfGeoTransform[0], adfGeoTransform[3]);
		TRACE(_T("Pixel Size = (%.2f, %.2f)\n"), adfGeoTransform[1], adfGeoTransform[5]);
	}
	
	// fetch a raster band

	GDALRasterBand *poBand;

	int iBlockXSize = poDataset->GetRasterXSize();
	int iBlockYSize = poDataset->GetRasterYSize();
	int bGotMin, bGotMax;//, iBlockXSize, iBlockYSize;
	double adfMinMax[2];

	for (int i = 1; i <= iRasterCount; i++)
	{
		//CString strInfo;
		//strInfo.Format(_T("------------------------------------------\r\nRaster : %d\r\n"), i);
		//f.WriteString(strInfo);
		poBand = poDataset->GetRasterBand(i);
		poBand->GetBlockSize(&iBlockXSize, &iBlockYSize);

		TRACE(_T("Block=%d x %d, Type=%s, ColorType=%s\n"), iBlockXSize, iBlockYSize,
			GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);

		if (!(bGotMin && bGotMax))
		{
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		}

		TRACE(_T("Min=%.2f, Max=%.2f\n"), adfMinMax[0], adfMinMax[1]);

		if (poBand->GetOverviewCount() > 0)
			TRACE(_T("Band has %d overviews.\n"), poBand->GetOverviewCount());

		if (poBand->GetColorTable() != NULL)
			TRACE(_T("Band has a color table with %d entries.\n"), poBand->GetColorTable()->GetColorEntryCount());



		// reading raster data
		float *pafScanline;
		int nXSize = poBand->GetXSize();
		int nYSize = poBand->GetYSize();
		pafScanline = (float*)CPLMalloc(sizeof(float)*nXSize*nYSize);
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0);

		// compute && output
		// y * 501 + x -- y,x are index
		//int m_iStandsize = 100;
		int X = nXSize / m_iStandsize;
		int Y = nYSize / m_iStandsize;
		m_iBlocksize = X*Y;

		for (int xx = 0; xx < X; xx++)
		{
			for (int yy = 0; yy < Y; yy++)
			{
				TRACE(_T("x[%d, %d];y[%d, %d]\r\n"), xx * m_iStandsize, (xx+1)*m_iStandsize, yy* m_iStandsize, (yy+1)*m_iStandsize);

				int iNum0 = 0;
				int iNum1 = 0;
				float fPercent = 0.0;
				for (int x = xx * m_iStandsize; x < (xx + 1) * m_iStandsize; x++)
				{
					for (int y = yy * m_iStandsize; y < (yy + 1) * m_iStandsize; y++)
					{
						if (pafScanline[y * nYSize + x] == 0)
							iNum0++;
						else if(pafScanline[y * nYSize + x] == 1)
							iNum1++;
						else
							TRACE(_T("value invalid[x:%d, y%d, value:%d]"), x, y, pafScanline[y * nYSize + x]);
					}
				}

				fPercent = (float)iNum0 / (m_iStandsize * m_iStandsize);

				//strInfo.Format(_T("Max : %.6f, Min : %.6f, Avg : %.6f\r\n"), fMax, fMin, fAvg);
				//strInfo.Format(_T("%.6f\r\n"), fAvg);
				//f.WriteString(strInfo);
				vecValue.push_back(fPercent);
			}
		}
		// 0, 100 --- 101, 200

		CPLFree(pafScanline);
		//f.Flush();
	}

	GDALClose(poDataset);
	//f.Close();
	m_vecValue.push_back(vecValue);
}

void CTifReaderDlg::OnBnClickedButtonRead()
{
	m_vecValue.clear();
	m_vecFile.clear();
	CString str;
	GetDlgItem(IDC_EDIT_MARGIN)->GetWindowText(str);
	int iMargin = atoi(str.GetBuffer(1024));
	str.ReleaseBuffer();

	iMargin -= 1;
	if (iMargin <= 0)
	{
		AfxMessageBox(_T("Please Input Valid Margin Value"));
		return;
	}

	if (m_strPath.IsEmpty())
	{
		AfxMessageBox(_T("Please Choose a Folder first!"));
		return;
	}
	// TODO: Add your control notification handler code here
	CFileFind ff;
	CString strFile = m_strPath + _T("\\*.tif");
	BOOL bFind = ff.FindFile(strFile);

	while (bFind)
	{
		bFind = ff.FindNextFile();

		if (!ff.IsDirectory() && ! ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();
			m_vecFile.push_back(ff.GetFileName());
			//ReadSingleFile1(strFile, iMargin);
			ReadSingleFile4(strFile);
		}
	}

	if (m_vecValue.empty())
		return;

	int iSize = m_vecValue.size();
	int iCount = 0;
	for (int i = 0; i < iSize; i++)
	{
		if (iCount == 0)
			iCount = m_vecValue[i].size();
		else if (iCount != m_vecValue[i].size())
		{
			ASSERT(FALSE);
			return;
		}
	}

	CStdioFile f;

	CString strOutput = m_strPath + _T("\\result_overall.txt");
	try
	{
		if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
			TRACE(strErr);
		}

		// 
		CString strInfo;

		for (int k = 0; k < m_vecFile.size(); k++)
		{
			CString str;
			str.Format(_T("%s, "), m_vecFile[k]);
			strInfo += str;
		}
		int l = strInfo.GetLength();
		strInfo = strInfo.Left(l - 2);
		strInfo += _T("\n");
		f.WriteString(strInfo);
	
		for (int j = 0; j < iCount; j++)
		{
			strInfo.Empty();
			for (int i = 0; i < iSize; i++)
			{
				CString str;
				str.Format(_T("%.6f, "), m_vecValue[i][j]);
				strInfo += str;
			}
			int len = strInfo.GetLength();
			strInfo = strInfo.Left(len - 2);
			strInfo += _T("\n");
			f.WriteString(strInfo);
		}

		f.Close();
	}
	catch(CFileException* e)
	{
		TCHAR szError[256];
		e->GetErrorMessage(szError, 256);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		TRACE(strErr);
	}
}

void CTifReaderDlg::OnBnClickedButtonOpen()
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

void CTifReaderDlg::OnBnClickedButtonAvg()
{
	// TODO: Add your control notification handler code here
	m_vecValue.clear();
	m_vecFile.clear();
	CString str;
	GetDlgItem(IDC_EDIT_INTER)->GetWindowText(str);
	int iInterval = atoi(str.GetBuffer(1024));

	if (iInterval <= 0)
	{
		AfxMessageBox(_T("Please Input Valid Interval Value"));
		return;
	}

	str.ReleaseBuffer();
	str.Empty();
	GetDlgItem(IDC_EDIT_MARGIN)->GetWindowText(str);
	int iMargin = atoi(str.GetBuffer(1024));
	str.ReleaseBuffer();

	str.Empty();
	GetDlgItem(IDC_EDIT_STANDSIZE)->GetWindowText(str);
	m_iStandsize = atoi(str.GetBuffer(1024));
	str.ReleaseBuffer();


	iMargin -= 1;
	if (iMargin <= 0)
	{
		AfxMessageBox(_T("Please Input Valid Margin Value"));
		return;
	}

	if (m_strPath.IsEmpty())
	{
		AfxMessageBox(_T("Please Choose a Folder first!"));
		return;
	}

	// TODO: Add your control notification handler code here
	CFileFind ff;
	CString strFile = m_strPath + _T("\\*.tif");
	BOOL bFind = ff.FindFile(strFile);

	while (bFind)
	{
		bFind = ff.FindNextFile();

		if (!ff.IsDirectory() && ! ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();
			m_vecFile.push_back(ff.GetFileName());
			ReadSingleFile2(strFile, iInterval, iMargin);
		}
	}

	if (m_vecValue.empty())
		return;

	int iSize = m_vecValue.size();
	int iCount = 0;
	for (int i = 0; i < iSize; i++)
	{
		if (iCount == 0)
			iCount = m_vecValue[i].size();
		else if (iCount != m_vecValue[i].size())
		{
			ASSERT(FALSE);
			return;
		}
	}

	CStdioFile f;

	CString strOutput = m_strPath + _T("\\result_block.txt");
	try
	{
		if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
			TRACE(strErr);
		}

		// 
		CString strInfo;

		for (int k = 0; k < m_vecFile.size(); k++)
		{
			CString str;
			str.Format(_T("%s, "), m_vecFile[k]);
			strInfo += str;
		}
		int l = strInfo.GetLength();
		strInfo = strInfo.Left(l - 2);
		strInfo += _T("\n");
		f.WriteString(strInfo);

		int iColumn = iCount / m_iBlocksize;
		// 其实就是band的size 应该是8 不会变
		// 即按band分列 每一列 前m_iBlocksize个是第一个文件的, 一共m_iBlocksize * 文件个数(iCount) 行

		//m_vecValue[][] 第一维是文件个数(iCount), 第二维是数据个数(band * m_iBlocksize)

		// row
		for (int row = 0; row < iSize * m_iBlocksize; row++)
		{
			strInfo.Empty();
			for (int col = 0; col < iColumn; col++)
			{
				int fileno = row / m_iBlocksize;
				int valueno = col * m_iBlocksize + row % m_iBlocksize;
				CString str;
				str.Format(_T("%.6f, "), m_vecValue[fileno][valueno]);
				strInfo += str;
			}
			int len = strInfo.GetLength();
			strInfo = strInfo.Left(len - 2);
			strInfo += _T("\n");
			f.WriteString(strInfo);
		}
		// column

		//for (int j = 0; j < iCount; j++)
		//{
		//	strInfo.Empty();
		//	for (int i = 0; i < iSize; i++)
		//	{
		//		CString str;
		//		str.Format(_T("%.6f, "), m_vecValue[i][j]);
		//		strInfo += str;
		//	}
		//	int len = strInfo.GetLength();
		//	strInfo = strInfo.Left(len - 2);
		//	strInfo += _T("\n");
		//	f.WriteString(strInfo);
		//}

		f.Close();
	}
	catch(CFileException* e)
	{
		TCHAR szError[256];
		e->GetErrorMessage(szError, 256);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		TRACE(strErr);
	}
}

void CTifReaderDlg::OnBnClickedButtonPer()
{
	// TODO: Add your control notification handler code here
	m_vecValue.clear();
	m_vecFile.clear();
	CString str;
	GetDlgItem(IDC_EDIT_STANDSIZE)->GetWindowText(str);
	m_iStandsize = atoi(str.GetBuffer(1024));
	str.ReleaseBuffer();
	if (m_strPath.IsEmpty())
	{
		AfxMessageBox(_T("Please Choose a Folder first!"));
		return;
	}
	// TODO: Add your control notification handler code here
	CFileFind ff;
	CString strFile = m_strPath + _T("\\*.tif");
	BOOL bFind = ff.FindFile(strFile);

	while (bFind)
	{
		bFind = ff.FindNextFile();

		if (!ff.IsDirectory() && ! ff.IsDots())
		{
			CString strFile;
			strFile = ff.GetFilePath();
			m_vecFile.push_back(ff.GetFileName());
			ReadSingleFile3(strFile);
		}
	}

	if (m_vecValue.empty())
		return;

	int iSize = m_vecValue.size();
	int iCount = 0;
	for (int i = 0; i < iSize; i++)
	{
		if (iCount == 0)
			iCount = m_vecValue[i].size();
		else if (iCount != m_vecValue[i].size())
		{
			ASSERT(FALSE);
			return;
		}
	}

	CStdioFile f;

	CString strOutput = m_strPath + _T("\\result_shadow.txt");
	try
	{
		if (!f.Open(strOutput, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone))
		{
			DWORD dwError = ::GetLastError();
			CString strErr;
			strErr.Format(_T("ReadDEM --- open file failed.(Error:%d, path:%s)\n"), dwError, strOutput);
			TRACE(strErr);
		}

		// 
		CString strInfo;

		for (int k = 0; k < m_vecFile.size(); k++)
		{
			CString str;
			str.Format(_T("%s, "), m_vecFile[k]);
			strInfo += str;
		}
		int l = strInfo.GetLength();
		strInfo = strInfo.Left(l - 2);
		strInfo += _T("\n");
		f.WriteString(strInfo);
	
		int iColumn = iCount / m_iBlocksize;
		// 其实就是band的size 应该是8 不会变
		// 即按band分列 每一列 前m_iBlocksize个是第一个文件的, 一共m_iBlocksize * 文件个数(iCount) 行

		//m_vecValue[][] 第一维是文件个数(iCount), 第二维是数据个数(band * m_iBlocksize)

		// row
		for (int row = 0; row < iSize * m_iBlocksize; row++)
		{
			strInfo.Empty();
			for (int col = 0; col < iColumn; col++)
			{
				int fileno = row / m_iBlocksize;
				int valueno = col * m_iBlocksize + row % m_iBlocksize;
				CString str;
				str.Format(_T("%.6f, "), m_vecValue[fileno][valueno]);
				strInfo += str;
			}
			int len = strInfo.GetLength();
			strInfo = strInfo.Left(len - 2);
			strInfo += _T("\n");
			f.WriteString(strInfo);
		}

		f.Close();
	}
	catch(CFileException* e)
	{
		TCHAR szError[256];
		e->GetErrorMessage(szError, 256);
		//TRACE(_T("CFileException catched. [%s]\n"), szError);
		CString strErr;
		strErr.Format(_T("CFileException catched. [%s]\n"), szError);
		TRACE(strErr);
	}
}

void CTifReaderDlg::ReadSingleFile4(CString strFile)
{
	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(strFile.GetBuffer(1024), GA_ReadOnly);
	strFile.ReleaseBuffer();

	if (poDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFile);
		AfxMessageBox(strErr);
		//f.Close();
		return;
	}

	std::vector<float> vecValue;
	int iRasterCount = poDataset->GetRasterCount();
	double adfGeoTransform[6];

	TRACE(_T("Drivers: %s/ %s\n"), poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

	if (poDataset->GetProjectionRef() != NULL)
	{
		TRACE(_T("Projection is \"{%s}\"\n"), poDataset->GetProjectionRef());
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		TRACE(_T("Origin(left, top) = (%.2f, %.2f)\n"), adfGeoTransform[0], adfGeoTransform[3]);
		TRACE(_T("Pixel Size = (%.2f, %.2f)\n"), adfGeoTransform[1], adfGeoTransform[5]);
	}
	
	// fetch a raster band

	GDALRasterBand *poBand;

	int iBlockXSize = poDataset->GetRasterXSize();
	int iBlockYSize = poDataset->GetRasterYSize();
	int bGotMin, bGotMax;//, iBlockXSize, iBlockYSize;
	double adfMinMax[2];
	int angleDirection;
	double featureVector[5] = {0};

//	for (int i = 1; i <= iRasterCount; i++)
//	{
		//CString strInfo;
		//strInfo.Format(_T("------------------------------------------\r\nRaster : %d\r\n"), i);
		//f.WriteString(strInfo);
	
		poBand = poDataset->GetRasterBand(1);
		GDALDataType iType = poBand->GetRasterDataType();
		int max_value = 0;
		switch (iType)
		{
		case GDT_Byte:
			max_value = 256;
			break;
		case GDT_UInt16:
			max_value = 65536;
			break;
		default:
			break;
		}
		poBand->GetBlockSize(&iBlockXSize, &iBlockYSize);

		TRACE(_T("Block=%d x %d, Type=%s, ColorType=%s\n"), iBlockXSize, iBlockYSize,
			GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);

		if (!(bGotMin && bGotMax))
		{
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		}

		TRACE(_T("Min=%.2f, Max=%.2f\n"), adfMinMax[0], adfMinMax[1]);

		if (poBand->GetOverviewCount() > 0)
			TRACE(_T("Band has %d overviews.\n"), poBand->GetOverviewCount());

		if (poBand->GetColorTable() != NULL)
			TRACE(_T("Band has a color table with %d entries.\n"), poBand->GetColorTable()->GetColorEntryCount());



		// reading raster data
		unsigned short *pafScanline;
		int nXSize = poBand->GetXSize();
		int nYSize = poBand->GetYSize();
		pafScanline = (unsigned short*)CPLMalloc(sizeof(unsigned short)*nXSize*nYSize);
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, iType, 0, 0);

		int * glcm = new int[GLCM_CLASS * GLCM_CLASS];  
		int * histImage = new int[nXSize * nYSize]; 
		int width = nXSize;
		int height = nYSize;
		int i,j;  

		//灰度等级化---分GLCM_CLASS个等级  
		for(i = 0;i < height;i++){  
			for(j = 0;j < width;j++){  
				histImage[i * width + j] = (int)(pafScanline[i*width+j] * GLCM_CLASS / max_value) + 1;  
				TRACE(_T("%d, "), histImage[i * width + j]);
			}  
			TRACE(_T("\n"));
		}  

		//初始化共生矩阵  
		for (i = 0;i < GLCM_CLASS;i++)  
			for (j = 0;j < GLCM_CLASS;j++)  
				glcm[i * GLCM_CLASS + j] = 0;  

		//计算灰度共生矩阵  
		int k,l;  
		//水平方向  
		angleDirection = GLCM_ANGLE_HORIZATION;
		if(angleDirection == GLCM_ANGLE_HORIZATION)  
		{  
			for (i = 0;i < height;i++)  
			{  
				for (j = 0;j < width;j++)  
				{  
					l = histImage[i * width + j];  
					if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width)  
					{  
						k = histImage[i * width + j + GLCM_DIS];  
						glcm[(l - 1) * GLCM_CLASS + k - 1]++;  
					}  
	/*				if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width)  
					{  
						k = histImage[i * width + j - GLCM_DIS];  
						glcm[(l - 1) * GLCM_CLASS + k - 1]++;  
					}  */
				}  
			}  

			CStdioFile f;
			f.Open("E:\\Code\\Github\\nikki\\GLCM\\result.txt", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);
			CString str1, str2;

			f.WriteString("H\n");

			for (int i = 0; i < GLCM_CLASS; i++)
			{
				str1.Empty();
				for (int j =0 ; j < GLCM_CLASS; j++)
				{
					str2.Empty();
					str2.Format(_T("%d, "), glcm[i * GLCM_CLASS + j]);
					str1 += str2;
				}
				str1 += _T("\n");
				f.WriteString(str1);
			}

			f.Close();

		}  
		//垂直方向  
		else if(angleDirection == GLCM_ANGLE_VERTICAL)  
		{  
			for (i = 0;i < height;i++)  
			{  
				for (j = 0;j < width;j++)  
				{  
					l = histImage[i * width + j];  
					if(i + GLCM_DIS >= 0 && i + GLCM_DIS < height)   
					{  
						k = histImage[(i + GLCM_DIS) * width + j];  
						glcm[l * GLCM_CLASS + k]++;  
					}  
					if(i - GLCM_DIS >= 0 && i - GLCM_DIS < height)   
					{  
						k = histImage[(i - GLCM_DIS) * width + j];  
						glcm[l * GLCM_CLASS + k]++;  
					}  
				}  
			}  
		}  
		//对角方向  
		else if(angleDirection == GLCM_ANGLE_DIGONAL)  
		{  
			for (i = 0;i < height;i++)  
			{  
				for (j = 0;j < width;j++)  
				{  
					l = histImage[i * width + j];  

					if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width && i + GLCM_DIS >= 0 && i + GLCM_DIS < height)  
					{  
						k = histImage[(i + GLCM_DIS) * width + j + GLCM_DIS];  
						glcm[l * GLCM_CLASS + k]++;  
					}  
					if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width && i - GLCM_DIS >= 0 && i - GLCM_DIS < height)  
					{  
						k = histImage[(i - GLCM_DIS) * width + j - GLCM_DIS];  
						glcm[l * GLCM_CLASS + k]++;  
					}  
				}  
			}  
		}  

		//计算特征值  
		double entropy = 0,energy = 0,contrast = 0,homogenity = 0;  
		for (i = 0;i < GLCM_CLASS;i++)  
		{  
			for (j = 0;j < GLCM_CLASS;j++)  
			{  
				//熵  
				if(glcm[i * GLCM_CLASS + j] > 0)  
					entropy -= glcm[i * GLCM_CLASS + j] * log10(double(glcm[i * GLCM_CLASS + j]));  
				//能量  
				energy += glcm[i * GLCM_CLASS + j] * glcm[i * GLCM_CLASS + j];  
				//对比度  
				contrast += (i - j) * (i - j) * glcm[i * GLCM_CLASS + j];  
				//一致性  
				homogenity += 1.0 / (1 + (i - j) * (i - j)) * glcm[i * GLCM_CLASS + j];  
			}  
		}  
		//返回特征值  
		i = 0;  
		featureVector[i++] = entropy;  
		featureVector[i++] = energy;  
		featureVector[i++] = contrast;  
		featureVector[i++] = homogenity;  

		delete[] glcm;  
		delete[] histImage;  
		CPLFree(pafScanline);
		//f.Flush();
//	}

	GDALClose(poDataset);
	//f.Close();
}