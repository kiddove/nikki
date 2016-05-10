// StasticDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Segmentation.h"
#include "StatisticDlg.h"
#include "afxdialogex.h"


// CStasticDlg dialog

IMPLEMENT_DYNAMIC(CStatisticDlg, CDialogEx)

CStatisticDlg::CStatisticDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStatisticDlg::IDD, pParent)
{
	m_pFont = new CFont;
	// 字体
	m_pFont->CreateFont(
		-16,											// 高度
		0,												// 宽度
		0,												// 角度
		0,												// 方向
		FW_NORMAL,										// 粗体
		0,												// 斜体
		0,												// 下划线
		0,												// 删除线
		DEFAULT_CHARSET,								// 字符集
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("Arial")
		);
	m_pFontSmall = new CFont;
	// 字体
	m_pFontSmall->CreateFont(
		-8,											// 高度
		0,												// 宽度
		0,												// 角度
		0,												// 方向
		FW_NORMAL,										// 粗体
		0,												// 斜体
		0,												// 下划线
		0,												// 删除线
		DEFAULT_CHARSET,								// 字符集
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("Arial")
		);
	m_pImage = NULL;
}

CStatisticDlg::~CStatisticDlg()
{
	delete m_pFont;
	delete m_pFontSmall;
}

void CStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatisticDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CStatisticDlg message handlers


void CStatisticDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	if (m_pImage == NULL)
		return;
	CRect rc;
	GetClientRect(&rc);
	//dc.FillSolidRect(rc, RGB(255, 255, 255));
	DrawVariogram(&dc, rc, m_pImage->m_mean, m_pImage->m_variogram, m_pImage->m_dp, m_pImage->m_stand_deviation);
}

void CStatisticDlg::DrawVariogram(CDC* pDC, CRect& rc, std::vector<float>& vecMean, std::vector<std::vector<float>>& vecVariogram, std::vector<drawParam>& vecParam, std::vector<float>& vecStandDeviation)
{
	if (rc.IsRectEmpty())
		return;
	pDC->FillSolidRect(rc, RGB(255, 255, 255));

	///////////////////////////////////////////////////////
	if (vecVariogram.empty())
		return;
	//CPen* pFrameLine = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	//CPen* pOldPen = pDC->SelectObject(pFrameLine);
	CFont* pOldFont = pDC->SelectObject(m_pFont);
	COLORREF oldColor = pDC->SetTextColor(RGB(0, 0, 0));

	CRect rcVariogram(rc);

	LOGFONT lf;
	m_pFont->GetLogFont(&lf);

	rcVariogram.DeflateRect(abs(lf.lfHeight), abs(lf.lfHeight), abs(lf.lfHeight), abs(lf.lfHeight));

	CRect rcLabel(rc);
	rcLabel.left = rcVariogram.left;
	rcLabel.bottom = rcLabel.top + abs(lf.lfHeight) + 2;
	// draw label --- height width channel datatype?
	CString strLabel;
	strLabel.Format(_T("height:%d, width:%d, channel:%d"), m_pImage->m_data.rows, m_pImage->m_data.cols, m_pImage->m_data.channels());
	pDC->DrawText(strLabel, rcLabel, DT_SINGLELINE|DT_LEFT);
	// draw single band, calculate rc first
	int iSize = vecVariogram.size();
	std::vector<CRect> rcBand;

	int iWidth = rcVariogram.Width() / 2;
	int iHeight = rcVariogram.Height() / 2;
	CRect rc1, rc2, rc3, rc4;
	rc1 = rcVariogram;
	rc1.DeflateRect(1, 1, 1, 1);
	rc1.bottom = rc1.top + iHeight;
	rc1.right = rc1.left + iWidth;

	rc2 = rc1;
	rc2.MoveToX(rc1.right + 1);

	rc3 = rc1;
	rc3.MoveToY(rc1.bottom + 1);

	rc4 = rc3;
	rc4.MoveToX(rc3.right + 1);
	rcBand.push_back(rc1);
	//pDC->FillSolidRect(rc1, RGB(255, 0, 0));
	rcBand.push_back(rc2);
	//pDC->FillSolidRect(rc2, RGB(0, 255, 0));
	rcBand.push_back(rc3);
	//pDC->FillSolidRect(rc3, RGB(0, 0, 255));
	rcBand.push_back(rc4);
	//pDC->FillSolidRect(rc4, RGB(0, 0, 0));
	for (int i = 0; i < iSize; i++)
	{
		// calculate rc to be continued...
		DrawVariogram(pDC, rcBand[i], i, vecMean[i], vecStandDeviation[i], vecVariogram[i], vecParam[i]);
	}

	//for (int i = 0; i < (int)m_pImage->m_mean.size(); i++)
	//{
	//	CString str;
	//	if (i != m_pImage->m_mean.size() - 1)
	//		str.Format(_T("%.3f, "), m_pImage->m_mean[i]);
	//	else
	//		str.Format(_T("%.3f"), m_pImage->m_mean[i]);

	//	strLabel += str;
	//}
	

	//CRect rcX, rcY, rcLabel;
	////pDC->FillSolidRect(rcLabel, RGB(255, 0, 0));

	//rcY.left = 0;
	//rcY.top = rcLabel.bottom + 1;
	//rcY.right = rcLabel.left - 1;
	//rcY.bottom = rcVariogram.bottom - 1;
	////pDC->FillSolidRect(rcY, RGB(255, 0, 0));

	//rcX.left = rcVariogram.left + 1;
	//rcX.top = rcVariogram.bottom + 1;
	//rcX.bottom = rc.bottom - 1;
	//rcX.right = rcVariogram.right - 1;
	////pDC->FillSolidRect(rcX, RGB(255, 0, 0));

	//// draw label --- height width channel meanvalue
	//CString strLabel;
	//strLabel.Format(_T("height:%d, width:%d, channel:%d, mean value:"), m_pImage->m_data.rows, m_pImage->m_data.cols, m_pImage->m_data.channels());
	//for (int i = 0; i < (int)m_pImage->m_mean.size(); i++)
	//{
	//	CString str;
	//	if (i != m_pImage->m_mean.size() - 1)
	//		str.Format(_T("%.3f, "), m_pImage->m_mean[i]);
	//	else
	//		str.Format(_T("%.3f"), m_pImage->m_mean[i]);

	//	strLabel += str;
	//}
	//pDC->DrawText(strLabel, rcLabel, DT_SINGLELINE|DT_LEFT);
	//// draw Y --- 5
	//float f = rcY.Height() / (float)5.0;
	//CRect rcTemp = rcY;
	//rcTemp.bottom = (int)(rcTemp.top + f);
	//for (int i = 0; i < 5; i++)
	//{
	//	CString str;
	//	str.Format(_T("%.3f"), dp.fMax *(1 -  i / 5.0));
	//	pDC->DrawText(str, rcTemp, DT_SINGLELINE|DT_RIGHT);
	//	rcTemp.top += f;
	//	rcTemp.bottom += f;
	//}
	//// draw X --- 5
	//f = rcX.Width() / (float)5.0;
	//rcTemp = rcX;
	//for (int i = 0; i <= 5; i++)
	//{
	//	CString str;
	//	str.Format(_T("%d"), (int)(dp.iMax * i / 5.0));
	//	pDC->DrawText(str, rcTemp, DT_SINGLELINE|DT_LEFT);
	//	rcTemp.left += f;
	//	rcTemp.right += f;
	//}
	//// draw Variogram
	//pDC->MoveTo(rcVariogram.left, rcVariogram.top);
	//
	//pDC->LineTo(rcVariogram.left, rcVariogram.bottom);

	//pDC->LineTo(rcVariogram.right, rcVariogram.bottom);

	//int iHeight = rcVariogram.Height() - 2;
	//int iWidth = rcVariogram.Width() - 2;
	//std::map<int, float>::iterator iter = mapVariogram.begin();
	//int iCount = (int)mapVariogram.size();

	//CPoint ptStart;
	//ptStart.x = rcVariogram.left + (int)(iter->first * (float)iWidth / (float)iCount);
	//ptStart.y = rcVariogram.bottom - (int)(iter->second * (float)iHeight / dp.fMax);

	//pDC->MoveTo(ptStart);
	//while (iter != mapVariogram.end())
	//{
	//	CPoint pt;
	//	pt.x = rcVariogram.left + (int)(iter->first * (float)iWidth / (float)iCount);
	//	pt.y = rcVariogram.bottom - (int)(iter->second * (float)iHeight / dp.fMax);
	//	pDC->LineTo(pt);
	//	iter++;
	//}


	pDC->SelectObject(pOldFont);
	//pDC->SelectObject(pOldPen);
	pDC->SetTextColor(oldColor);
	//delete pFrameLine;
}

void CStatisticDlg::DrawVariogram(CDC* pDC, CRect& rc, int band, float& fMean, float& fStandDeviation, std::vector<float>& bandVariogram, drawParam& dp)
{
	if (bandVariogram.empty())
		return;
	// init
	CPen* pFrameLine = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(pFrameLine);
	CFont* pOldFont = pDC->SelectObject(m_pFont);
	COLORREF oldColor = pDC->SetTextColor(RGB(0, 0, 0));
	//COLORREF oldBkg = pDC->SetBkColor(RGB(255, 255, 255));

	CRect rcDraw(rc);
	rcDraw.DeflateRect(1, 1, 1, 1);
	CRect rcTemp1, rcTemp2;
	pDC->DrawText(_T("(lag in pixels)"), &rcTemp1, DT_CALCRECT);
	pDC->DrawText(_T("(semi-variance 1/2)"), &rcTemp2, DT_CALCRECT);
	rcDraw.DeflateRect(rcTemp2.Width() + 2, 0, rcTemp1.Width() + 2, 0);

	LOGFONT lf;
	m_pFont->GetLogFont(&lf);
	CRect rcLabel, rcX, rcY, rcChart;
	// label
	rcLabel = rcDraw;
	rcLabel.bottom = rcLabel.top + abs(lf.lfHeight) + 2;
	CRect rcTemp;
	pDC->DrawText(_T("00.000"), &rcTemp, DT_CALCRECT);
	// x
	rcX = rcDraw;
	rcX.top = rcDraw.bottom - abs(lf.lfHeight) - 2;
	// y
	rcY = rcDraw;
	rcY.top= rcLabel.bottom + 1;
	rcY.right = rcY.left + rcTemp.Width();;
	rcY.bottom = rcX.top - 1;

	rcX.left = rcY.right + 1;
	rcLabel.left = rcY.right + 1;
	// chart
	rcChart = rcDraw;
	rcChart.left = rcY.right + 1;
	rcChart.top = rcLabel.bottom + 1;
	rcChart.bottom = rcX. top - 1;

	//pDC->FillSolidRect(rcDraw, RGB(255, 0, 0));
	//pDC->FillSolidRect(rcLabel, RGB(255, 0, 0));
	//pDC->FillSolidRect(rcX, RGB(0, 255, 0));
	//pDC->FillSolidRect(rcY, RGB(0, 0, 255));
	//pDC->FillSolidRect(rcChart, RGB(0, 0, 0));
	//return;

	// draw
	// label
	CString strLabel;
	strLabel.Format(_T("band%d --- mean: %.2f, SD: %.2f"), band+1, fMean, fStandDeviation);
	pDC->DrawText(strLabel, rcLabel, DT_SINGLELINE|DT_CENTER);

	int y = 0;
	CRect rcYAxis = rcLabel;
	CRect rcTemp3;

	rcYAxis.right = rcY.right - 1;
	rcYAxis.left = rcYAxis.right - rcTemp2.Width() - 1;
	pDC->DrawText(_T("(semi-variance "), &rcTemp3, DT_CALCRECT);
	pDC->DrawText(_T("(semi-variance "), rcYAxis, DT_SINGLELINE|DT_LEFT);
	rcYAxis.left += rcTemp3.Width();

	// change font;
	pDC->SelectObject(m_pFontSmall);
	pDC->DrawText(_T("1/2"), &rcTemp3, DT_CALCRECT);
	pDC->DrawText(_T("1/2"), rcYAxis, DT_SINGLELINE|DT_LEFT);
	rcYAxis.left += rcTemp3.Width();

	// change font
	pDC->SelectObject(m_pFont);
	pDC->DrawText(_T(")"), rcYAxis, DT_SINGLELINE|DT_LEFT);


	// y
	float f = rcY.Height() / (float)5.0;
	rcTemp = rcY;
	rcTemp.bottom = (int)(rcTemp.top + f);
	for (int i = 0; i < 5; i++)
	{
		CString str;
		str.Format(_T("%.2f"), dp.fMax *(1 -  i / 5.0));
		pDC->DrawText(str, rcTemp, DT_SINGLELINE|DT_RIGHT);
		rcTemp.top += f;
		rcTemp.bottom += f;
	}

	// x
	f = rcX.Width() / (float)5.0;
	rcTemp = rcX;
	for (int i = 0; i <= 5; i++)
	{
		CString str;
		str.Format(_T("%d"), (int)(dp.iMax * i / 5.0));
		CRect rcMid;
		pDC->DrawText(str, rcMid, DT_SINGLELINE|DT_CALCRECT);
		if (i == 0)
			pDC->DrawText(str, rcX, DT_SINGLELINE|DT_LEFT);
		else if (i == 5)
			pDC->DrawText(str, rcX, DT_SINGLELINE|DT_RIGHT);
		else
		{
			CRect rcx = rcTemp;
			rcx.MoveToX(rcTemp.left - rcMid.Width() / 2);
			pDC->DrawText(str, rcx, DT_SINGLELINE|DT_LEFT);
		}
		rcTemp.left += f;
		rcTemp.right = rcTemp.left + f + 1;
	}
	CRect rcXAxis = rcX;
	rcXAxis.MoveToX(rcX.right + 10);
	rcXAxis.right = rcXAxis.right + rcTemp1.Width();
	pDC->DrawText(_T("(lag in pixels)"), rcXAxis, DT_SINGLELINE|DT_LEFT);

	// chart
	pDC->MoveTo(rcChart.left, rcChart.top);

	pDC->LineTo(rcChart.left, rcChart.bottom);

	pDC->LineTo(rcChart.right, rcChart.bottom);

	int iHeight = rcChart.Height() - 2;
	int iWidth = rcChart.Width() - 2;
	int iCount = (int)bandVariogram.size();

	CPoint ptStart;
	ptStart.x = rcChart.left + (int)((float)iWidth / (float)iCount);
	ptStart.y = rcChart.bottom - (int)(bandVariogram[0] * (float)iHeight / dp.fMax);

	pDC->MoveTo(ptStart);
	for (int i = 0; i < iCount; i++)
	{
		CPoint pt;
		pt.x = rcChart.left + (int)((i + 1) * (float)iWidth / (float)iCount);
		pt.y = rcChart.bottom - (int)(bandVariogram[i] * (float)iHeight / dp.fMax);
		pDC->LineTo(pt);
	}

	// uninit
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(oldColor);
	//pDC->SetBkColor(oldBkg);
	delete pFrameLine;
}


void CStatisticDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	delete this;
}

void CStatisticDlg::DrawAxis(CDC* pDC, CRect& rc, char* pLabel)
{
	pDC->FillSolidRect(rc, RGB(255, 0, 0));
}