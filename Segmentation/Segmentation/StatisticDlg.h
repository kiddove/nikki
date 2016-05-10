#pragma once
#include "ImageReader.h"

// CStatisticDlg dialog

class CStatisticDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStatisticDlg)

public:
	CStatisticDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatisticDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STATISTIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	void DrawVariogram(CDC* pDC, CRect& rc, std::vector<float>& vecMean, std::vector<std::vector<float>>& vecVariogram, std::vector<drawParam>& vecParam, std::vector<float>& vecStandDeviation);

	void DrawVariogram(CDC* pDC, CRect& rc, int band, float& fMean, float& fStandDeviation, std::vector<float>& bandVariogram, drawParam& dp);

	void DrawAxis(CDC* pDC, CRect& rc, char* pLabel);

	void SetImage(ImageReader* pImage = NULL)
	{
		m_pImage = pImage;
	}
private:
	CFont* m_pFont;
	CFont* m_pFontSmall;
	ImageReader* m_pImage;
public:
	afx_msg void OnDestroy();
};
