// FileParserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileParser.h"
#include "FileParserDlg.h"
#include "IDXFile.h"
#include "NDFFile.h"
#include "LasFile.h"
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


// CFileParserDlg 对话框




CFileParserDlg::CFileParserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileParserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_FILE, m_Progress);
}

BEGIN_MESSAGE_MAP(CFileParserDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_BUTTON_OPEN_LAS, &CFileParserDlg::OnBnClickedButtonOpenLas)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_NDF, &CFileParserDlg::OnBnClickedButtonOpenNdf)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TIME, &CFileParserDlg::OnBnClickedButtonAddTime)
//	ON_BN_CLICKED(IDC_BUTTON_CSD, &CFileParserDlg::OnBnClickedButtonCsd)
	ON_BN_CLICKED(IDC_BUTTON_TXT, &CFileParserDlg::OnBnClickedButtonTxt)
END_MESSAGE_MAP()


// CFileParserDlg 消息处理程序

BOOL CFileParserDlg::OnInitDialog()
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

	GetDlgItem(IDC_BUTTON_OPEN_LAS)->ShowWindow(SW_HIDE);
	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_BUTTON_ADD_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN_NDF)->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileParserDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileParserDlg::OnPaint()
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
HCURSOR CFileParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//void CFileParserDlg::OnBnClickedButtonOpenLas()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString strPath;
//	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Las Files(*.las)|*.las|All Files (*.*)|*.*||"));
//	if(dlg->DoModal()!=IDOK)
//	{
//		delete dlg;
//		dlg = NULL;
//		return;
//	}
//	else
//		strPath = dlg->GetPathName();
//	m_strLasPath = strPath;
//	delete dlg;
//	dlg = NULL;
//	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
//	UpdateData(FALSE);
//}

void CFileParserDlg::OnBnClickedButtonOpenNdf()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Ndf Files(*.ndf)|*.ndf|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		strPath = dlg->GetPathName();
	m_strNdfPath = strPath;
	m_strIdxPath = strPath.Left(strPath.GetLength()-4) + _T(".idx");
	delete dlg;
	dlg = NULL;
	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	GetDlgItem(IDC_BUTTON_OPEN_NDF)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD_TIME)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CFileParserDlg::OnBnClickedButtonAddTime()
{
	/*OnBnClickedButtonOpenLas();
	OnBnClickedButtonOpenNdf();*/
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_ADD_TIME)->EnableWindow(FALSE);
	std::vector<double> v_ndftime;
	std::vector<int> v_ndfseq;
//	std::set<double> las_time_set;
	std::vector<int> v_ndfframenum;
	std::vector<int> v_ndflinenum;
	//std::map<int,int> frameoffestMap;
	std::vector<int> v_ndflinepos;
	std::vector<double> v_gpstime;
	double dmin=0.0,dmax=0.0;
	ReadTimeFile(v_gpstime,dmin,dmax);
	dmax -= 0.00001;
	dmin -= 0.00002;
	//CString strTimePath;
	//strTimePath = m_strIdxPath.Left(m_strIdxPath.GetLength()-4) + _T(".time");
	CString strTxtPath;
	strTxtPath = m_strNdfPath.Left(m_strIdxPath.GetLength()-4) + _T(".txt");
	CFile idxfile;
	if (!idxfile.Open(m_strIdxPath,CFile::modeRead|CFile::shareDenyNone))
		return;
	//CFile timefile;
	//timefile.Open(strTimePath,CFile::modeCreate|CFile::modeWrite);
	//CFile csdfile;
	//if (!csdfile.Open(m_strCsdPath,CFile::modeRead))
	//{
	//	idxfile.Close();
	//	return;
	//}
	//CStdioFile timefile;
	//if (!timefile.Open(m_strTimePath,CFile::modeRead|CFile::shareDenyNone))
	//{
	//	idxfile.Close();
	//	//csdfile.Close();
	//	return;
	//}
	CFile ndffile;
	if (!ndffile.Open(m_strNdfPath,CFile::modeRead|CFile::shareDenyNone))
	{
		idxfile.Close();
		//csdfile.Close();
//		timefile.Close();
		return;
	}
	CStdioFile outfile;
	if (!outfile.Open(strTxtPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		idxfile.Close();
		//csdfile.Close();
		ndffile.Close();
//		timefile.Close();
		return;
	}
	IDXHeader idxheader;
	IDXRecord idxrecord;
//	CSDHeader csdheader;
//	CSDData csddata;
//	Public_Header_Block lasheader;
//	Variable_Length_Record lasvariablerecord;
//	Point_Data_Record lasdata;

	// read idx & ndf
	GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("Read Idx and Ndf File"));
	UpdateData(FALSE);
	memset(&idxheader,0,sizeof(IDXHeader));
	idxfile.Read(&idxheader,sizeof(IDXHeader));
	//v_ndftime.reserve(idxheader.iNumOfFrame*idxheader.iNumOfLinesPerFrame);
	//v_ndfseq.reserve(idxheader.iNumOfFrame*idxheader.iNumOfLinesPerFrame);
	//v_ndfseq.assign(idxheader.iNumOfFrame*idxheader.iNumOfLinesPerFrame,-1);
	//idxheader.iNumOfLinesPerFrame
	memset(&idxrecord,0,sizeof(IDXRecord));
	idxfile.Read(&idxrecord,sizeof(IDXRecord));
	m_Progress.SetRange32(0,idxheader.iNumOfFrame*idxheader.iNumOfLinesPerFrame-1);
	m_Progress.SetStep(1);
	//v_ndflinepos[0]=0;
	int linepos=0;
	for (int i = 1; i<=idxheader.iNumOfFrame; i++)
	{
		memset(&idxrecord,0,sizeof(IDXRecord));
		idxfile.Read(&idxrecord,sizeof(IDXRecord));
		double dInterval = (idxrecord.dEndTime - idxrecord.dStartTime) / idxheader.iNumOfLinesPerFrame;
		double dtime = idxrecord.dStartTime;
		ndffile.Seek(idxrecord.pFrameStart,CFile::begin);
		for (int j=1;j<=idxheader.iNumOfLinesPerFrame;j++)
		{
			// v_ndflinepos.push_back(linepos);
			dtime = idxrecord.dStartTime + (j-1)*dInterval;
			USHORT nLineNum=0,nOffset2NextLine=0;
			ndffile.Read(&nLineNum,sizeof(USHORT));
			ndffile.Read(&nOffset2NextLine,sizeof(USHORT));
			ndffile.Seek(nOffset2NextLine,CFile::current);
			if (dtime <= dmax && dtime >= dmin)
			{
				v_ndflinepos.push_back(linepos);
				v_ndftime.push_back(dtime);
				v_ndfframenum.push_back(i);
				v_ndflinenum.push_back(j);
			}
			m_Progress.StepIt();
			linepos+=nOffset2NextLine+2*sizeof(USHORT);
			UpdateData(FALSE);
		}
	}
	ASSERT(linepos == ndffile.GetLength());
	memset(&idxrecord,0,sizeof(IDXRecord));
	idxfile.Read(&idxrecord,sizeof(IDXRecord));

	//GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("Read Las File"));
	//memset(&lasheader,0,sizeof(Public_Header_Block));
	//lasfile.Read(&lasheader,sizeof(Public_Header_Block));
	//lasfile.Seek(lasheader.uOffset_to_data,CFile::begin);
//	m_Progress.SetRange32(0,lasheader.uNum_of_point_records-1);
	//m_Progress.SetPos(0);
	//m_Progress.SetStep(1);
	//UpdateData(FALSE);
	//for (UINT i=0;i<lasheader.uNum_of_point_records;i++)
	//{
	//	memset(&lasdata,0,sizeof(Point_Data_Record));
	//	lasfile.Read(&lasdata,sizeof(Point_Data_Record));
	//	las_time_set.insert(lasdata.dGPS_Time);
	//	m_Progress.StepIt();
	//	UpdateData(FALSE);
	//}

	//lasfile.Close();
	//idxfile.Close();
	//ndffile.Close();
	//csdfile.Close();
	//outfile.Close();
	//return;


	CString strContent;
	/*strContent.Format("")*/
	//outfile.WriteString(_T(""))

	GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("Search and Read Ndf File"));
	UpdateData(FALSE);
	std::vector<double>::iterator iter = v_ndftime.begin();
	m_Progress.SetRange32(0,(int)v_gpstime.size()-1);
	m_Progress.SetPos(0);
	m_Progress.SetStep(1);
	UpdateData(FALSE);
	BYTE WaveData[40];
	//double dndfmin = v_ndftime[0];
	//double dndfmax = v_ndftime[v_ndftime.size()-1];
	UINT nSize = (UINT)v_gpstime.size();
	//std::vector<double>::iterator iterbegin = v_gpstime.begin();
	//std::vector<double>::iterator iterlast = v_gpstime.end(); 
	//iterlast--;
	//for (std::vector<double>::iterator iterset = v_gpstime.begin();iterset!=v_gpstime.end();iterset++)
	for (UINT i= 0; i<nSize;i++)
	{
		// do find
		int ipos=0;
		double dmintime = (v_gpstime[i]) - 0.00002;
		double dmaxtime = (v_gpstime[i]) - 0.00001;
		//if (v_gpstime[0]>dndfmax)
		//	continue;
		//if (v_gpstime[nSize -1]<dndfmin)
		//	continue;
		if (SearchInVec(v_ndftime,dmintime,dmaxtime,ipos) >= 0)
		{
			//int ndftimepos = (int)distance(v_ndftime.begin(),iter);
			//if (v_ndfseq[ndftimepos] == -1)
			//	continue;
			strContent.Format(_T("Frame:%d\r\nLine:%d\r\n"),v_ndfframenum[ipos],v_ndflinenum[ipos]);
			outfile.WriteString(strContent);
			ndffile.Seek(v_ndflinepos[ipos],CFile::begin);
			USHORT nLineNum = 0,nOffset2NextLine=0,nNumOfBytes=0,nTimeInerval=0;
			ndffile.Read(&nLineNum,sizeof(USHORT));
			//strContent.Format(_T("Line %d:\r\n"),k+1);
			//outfile.WriteString(strContent);
			ndffile.Read(&nOffset2NextLine,sizeof(USHORT));
			strContent.Format(_T("Offset to next line:%d\r\n"),nOffset2NextLine);
			outfile.WriteString(strContent);
			ndffile.Read(WaveData,40*sizeof(BYTE));
			for (int iw=0;iw<40;iw++)
			{
				strContent.Format(_T("Wavaform Data[%d]:%d\r\n"),iw+1,WaveData[iw]);
				outfile.WriteString(strContent);
			}
			USHORT nRest = nOffset2NextLine+4-sizeof(USHORT)*2-40*sizeof(BYTE);
			// read segment
			int pc=0;
			while (nRest>0)
			{
				//strContent.Format(_T("Number Of Bytes in %d:"),++pc);
				ndffile.Read(&nNumOfBytes,sizeof(BYTE));
				strContent.Format(_T("Number Of Bytes in %d Waveform Segment:%d\r\n"),++pc,nNumOfBytes);
				outfile.WriteString(strContent);
				LPBYTE pBuffer = new BYTE [nNumOfBytes];
				memset(pBuffer,0,nNumOfBytes*sizeof(BYTE));

				ndffile.Read(pBuffer,nNumOfBytes*sizeof(BYTE));
				for (int iw=0;iw<nNumOfBytes;iw++)
				{
					strContent.Format(_T("Waveform Segment Data[%d]:%d\r\n"),iw+1,*(pBuffer+iw));
					outfile.WriteString(strContent);
				}
				delete[] pBuffer;
				ndffile.Read(&nTimeInerval,sizeof(USHORT));
				strContent.Format(_T("TimeInterval:%d\r\n"),nTimeInerval);
				outfile.WriteString(strContent);
				nRest -= nNumOfBytes+sizeof(USHORT)+sizeof(BYTE);
			}	
			//iter = tempiter;
		}
		m_Progress.StepIt();
		UpdateData(FALSE);
	}
//	lasfile.Close();
	idxfile.Close();
	ndffile.Close();
	//csdfile.Close();
//	timefile.Close();
	outfile.Close();

	GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("Finish"));
	m_Progress.SetPos(0);
	GetDlgItem(IDC_BUTTON_TXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OPEN_NDF)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD_TIME)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

//void CFileParserDlg::OnBnClickedButtonCsd()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString strPath;
//	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Ndf Files(*.csd)|*.csd|All Files (*.*)|*.*||"));
//	if(dlg->DoModal()!=IDOK)
//	{
//		delete dlg;
//		dlg = NULL;
//		return;
//	}
//	else
//		strPath = dlg->GetPathName();
//	m_strCsdPath = strPath;
//	delete dlg;
//	dlg = NULL;
//	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
//	UpdateData(FALSE);
//}

int CFileParserDlg::SearchInVec(std::vector<double>& timevec,double dmintime,double dmaxtime,int& iStartPos)
{
	//int ipos=iStartPos;
	int iSize = (int)timevec.size();
	//if (dmintime > timevec[iSize-1] || dmaxtime < timevec[iStartPos])
	//	return -1;
	//while (iStartPos < iSize)
	//{
	//	if (timevec[iStartPos] >= dmintime)
	//		//&& timevec[iStartPos] <= dmaxtime)
	//	{
	//		if (timevec[iStartPos] <= dmaxtime)
	//			return iStartPos;
	//	}
	//	iStartPos++;
	//}
	//iStartPos = ipos;
	for (int i=0;i<iSize-1;i++)
	{
		if (timevec[i] >= dmintime)
			if (timevec[i] <= dmaxtime)
			{
				iStartPos = i;
				return i;
			}
			
	}
	return -1;
}
void CFileParserDlg::OnBnClickedButtonTxt()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Ndf Files(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		strPath = dlg->GetPathName();
	m_strTimePath = strPath;
	//m_strIdxPath = strPath.Left(strPath.GetLength()-4) + _T(".idx");
	delete dlg;
	dlg = NULL;
	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	GetDlgItem(IDC_BUTTON_TXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN_NDF)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CFileParserDlg::ReadTimeFile(std::vector<double>& v_gpstime,double& dMin,double& dMax)
{
	v_gpstime.clear();
	std::set<double> s_time;
	CStdioFile timefile;
	if (!timefile.Open(m_strTimePath,CFile::modeRead|CFile::shareDenyNone))
	{
		GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("Failed to Read Time File"));
		return;
	}
	CString strContent;
	double dGPSTime = 0.0;
	ULONGLONG nLen = timefile.GetLength();
	while (1)
	{
		ULONGLONG nPos = timefile.GetPosition();
		if (nPos >= nLen)
			break;
		timefile.ReadString(strContent);
		if (!strContent.IsEmpty())
		{
			dGPSTime = atof(strContent);
			v_gpstime.push_back(dGPSTime);
			s_time.insert(dGPSTime);
		}
	}
	std::set<double>::iterator iter = s_time.begin();
	if (iter!=s_time.end())
	{
		dMin = *iter;
		iter = s_time.end();
		iter--;
		dMax = *(iter);//+(s_time.size()-1)); 
	}
	/*dMin = s_time[0];
	dMin = s_time[s_time.size()-1];*/
	timefile.Close();
}