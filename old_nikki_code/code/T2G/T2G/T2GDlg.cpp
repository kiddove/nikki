// T2GDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "T2G.h"
#include "T2GDlg.h"

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


// CT2GDlg �Ի���




CT2GDlg::CT2GDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CT2GDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CT2GDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CT2GDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN_T, &CT2GDlg::OnBnClickedButtonOpenT)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_G, &CT2GDlg::OnBnClickedButtonOpenG)
	ON_BN_CLICKED(IDC_BUTTON_START, &CT2GDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CT2GDlg ��Ϣ�������

BOOL CT2GDlg::OnInitDialog()
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

void CT2GDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CT2GDlg::OnPaint()
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
HCURSOR CT2GDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CT2GDlg::OnBnClickedButtonOpenT()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Txt Files(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		m_strT = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	UpdateData(FALSE);
	// Read T
	CStdioFile tfile;
	tfile.Open(m_strT,CFile::modeRead|CFile::shareDenyWrite);
	while (tfile.GetPosition()!=tfile.GetLength())
	{
		int i=0;
		CString strContent = _T("");
		tfile.ReadString(strContent);
		strContent.Trim();
		if (_T('0') < strContent.GetAt(0) && _T('9') > strContent.GetAt(0))
		{
			int ilen = strContent.GetLength();
			while (ilen>0)
			{
				double dvalue = 0.0;
				if (i%3 == 0)
				{
					dvalue = atof(strContent);
					Tx.push_back(dvalue);
				}
				else if (i%3 == 1)
				{
					dvalue = atof(strContent);
					Ty.push_back(dvalue);
				}
				else if (i%3 == 2)
				{
					dvalue = atof(strContent);
					Tz.push_back(dvalue);
				}
				int ipos = strContent.Find(_T(' '));
				if (ipos > 0)
				{
					strContent = strContent.Right(ilen - ipos);
					strContent.Trim();
					ilen = strContent.GetLength();
				}
				else
					ilen = 0;
				i++;
			}
		}
	}
	tfile.Close();
}

void CT2GDlg::OnBnClickedButtonOpenG()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog *dlg = new CFileDialog(TRUE,NULL,NULL,4|2,_T("Txt Files(*.txt)|*.txt|All Files (*.*)|*.*||"));
	if(dlg->DoModal()!=IDOK)
	{
		delete dlg;
		dlg = NULL;
		return;
	}
	else
		m_strG = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	//GetDlgItem(IDC_EDIT_LAS_PATH)->SetWindowText(strPath);
	UpdateData(FALSE);
	// Read G
	CStdioFile tfile;
	tfile.Open(m_strG,CFile::modeRead|CFile::shareDenyWrite);
	while (tfile.GetPosition()!=tfile.GetLength())
	{
		int i=0;
		CString strContent = _T("");
		tfile.ReadString(strContent);
		strContent.Trim();
		if (_T('0') < strContent.GetAt(0) && _T('9') > strContent.GetAt(0))
		{
			int ilen = strContent.GetLength();
			while (ilen>0)
			{
				double dvalue = 0.0;
				if (i%3 == 0)
				{
					dvalue = atof(strContent);
					Gx.push_back(dvalue);
				}
				else if (i%3 == 1)
				{
					dvalue = atof(strContent);
					Gy.push_back(dvalue);
				}
				else if (i%3 == 2)
				{
					dvalue = atof(strContent);
					Gz.push_back(dvalue);
				}
				int ipos = strContent.Find(_T(' '));
				if (ipos > 0)
				{
					strContent = strContent.Right(ilen - ipos);
					strContent.Trim();
					ilen = strContent.GetLength();
				}
				else
					ilen = 0;
				i++;
			}
		}
	}
	tfile.Close();
}

void CT2GDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwLen=0;
	dwLen = GetModuleFileName(NULL,m_strOut.GetBuffer(MAX_PATH + 1),MAX_PATH);
	m_strOut.ReleaseBuffer(dwLen);
	int ipos = m_strOut.ReverseFind(_T('\\'));
	m_strOut = m_strOut.Left(ipos+1) + _T("Output.txt");

	CStdioFile outfile;
	outfile.Open(m_strOut,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite);
	UINT nSize = Tx.size();
	for (UINT i = 0; i<nSize;i++)
	{
		double dvalue = Tz[i] - Gz[i];
		CString strContent = _T("");
		if ( dvalue < 1000.0 && dvalue > 0.0)
		{
			if (dvalue < 5.0)
			{
				Rx.push_back(Tx[i]);
				Ry.push_back(Ty[i]);
				Rz.push_back(Tz[i]);
				Rd.push_back(dvalue);
				strContent.Format(_T("%.6f\t%.6f\t%.6f\t%.6f\r\n"),Tx[i],Ty[i],Tz[i],dvalue);
				outfile.WriteString(strContent);
			}
		}
	}
	outfile.Close();
}
