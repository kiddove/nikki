#include "StdAfx.h"
#include "CSDFile.h"

CCSDFile::CCSDFile(LPCTSTR lpCSDFilePath /* = NULL */,LPCTSTR lpTXTFilePath /* = NULL */)
{
	m_strCSDFilePath = lpCSDFilePath;
	m_strTXTFilePath = lpTXTFilePath;
}

CCSDFile::~CCSDFile(void)
{
}
bool CCSDFile::ParseFile()
{
	TRACE("%d&%d",sizeof(CSDHeader),sizeof(CSDData));
	CFile csdfile;
	if (!csdfile.Open(m_strCSDFilePath,CFile::modeRead))
	{
		int iError = GetLastError();
		AfxMessageBox("Failed When Open csd File");
		return false;
	}

	CStdioFile txtfile;
	int iPos = m_strCSDFilePath.Find(_T('.'));
	m_strTXTFilePath = m_strCSDFilePath.Left(iPos);
	m_strTXTFilePath += _T(".txt");
	//m_strTXTFilePath = _T("d:\\out.txt");
	if (!txtfile.Open(m_strTXTFilePath,CFile::modeWrite|CFile::modeCreate))
	{
		AfxMessageBox("Failed When Open csd File");
		return false;
	}

	// read file header
	CSDHeader fileheader;
	CSDData filedata;

	UINT uSize = sizeof(CSDHeader);

	memset(&fileheader,0,uSize);
	
	csdfile.Read(&fileheader,sizeof(CSDHeader));
	// write to txt
	WriteHeader(txtfile,fileheader);

	txtfile.WriteString(_T("\r\nData:\r\n"));
	txtfile.WriteString(_T("GPSTime\tPulseCount\tRanges1\tRanges2\tRanges3\tRanges4\tIntensities1\tIntensities2\tIntensities3\tIntensities4\tScanAngle\tRoll\tPitch\tHeading\tAircraftLatitude\tAircraftLongitude\tAircraftElevation\t"));
//	txtfile.Close();
	int iNum = fileheader.iNumOfRecords;
	while (iNum>0)
	{
		memset(&filedata,0,sizeof(CSDData));
		csdfile.Read(&filedata,sizeof(CSDData));
		// write to txt
		WriteData(txtfile,filedata);
		iNum--;
		//txtfile.Close();
	}
	csdfile.Close();




	txtfile.Close();
	return true;
}

bool CCSDFile::WriteHeader(CStdioFile& txtfile,CSDHeader& csdheader)
{
	CString strContent;
	strContent.Format(_T("File Header:\r\n"));
	txtfile.WriteString(strContent);

	strContent.Format(_T("Singature:%s\r\n"),CString(csdheader.szSignature));
	txtfile.WriteString(strContent);


	//strContent.Format(_T(""));
	//txtfile.WriteString(strContent);

	strContent.Format(_T("Vendor ID:%s\r\n"),CString(csdheader.szVendorID));
	txtfile.WriteString(strContent);

	strContent.Format(_T("Software Version:%s\r\n"),CString(csdheader.szSoftwareVersion));
	txtfile.WriteString(strContent);

	strContent.Format(_T("Format Version:%.2f\r\n"),csdheader.fFormatVersion);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Header Size:%d\r\n"),csdheader.iSize);
	txtfile.WriteString(strContent);

	strContent.Format(_T("GPS Week:%d\r\n"),csdheader.iGPSWeek);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Min Time:%.2f\r\n"),csdheader.MinTime);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Max Time:%.2f\r\n"),csdheader.MaxTime);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Number Of Records:%d\r\n"),csdheader.iNumOfRecords);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Number Of Strips:%d\r\n"),csdheader.iNumOfStrips);
	txtfile.WriteString(strContent);


	for (int i=0;i<256;i++)
	{
		strContent.Format(_T("Strip Point Array[%d]:%d\r\n"),i,csdheader.StripPointArray[i]);
		txtfile.WriteString(strContent);
	}

	for (int i=0;i<3;i++)
	{
		strContent.Format(_T("Misalignment Angles[%d]:%.2f\r\n"),i,csdheader.MisalignmentAngles[i]);
		txtfile.WriteString(strContent);
	}

	for (int i=0;i<3;i++)
	{
		strContent.Format(_T("IMU Offsets[%d]:%.2f\r\n"),i,csdheader.IMUOffsets[i]);
		txtfile.WriteString(strContent);
	}

	strContent.Format(_T("Temperature:%.2f\r\n"),csdheader.dTemperature);
	txtfile.WriteString(strContent);

	strContent.Format(_T("Pressure:%.2f\r\n"),csdheader.dPressure);
	txtfile.WriteString(strContent);
	return true;
}

bool CCSDFile::WriteData(CStdioFile& txtfile,CSDData& csddata)
{
	CString strContent;
	strContent.Format(_T("\r\n%.6f\t"),csddata.dGPSTime);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%d\t"),csddata.iPulseCount);
	txtfile.WriteString(strContent);

	for (int i=0;i<4;i++)
	{
		strContent.Format(_T("%f\t"),csddata.Ranges[i]);
		txtfile.WriteString(strContent);
	}

	for (int i=0;i<4;i++)
	{
		strContent.Format(_T("%d\t"),csddata.Intensities[i]);
		txtfile.WriteString(strContent);
	}

	strContent.Format(_T("%f\t"),csddata.fScanAngle);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%f\t"),csddata.fRoll);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%f\t"),csddata.fPitch);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%f\t"),csddata.fHeading);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%.6f\t"),csddata.dAirCraftLatitude);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%.6f\t"),csddata.dAirCraftLongitude);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%.6f\t"),csddata.fAirCraftElevation);
	txtfile.WriteString(strContent);
	//	float fRoll;
	//	float fPitch;
	//	float fHeading;
	//	double dAirCraftLatitude;
	//	double dAirCraftLongitude;
	//	float fAirCraftElevation;
	//};
	return true;
}