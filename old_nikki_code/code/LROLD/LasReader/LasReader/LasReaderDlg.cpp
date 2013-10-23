// LasReaderDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LasReader.h"
#include "LasReaderDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CLasReaderDlg �Ի���




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


// CLasReaderDlg ��Ϣ�������

BOOL CLasReaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLasReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CLasReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLasReaderDlg::OnBnClickedButtonOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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