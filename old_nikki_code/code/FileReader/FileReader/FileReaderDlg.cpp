// FileReaderDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileReader.h"
#include "FileReaderDlg.h"
#include <afxdlgs.h>
#include "CSDFile.h"
#include "NDFFile.h"

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


// CFileReaderDlg �Ի���




CFileReaderDlg::CFileReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileReaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileReaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CSD, &CFileReaderDlg::OnBnClickedButtonCsd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_CSD, &CFileReaderDlg::OnBnClickedButtonBrowserCsd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_NDF, &CFileReaderDlg::OnBnClickedButtonBrowserNdf)
	ON_BN_CLICKED(IDC_BUTTON_NDF, &CFileReaderDlg::OnBnClickedButtonNdf)
END_MESSAGE_MAP()


// CFileReaderDlg ��Ϣ�������

BOOL CFileReaderDlg::OnInitDialog()
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

void CFileReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileReaderDlg::OnPaint()
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
HCURSOR CFileReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileReaderDlg::OnBnClickedButtonCsd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	CString strPath;
	GetDlgItem(IDC_EDIT_CSD)->GetWindowText(strPath);
	TRACE("%s\r\n",strPath);
	CCSDFile csdfile(strPath);
	csdfile.ParseFile();
}

void CFileReaderDlg::OnBnClickedButtonBrowserCsd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE);
	if(dlg->DoModal()!=IDOK)
		return;
	else
		strPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	GetDlgItem(IDC_EDIT_CSD)->SetWindowText(strPath);
	UpdateData(FALSE);
}

void CFileReaderDlg::OnBnClickedButtonBrowserNdf()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPath;
	CFileDialog *dlg = new CFileDialog(TRUE);
	if(dlg->DoModal()!=IDOK)
		return;
	else
		strPath = dlg->GetPathName();
	delete dlg;
	dlg = NULL;
	GetDlgItem(IDC_EDIT_NDF)->SetWindowText(strPath);
	UpdateData(FALSE);
}

void CFileReaderDlg::OnBnClickedButtonNdf()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	CString strPath;
	GetDlgItem(IDC_EDIT_NDF)->GetWindowText(strPath);
	TRACE("%s\r\n",strPath);
	CNDFFile ndffile(strPath);
	ndffile.ParseFile();
}
