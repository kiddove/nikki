// Work1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Work1.h"
#include "Work1Dlg.h"

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


// CWork1Dlg 对话框




CWork1Dlg::CWork1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWork1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWork1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWork1Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_Y, &CWork1Dlg::OnBnClickedButtonY)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CWork1Dlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// CWork1Dlg 消息处理程序

BOOL CWork1Dlg::OnInitDialog()
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

void CWork1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWork1Dlg::OnPaint()
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
HCURSOR CWork1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWork1Dlg::OnBnClickedButtonY()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("txt Files(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		strPath = dlg->GetPathName();
	ReadFromTxt(strPath,VecY);
	delete dlg;
	dlg = NULL;
	UpdateData(FALSE);
}

void CWork1Dlg::ReadFromTxt(CString& strPath,vector<double>& vec)
{
	CStdioFile stdfile;
	if (!stdfile.Open(strPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		CString strErr;
		strErr.Format(_T("打开文件%s失败"));
		ReportError(strErr);
		return;
	}
	double dValue = 0.0;
	CString strValue;
	//stdfile.ReadString(strValue);
	ULONGLONG nLen = stdfile.GetLength();
	ULONGLONG nPos = 0;
	while (nLen>nPos)
	{
		stdfile.ReadString(strValue);
		nPos = stdfile.GetPosition();
		dValue = atof(strValue);
		vec.push_back(dValue);
	}

	stdfile.Close();
}

void CWork1Dlg::InvMatrix(vector<vector<double>>& matrix)
{
	int iSize1 = (int)matrix.size();
	if (0 == iSize1)
		return;
	int iSize2 = (int)matrix[0].size();
	if (0 == iSize2)
		return;
	int i,j,k;
	for (k=0;k<iSize1;k++)
	{
		for (j=0;j<iSize2;j++)
			if(j!=k)
				matrix[k][j]=matrix[k][j]/matrix[k][k];
		matrix[k][k]=1.0/matrix[k][k];
		for(i=0;i<iSize1;i++)
			if(i!=k)
			{
				for(j=0;j<iSize2;j++)
					if (j!=k) 
						matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
				matrix[i][k]=-matrix[i][k]*matrix[k][k];
			}/*if i<>k*/
	}/*for k*/
}


double CWork1Dlg::Sum(vector<double>& vec)
{
	double dValue = 0.0;
	int iSize = (int)vec.size();
	for (int i=0;i<iSize;i++)
		dValue += vec[i];
	return dValue;
}

double CWork1Dlg::Sum2(vector<double>& vec)
{
	double dValue = 0.0;
	int iSize = (int)vec.size();
	for (int i=0;i<iSize;i++)
		dValue += (vec[i] * vec[i]);
	return dValue;
}

double CWork1Dlg::Sum3(vector<double>& vec)
{
	double dValue = 0.0;
	int iSize = (int)vec.size();
	for (int i=0;i<iSize;i++)
		dValue += (vec[i] * vec[i] * vec[i]);
	return dValue;
}

double CWork1Dlg::Sum4(vector<double>& vec)
{
	double dValue = 0.0;
	int iSize = (int)vec.size();
	for (int i=0;i<iSize;i++)
		dValue += (vec[i] * vec[i] * vec[i] * vec[i]);
	return dValue;
}

void CWork1Dlg::MultiMatrix(vector<vector<double>>& VecResult,vector<vector<double>>& VecA,vector<vector<double>>& VecB)
{
	int i,j,k;
	for(i=0;i<(int)VecResult.size();i++)
		for(j=0;j<(int)VecResult[0].size();j++)
		{
			VecResult[i][j]=0;
			for(k=0;k<(int)VecA.size();k++)
				VecResult[i][j]+=VecA[i][k]*VecB[k][j];
		}
}

void CWork1Dlg::Run()
{
	UpdateData();
	CString strK;
	GetDlgItem(IDC_EDIT_K)->GetWindowText(strK);
	m_iK = atoi(strK);
	CStdioFile ss;
	GetModuleFileName(NULL,m_strPath.GetBuffer(1024),1024);
	m_strPath.ReleaseBuffer(1024);
	int ipos = m_strPath.Find('.');
	m_strPath = m_strPath.Left(ipos);
	m_strPath += _T(".txt");
	if (!ss.Open(m_strPath,CFile::modeCreate|CFile::shareDenyWrite|CFile::modeWrite))
	{
		CString strErr;
		strErr.Format(_T("Failed to create file %s"),m_strPath);
		ReportError(strErr);
		return;
	}
	if (m_iK < 0)
	{
		AfxMessageBox(_T("Invalid K"));
		ss.Close();
		return;
	}
	for (int i=1;i<=m_iK;i++)
		VecT.push_back((i-(m_iK+1)/2)*0.01);

	//m_Matrix.assign(vector<double>)
	//m_Matrix.push_back(m_Matrix[0].push_back((double)m_iK));
	m_Matrix.assign(3,vector<double>(3,0.0));
	C1.assign(3,vector<double>(1,0.0));
	C2.assign(3,vector<double>(1,0.0));
	C3.assign(3,vector<double>(1,0.0));

	Lambda.assign(3,vector<double>(1,0.0));
	Mju.assign(3,vector<double>(1,0.0));
	Eta.assign(3,vector<double>(1,0.0));

	vecx0.assign(590,vector<double>(1,0.0));
	vecx1.assign(590,vector<double>(1,0.0));
	vecx2.assign(590,vector<double>(1,0.0));

	veca.assign(m_iK,vector<double>(1,0.0));
	vecb.assign(m_iK,vector<double>(1,0.0));
	vecc.assign(m_iK,vector<double>(1,0.0));

	vecs.assign(590,vector<double>(1,0.0));

	m_Matrix[0][0] = (double)m_iK;
	m_Matrix[0][1] = Sum(VecT);
	m_Matrix[0][2] = 0.5 * Sum2(VecT);

	m_Matrix[1][1] = Sum2(VecT);
	m_Matrix[1][2] = 0.5 * Sum3(VecT);
	m_Matrix[2][2] = 0.25 * Sum4(VecT);

	m_Matrix[1][0] = m_Matrix[0][1];
	m_Matrix[2][0] = m_Matrix[0][2];
	m_Matrix[2][1] = m_Matrix[1][2];

	//ss.WriteString(_T("Initial M:\r\n"));
	//for (int i=0;i<3;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\t%f\t%f\r\n"),m_Matrix[i][0],m_Matrix[i][1],m_Matrix[i][2]);
	//	ss.WriteString(strMatrix);
	//}

	C1[0][0] = 1.0;
	C1[1][0] = 0.0;
	C1[2][0] = 0.0;

	C2[0][0] = 0.0;
	C2[1][0] = 1.0;
	C2[2][0] = 0.0;

	C3[0][0] = 0.0;
	C3[1][0] = 0.0;
	C3[2][0] = 1.0;

	InvMatrix(m_Matrix);

	//ss.WriteString(_T("After Inv M:\r\n"));
	//for (int i=0;i<3;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\t%f\t%f\r\n"),m_Matrix[i][0],m_Matrix[i][1],m_Matrix[i][2]);
	//	ss.WriteString(strMatrix);
	//}

	MultiMatrix(Lambda,m_Matrix,C1);
	MultiMatrix(Mju,m_Matrix,C2);
	MultiMatrix(Eta,m_Matrix,C3);

	//ss.WriteString(_T("Lambda :\r\n"));
	//for (int i=0;i<3;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),Lambda[i][0]);
	//	ss.WriteString(strMatrix);
	//}

	//ss.WriteString(_T("Mju :\r\n"));
	//for (int i=0;i<3;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),Lambda[i][0]);
	//	ss.WriteString(strMatrix);
	//}

	//ss.WriteString(_T("Eta :\r\n"));
	//for (int i=0;i<3;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),Eta[i][0]);
	//	ss.WriteString(strMatrix);
	//}

	for (int i=0;i<m_iK;i++)
	{
		veca[i][0]=(Lambda[0][0] + Lambda[1][0]*VecT[i] + Lambda[2][0]*VecT[i]*VecT[i]/2);
		vecb[i][0]=(Mju[0][0] + Mju[1][0]*VecT[i] + Mju[2][0]*VecT[i]*VecT[i]/2);
		vecc[i][0]=(Eta[0][0] + Eta[1][0]*VecT[i] + Eta[2][0]*VecT[i]*VecT[i]/2);
	}

	//ss.WriteString(_T("a :\r\n"));
	//for (int i=0;i<m_iK;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),veca[i][0]);
	//	ss.WriteString(strMatrix);
	//}

	//ss.WriteString(_T("b :\r\n"));
	//for (int i=0;i<m_iK;i++)
	//{	
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),vecb[i][0]);
	//	ss.WriteString(strMatrix);
	//}

	//ss.WriteString(_T("c :\r\n"));
	//for (int i=0;i<m_iK;i++)
	//{
	//	CString strMatrix;
	//	strMatrix.Format(_T("%f\r\n"),vecc[i][0]);
	//	ss.WriteString(strMatrix);
	//}
	
	for (int k= (m_iK+1)/2-1 ;k<=(590-(m_iK+1)/2);k++)
	{
		double x0=0.0,x1=0.0,x2=0.0;
		for (int i=0;i<m_iK;i++)
		{
			x0 += veca[i][0] * VecY[k+(i-(m_iK+1)/2)+1];
			x1 += vecb[i][0] * VecY[k+(i-(m_iK+1)/2)+1];
			x2 += vecc[i][0] * VecY[k+(i-(m_iK+1)/2)+1];
		}
		vecx0[k][0] = x0;
		vecx1[k][0] = x1;
		vecx2[k][0] = x2;
		vecs[k+1][0] = vecx0[k][0] + 0.01*vecx1[k][0] + 0.01*0.01*vecx2[k][0]*0.5;
		CString strS;
		strS.Format(_T("x0[%d] = %f\r\n"),k+1,vecx0[k][0]);
		ss.WriteString(strS);
		//ss.Close();
	}
	ss.Close();
}
void CWork1Dlg::OnBnClickedButtonRun()
{
	// TODO: 在此添加控件通知处理程序代码
	Run();
}
