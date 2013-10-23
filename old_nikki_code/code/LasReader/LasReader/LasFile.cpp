#include "StdAfx.h"
#include "LasFile.h"

CLasFile::CLasFile(LPCTSTR lpFilePath /* = NULL */,int iRetNo /* = 5 */)
{
	m_strFilePath = lpFilePath;
	m_iRetNo = iRetNo;
}

CLasFile::~CLasFile(void)
{
}

void CLasFile::ParseFile()
{
	//AfxMessageBox(strPath);
	TRACE("\r\nHeader:%d",sizeof(Public_Header_Block));
	TRACE("\r\nVariableRecord:%d",sizeof(Variable_Length_Record));
	TRACE("\r\nPointDataRecord:%d",sizeof(Point_Data_Record));
//	CFile lasfile;
//	CStdioFile txtfile;
	CStdioFile newlasfile;
	CString strnewlasfile;
	int ipos = m_strFilePath.Find(_T('.'));
	CString strTxt = m_strFilePath.Left(ipos);
	strTxt += _T(".txt");
	ipos = m_strFilePath.ReverseFind(_T('\\'));
	strnewlasfile = m_strFilePath.Left(ipos+1)+_T("Z.txt");
	txtfile.Open(strTxt,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone);
	lasfile.Open(m_strFilePath,CFile::modeRead|CFile::shareDenyWrite);
	newlasfile.Open(strnewlasfile,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite);
	//Public_Header_Block header;
	Variable_Length_Record variable_Record;
	Point_Data_Record point_record_data;
	BYTE buffer[256];
	lasfile.Read(&m_header,sizeof(m_header));
	// write header to txt
	//txtfile.WriteString(_T("Header Block:\r\n"));

	//newlasfile.Write(&m_header,sizeof(m_header));

	CString strContent;
	//strContent.Format(_T("%s\r\n"),header.szFile_Signature);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uReserved);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uGUID_Data1);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uGUID_Data2);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uGUID_Data3);
	//txtfile.WriteString(strContent);

	//for (int i=0;i<4;i++)
	//{
	//	strContent.Format(_T("%d\r\n"),header.uGUID_Data4[i]);
	//	//txtfile.WriteString(strContent);
	//}

	//strContent.Format(_T("%d\r\n"),header.uVersion_Major);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uVersion_Minor);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%s\r\n"),header.szSystem_Identifier);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%s\r\n"),header.szGenerating_SoftWare);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uFlight_Date_Julian);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uYear);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uHearder_Size);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uOffset_to_data);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uNum_of_variable_length_records);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uPoint_Data_Format_ID);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uPoint_Data_Record_Length);
	//txtfile.WriteString(strContent);

	//strContent.Format(_T("%d\r\n"),header.uNum_of_point_records);
	//txtfile.WriteString(strContent);

//	for (int i=0;i<5;i++)
//	{
//		strContent.Format(_T("%d\r\n"),header.uNum_of_points_by_return[i]);
//		txtfile.WriteString(strContent);
//	}
//
//	strContent.Format(_T("%.6lf\r\n"),header.dX_scale_factor);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dY_scale_factor);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dZ_scale_factor);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dX_Offset);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dY_Offset);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dZ_Offset);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMax_X);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMin_X);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMax_Y);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMin_Y);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMax_Z);
//	txtfile.WriteString(strContent);
//
//	strContent.Format(_T("%.6lf\r\n"),header.dMin_Z);
//	txtfile.WriteString(strContent);
//
////	txtfile.Close();
///*	strContent.Format(_T("\r\n"));
//	txtfile.WriteString(strContent);*/
//
//
//	txtfile.WriteString(_T("\r\nVariable Length Record:\r\n"));
	for (UINT i=0; i<m_header.uNum_of_variable_length_records;i++)
	{
		lasfile.Read(&variable_Record,sizeof(Variable_Length_Record));
		//newlasfile.Write(&variable_Record,sizeof(Variable_Length_Record));
		//strContent.Format(_T("0x%.4x\r\n"),variable_Record.uRecord_Signature);
		//txtfile.WriteString(strContent);

		//strContent.Format(_T("%s\r\n"),variable_Record.szUser_ID);
		//txtfile.WriteString(strContent);

		//strContent.Format(_T("%d\r\n"),variable_Record.uRecord_ID);
		//txtfile.WriteString(strContent);

		//strContent.Format(_T("%d\r\n"),variable_Record.uRecord_Length_After_Header);
		//txtfile.WriteString(strContent);

		//strContent.Format(_T("%s\r\n"),variable_Record.szDiscription);
		//txtfile.WriteString(strContent);

		memset(buffer,0,256);
		//lasfile.Seek(variable_Record.uRecord_Length_After_Header,CFile::current);
		lasfile.Read(buffer,variable_Record.uRecord_Length_After_Header);
		//newlasfile.Write(buffer,variable_Record.uRecord_Length_After_Header);
	}
	/*BYTE Buffer[2];
	lasfile.Read(Buffer,2);*/

	//txtfile.WriteString(_T("\r\nPoint Data:\r\n"));
	//txtfile.WriteString(_T("X\tY\tZ\tIntensity\tReturn Number\tNumber of Returns(given pulse)\t"));
	//txtfile.WriteString(_T("Scan Direction Flag\tEdge of Flight Line\tClassification\t"));
	//txtfile.WriteString(_T("Scan Angle Rank(-90 to +90) - Left side\tFile Maker\t"));
	//txtfile.WriteString(_T("User Bit Field\tGPS Time\t\r\n"));

	newlasfile.WriteString(_T("\r\nPoint Data:\r\n"));
	newlasfile.WriteString(_T("X\tY\tZ\tIntensity\tReturn Number\tNumber of Returns(given pulse)\t"));
	newlasfile.WriteString(_T("Scan Direction Flag\tEdge of Flight Line\tClassification\t"));
	newlasfile.WriteString(_T("Scan Angle Rank(-90 to +90) - Left side\tFile Maker\t"));
	newlasfile.WriteString(_T("User Bit Field\tGPS Time\t\r\n"));
//	txtfile.Close();
	lasfile.Seek(m_header.uOffset_to_data,CFile::begin);
	//newlasfile.Seek(m_header.uOffset_to_data,CFile::begin);

	//if (m_iRetNo!=5)
	//{
	//	for (UINT i=0;i<m_header.uNum_of_point_records;i++)
	//	{
	//		lasfile.Read(&point_record_data,sizeof(Point_Data_Record));

	//		if (point_record_data.szReturn_Num == m_iRetNo)
	//		{
	//			//newlasfile.Write(&point_record_data,sizeof(Point_Data_Record));
	//			strContent.Format(_T("%.2f-%.2f-%.2f"),
	//				((double)point_record_data.x*m_header.dX_scale_factor+m_header.dX_Offset),
	//				((double)point_record_data.y*m_header.dY_scale_factor+m_header.dY_Offset),
	//				((double)point_record_data.z*m_header.dZ_scale_factor+m_header.dZ_Offset));
	//			m_map.insert(std::make_pair(strContent,i));
	//		}
	//		//txtfile.WriteString(strContent);

	//		//strContent.Format(_T("%d\t%d\t%d\t%d\t"),point_record_data.szReturn_Num,point_record_data.szNum_of_Returns,point_record_data.Scan_Direction_Flag,point_record_data.Edge_of_Flight_Line);
	//		//txtfile.WriteString(strContent);

	//		//strContent.Format(_T("%d\t%d\t%d\t%d\t%.6lf\r\n"),point_record_data.uClassification,point_record_data.uScan_Angle_Range,point_record_data.uFile_Marker,point_record_data.uUser_Bit_Field,point_record_data.dGPS_Time);
	//		//txtfile.WriteString(strContent);
	//	}

	//}
	//else
//	{
		for (UINT i=0;i<m_header.uNum_of_point_records;i++)
		{
			lasfile.Read(&point_record_data,sizeof(Point_Data_Record));
			if (point_record_data.szNum_of_Returns == m_iRetNo)
			{
				//newlasfile.Write(&point_record_data,sizeof(Point_Data_Record));
				strContent.Format(_T("%.2f-%.2f-%.2f"),
					((double)point_record_data.x*m_header.dX_scale_factor+m_header.dX_Offset),
					((double)point_record_data.y*m_header.dY_scale_factor+m_header.dY_Offset),
					((double)point_record_data.z*m_header.dZ_scale_factor+m_header.dZ_Offset));
				m_map.insert(std::make_pair(strContent,i));
				//txtfile.WriteString(strContent);
				strContent.Format(_T("%.2f\t%.2f\t%.2f\t"),
					((double)point_record_data.x*m_header.dX_scale_factor+m_header.dX_Offset),
					((double)point_record_data.y*m_header.dY_scale_factor+m_header.dY_Offset),
					((double)point_record_data.z*m_header.dZ_scale_factor+m_header.dZ_Offset));
				newlasfile.WriteString(strContent);

				strContent.Format(_T("%d\t%d\t%d\t%d\t%d\t"),point_record_data.uIntensity,point_record_data.szReturn_Num,point_record_data.szNum_of_Returns,point_record_data.Scan_Direction_Flag,point_record_data.Edge_of_Flight_Line);
				//txtfile.WriteString(strContent);
				newlasfile.WriteString(strContent);

				strContent.Format(_T("%d\t%d\t%d\t%d\t%.6lf\r\n"),point_record_data.uClassification,point_record_data.uScan_Angle_Range,point_record_data.uFile_Marker,point_record_data.uUser_Bit_Field,point_record_data.dGPS_Time);
				//txtfile.WriteString(strContent);
				newlasfile.WriteString(strContent);
			}
		}
//	}
	newlasfile.Close();
//	lasfile.Close();
//	txtfile.Close();
}
void CLasFile::WriteTitle()
{
	txtfile.WriteString(_T("\r\nPoint Data:\r\n"));
	txtfile.WriteString(_T("X\tY\tZ\tIntensity\tReturn Number\tNumber of Returns(given pulse)\t"));
	txtfile.WriteString(_T("Scan Direction Flag\tEdge of Flight Line\tClassification\t"));
	txtfile.WriteString(_T("Scan Angle Rank(-90 to +90) - Left side\tFile Maker\t"));
	txtfile.WriteString(_T("User Bit Field\tGPS Time\t\r\n"));
}
void CLasFile::ReadFile(int iIndex)
{
	CString strContent;
	//CFile lasfile;
	//CStdioFile txtfile;
	//int ipos = m_strFilePath.Find(_T('.'));
	//CString strTxt = m_strFilePath.Left(ipos);
	//strTxt += _T(".txt");
	//txtfile.Open(strTxt,CFile::modeCreate|CFile::modeWrite);
	//lasfile.Open(m_strFilePath,CFile::modeRead|CFile::shareDenyWrite);
	lasfile.Seek(m_header.uOffset_to_data + iIndex*sizeof(Point_Data_Record),CFile::begin);

	Point_Data_Record point_record_data;
	lasfile.Read(&point_record_data,sizeof(Point_Data_Record));

	strContent.Format(_T("%.6f\t%.6f\t%.6f\t%d\t"),
		((double)point_record_data.x*m_header.dX_scale_factor+m_header.dX_Offset),
		((double)point_record_data.y*m_header.dY_scale_factor+m_header.dY_Offset),
		((double)point_record_data.z*m_header.dZ_scale_factor+m_header.dZ_Offset),
		point_record_data.uIntensity);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%d\t%d\t%d\t%d\t"),point_record_data.szReturn_Num,point_record_data.szNum_of_Returns,point_record_data.Scan_Direction_Flag,point_record_data.Edge_of_Flight_Line);
	txtfile.WriteString(strContent);

	strContent.Format(_T("%d\t%d\t%d\t%d\t%.6lf\r\n"),point_record_data.uClassification,point_record_data.uScan_Angle_Range,point_record_data.uFile_Marker,point_record_data.uUser_Bit_Field,point_record_data.dGPS_Time);
	txtfile.WriteString(strContent);
}
