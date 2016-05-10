
// calcPCADlg.cpp : implementation file
//

#include "stdafx.h"
#include "calcPCA.h"
#include "calcPCADlg.h"
#include "afxdialogex.h"
#include <fstream>

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcalcPCADlg dialog



CcalcPCADlg::CcalcPCADlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcalcPCADlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strFolder = "";
}

void CcalcPCADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcalcPCADlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CcalcPCADlg::OnBnClickedButtonChoose)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CcalcPCADlg::OnBnClickedButtonCalculate)
END_MESSAGE_MAP()


// CcalcPCADlg message handlers

BOOL CcalcPCADlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

void CcalcPCADlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CcalcPCADlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CcalcPCADlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

float CcalcPCADlg::GetValue(Mat& mat, int r, int c, int b)
{
	if (!mat.isContinuous())
		return 0.0;
	if (r < 0 || r >= mat.rows)
		return 0.0;
	if (c < 0 || c >= mat.cols)
		return 0.0;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return 0.0;

	float * p = (float*)mat.ptr(r);

	float f = (float)p[c * iChannel + b];
	return f;
}

void CcalcPCADlg::SetValue(Mat& mat, int r, int c, int b, float& fValue)
{
	if (!mat.isContinuous())
		return;
	if (r < 0 || r >= mat.rows)
		return;
	if (c < 0 || c >= mat.cols)
		return;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return;

	float * p = (float*)mat.ptr(r);

	//float f = (float)p[c * iChannel + b];
	//return f;
	p[c * iChannel + b] = fValue;
}

void CcalcPCADlg::OutputFile(CString strFile, Mat& result, GDALDataset* pDataset)
{
	if (result.type() != 5)
		return;

	const char* pszFormat = "GTiff";
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	char** papszMetadata;

	if (poDriver == NULL)
	{
		return;
	}

	papszMetadata = poDriver->GetMetadata();
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
		printf(("Driver %s supports Create() method.\n"), pszFormat );
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
		printf(("Driver %s supports CreateCopy() method.\n"), pszFormat );

	// create file
	GDALDataset* poDstDS;
	char** papszOptions = NULL;
	//poDstDS = poDriver->Create(strFile.GetBuffer(1024), width, height, 1, GDT_UInt16, papszMetadata);
	poDstDS = poDriver->Create(strFile.GetBuffer(1024), result.cols, result.rows, 1, GDT_Float32, papszMetadata);
	strFile.ReleaseBuffer();

	double adfGeoTransform[6];
	if (pDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		poDstDS->SetGeoTransform(adfGeoTransform);
	}
	//else
	//{
	//	AfxMessageBox(_T("Output:Can not GetGeoTransform."));
	//	GDALClose( (GDALDatasetH) poDstDS );
	//	return;
	//}
	const char* pszProjection = pDataset->GetProjectionRef();
	poDstDS->SetProjection(pszProjection);

	for (int k = 1; k < 2; k++)
	{
		GDALRasterBand* poBand = poDstDS->GetRasterBand(k);
		if (poBand == NULL)
		{
			GDALClose( (GDALDatasetH) poDstDS );
			return;
		}

		//unsigned short* pData = new unsigned short[width * height];
		float* pData = new float[result.rows * result.cols];


		for (int r = 0; r < result.rows; r++)
		{
			for (int c = 0; c < result.cols; c++)
			{
				int index = r * result.cols + c;
				pData[index] = GetValue(result, r, c, 0);
			}
		}

		char* pszData = (char*)pData;
		poBand->RasterIO(GF_Write, 0, 0, result.cols, result.rows, pszData, result.cols, result.rows, GDT_Float32, 0, 0);
		delete pData;
	}

	/* Once we're done, close properly the dataset */
	GDALClose( (GDALDatasetH) poDstDS );
}

void CcalcPCADlg::OnBnClickedButtonChoose()
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

	if(pidl == NULL)
		return;

	if(!SHGetPathFromIDList(pidl, szDir))
		return;

	else
		m_strFolder = szDir;

	UpdateData(FALSE);
}


void CcalcPCADlg::OnBnClickedButtonCalculate()
{
	// TODO: Add your control notification handler code here
	if (m_strFolder.IsEmpty())
		return;
	CFileFind ff;
	CString strFile = m_strFolder + _T("\\*.tif");
	BOOL bFind = ff.FindFile(strFile);

	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			processFile(ff.GetFilePath());
		}
	}

	AfxMessageBox(_T("All Finished."));
}

void CcalcPCADlg::processFile(CString strFile)
{
	ImageReader image;
	image.LoadFile(strFile.GetBuffer(1024));
	strFile.ReleaseBuffer();
	image.CloseFile();

	//outputMatrix("original", image.m_data);
	int cn = image.m_data.channels();
	int iType = image.m_data.type();

	//unsigned short v = GetValueU16(image.m_data, 0, 0, 0);
	cv::Mat rscv = ReshapeByOpenCV(image.m_data);

	//cv::Mat tobeDone = ReshapeByMe(image.m_data);
	cv::Mat tobeDone = rscv.clone();

	/*cv::Mat ref = image.m_data.reshape(1, image.m_data.rows * image.m_data.cols);
	cv::Mat tobeDone = ref.clone();*/

	//int nCount = cv::countNonZero(tobeDone);
	cn = tobeDone.channels();

	iType = tobeDone.type();

	try {
		PCA pca_analysis(tobeDone, Mat(), CV_PCA_DATA_AS_ROW);
		Mat transposeMat = pca_analysis.eigenvectors.t();
		//outputMatrix("eigenvectors", pca_analysis.eigenvectors);
		//outputMatrix("eigenvalues", pca_analysis.eigenvalues);
		//outputMatrix("transposeMat", transposeMat);
		iType = pca_analysis.eigenvectors.type();

		Mat mult = tobeDone * transposeMat;

		// if reshapebyme, should fill the zero back
		FillBackZero(mult, rscv);

		//// if reshape by opencv
		//cv::Mat ref = mult.reshape(4, image.m_data.rows);
		// if reshape by me
		cv::Mat ref = rscv.reshape(4, image.m_data.rows);
		Mat result = ref.clone();
		std::vector<Mat> channels;
		split(result, channels);

		Mat print = channels[0];
		//outputMatrix("result", print);

		int iType = print.type();
		int iDepth = print.depth();

		CString strOutput;
		int iPos = strFile.ReverseFind(_T('.'));
		strOutput = strFile.Left(iPos);
		strOutput += _T("p1.tif.result");

		GDALDataset* pData = (GDALDataset*)GDALOpen(strFile.GetBuffer(1024), GA_ReadOnly);
		strFile.ReleaseBuffer();

		OutputFile(strOutput, print, pData);
		strOutput.ReleaseBuffer();

		//Mat wtf = imread(strOutput.GetBuffer(1024), CV_LOAD_IMAGE_UNCHANGED);
		//strOutput.ReleaseBuffer();
		//float f = GetValue(wtf, 0, 0, 0);
		//float f1 = GetValue(print, 0, 0, 0);
	} catch (cv::Exception e) {
		TRACE("error: %s\n", e.what());
	}
}

cv::Mat CcalcPCADlg::ReshapeByOpenCV(cv::Mat& mat)
{
	cv::Mat ref = mat.reshape(1, mat.rows * mat.cols);
	cv::Mat tobeDone = ref.clone();
	return tobeDone;
}

cv::Mat CcalcPCADlg::ReshapeByMe(cv::Mat& mat)
{
	cv::Mat ref = mat.reshape(1, mat.rows * mat.cols);
	cv::Mat tobeDone = ref.clone();

	//outputMatrix("reshapebyopencv", tobeDone);
	int iType = tobeDone.type();

	std::vector<std::vector<float>> values;
	
	for (int c = 0; c < tobeDone.cols; c++) {
		std::vector<float> colValue;
		for (int r = 0; r < tobeDone.rows; r++) {
			float uValue = GetValue(tobeDone, r, c, 0);
			if (uValue > -0.0001 && uValue < 0.0001)
				continue;
			else
				colValue.push_back(uValue);
		}
		values.push_back(colValue);
	}

	cv::Mat result(values.at(0).size(), tobeDone.cols, tobeDone.type());

	for (int r = 0; r < values.at(0).size(); r++) {
		for (int c = 0; c < values.size(); c++) {
			result.at<float>(r, c) = values.at(c).at(r);
		}
	}

	//outputMatrix("reshapebyme", result);
	return result;
}


unsigned short CcalcPCADlg::GetValueU16(Mat& mat, int r, int c, int b)
{
	if (!mat.isContinuous())
		return 0;
	if (r < 0 || r >= mat.rows)
		return 0;
	if (c < 0 || c >= mat.cols)
		return 0;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return 0;

	unsigned short * p = (unsigned short*)mat.ptr(r);

	unsigned short f = (unsigned short)p[c * iChannel + b];
	return f;
}

void CcalcPCADlg::SetValueU16(Mat& mat, int r, int c, int b, unsigned short& fValue)
{
	if (!mat.isContinuous())
		return;
	if (r < 0 || r >= mat.rows)
		return;
	if (c < 0 || c >= mat.cols)
		return;
	int iChannel = mat.channels();
	if (b < 0 || b >= iChannel)
		return;

	unsigned short * p = (unsigned short*)mat.ptr(r);

	//float f = (float)p[c * iChannel + b];
	//return f;
	p[c * iChannel + b] = fValue;
}

void CcalcPCADlg::outputMatrix(string name, cv::Mat& mat)
{
	// redirect cout to file
	string filename = name + ".txt";
	std::ofstream out(filename);
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());

	cout << "Mat(" << name << ") = "<< endl << " "  << mat << endl << endl;

	//reset cout to console
	std::cout.rdbuf(coutbuf);
}

void CcalcPCADlg::FillBackZero(cv::Mat& nonZero, cv::Mat& tobeDone)
{
	for (int c = 0; c < tobeDone.cols; c++)
	{
		int index = 0;
		for (int r = 0; r < tobeDone.rows; r++)
		{
			float fValue = GetValue(tobeDone, r, c, 0);
			float ftobeSet = GetValue(nonZero, index, c, 0);
			if (fValue > -0.0001 && fValue < 0.0001)
			{
				continue;
			}
			else
			{
				SetValue(tobeDone, r, c, 0, ftobeSet);
				index++;
			}

		}
	}

	//outputMatrix("fillback", tobeDone);
}