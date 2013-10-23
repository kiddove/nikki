#include "StdAfx.h"
#include "NDFFile.h"

CNDFFile::CNDFFile(LPCTSTR lpNDFFilePath /* = NULL */)
{
	m_strNDFFilePath = lpNDFFilePath;
}

CNDFFile::~CNDFFile(void)
{
}

void CNDFFile::ParseFile()
{
	CString stridxtxt;
	int iPos = m_strNDFFilePath.Find(_T('.'));
	m_strTXTFilePath = m_strNDFFilePath.Left(iPos);
	m_strIDXFilePath = m_strNDFFilePath.Left(iPos);
	stridxtxt = m_strTXTFilePath + _T("_idx.txt");
	m_strTXTFilePath += _T(".txt");
	m_strIDXFilePath += _T(".idx");
	CIDXFile idxfile(m_strIDXFilePath);
	idxfile.ParseFile();
	CFile idx_file,ndf_file;
	CStdioFile txt_file,idx_txt_file;
	if (!idx_file.Open(m_strIDXFilePath,CFile::modeRead))
	{
		AfxMessageBox("Failed to open idx file");
		return;
	}
	if (!ndf_file.Open(m_strNDFFilePath,CFile::modeRead))
	{
		AfxMessageBox("Failed to open ndf file");
		idx_file.Close();
		return;
	}
	if (!txt_file.Open(m_strTXTFilePath,CFile::modeWrite|CFile::modeCreate))
	{
		AfxMessageBox("Failed to create txt file");
		idx_file.Close();
		ndf_file.Close();
		return;
	}

	if (!idx_txt_file.Open(stridxtxt,CFile::modeWrite|CFile::modeCreate))
	{
		AfxMessageBox("Failed to create txt file");
		idx_file.Close();
		ndf_file.Close();
		txt_file.Close();
		return;
	}
	// 0

	USHORT nLineNum;
	USHORT nOffset2NextLine;
	BYTE WaveData[40];
	BYTE nNumOfBytes;
	USHORT nTimeInerval;
	memset(&m_idxheader,0,sizeof(IDXHeader));
	idx_file.Read(&m_idxheader,sizeof(IDXHeader));

	idx_txt_file.WriteString(_T("IDX Header:\r\n"));
	CString strIdx;
	strIdx.Format(_T("FileName:%s\r\n"),m_idxheader.szFileName);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Author:%s\r\n"),m_idxheader.szAuthor);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Number of Frame:%d\r\n"),m_idxheader.iNumOfFrame);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Number of Bytes PerLine:%d\r\n"),m_idxheader.iNumOfBytesPerLine);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Number of Lines PerSample:%d\r\n"),m_idxheader.iNumOfBitsPerSample);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Number of Bits PerSample:%d\r\n"),m_idxheader.iNumOfBitsPerSample);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Size of Ancillary Data Block:%d\r\n"),m_idxheader.iSizeOfAncillaryDataBlock);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("Time Modified:%I64d\r\n"),m_idxheader.tModified);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("GPS Week Number:%d\r\n"),m_idxheader.iGPSWeekNum);
	idx_txt_file.WriteString(strIdx);
	strIdx.Format(_T("User Comments:%s\r\n"),m_idxheader.szUserComments);
	idx_txt_file.WriteString(strIdx);
	//char szFileName[40];
	//char szAuthor[40];
	//int iNumOfFrame;
	//int iNumOfBytesPerLine;
	//int iNumOfLinesPerFrame;
	//int iNumOfBitsPerSample;
	//int iSizeOfAncillaryDataBlock;
	//time_t tModified;
	//int iGPSWeekNum;
	//char szUserComments[992];

	memset(&m_idxrecord,0,sizeof(IDXRecord));
	idx_file.Read(&m_idxrecord,sizeof(IDXRecord));

	idx_txt_file.WriteString(_T("IDX Record:\r\n"));
	idx_txt_file.WriteString(_T("Frame Number\tStart Time\tEnd Time\tFrame Start\t\r\n"));
	strIdx.Format(_T("%d\t%.6f\t%.6f\t%d\t\r\n"),m_idxrecord.iFrameNum,m_idxrecord.dStartTime,m_idxrecord.dEndTime,m_idxrecord.pFrameStart);
	idx_txt_file.WriteString(strIdx);
	/*idx_txt_file.WriteString(_T(""));
	idx_txt_file.WriteString(_T(""));*/
	//int iFrameNum;
	//double dStartTime;
	//double dEndTime;
	//fpos_t pFrameStart;

	//read ndf
	// read frame
	CString strContent;
	for (int i = 0; i<m_idxheader.iNumOfFrame; i++)
	{
		memset(&m_idxrecord,0,sizeof(IDXRecord));
		idx_file.Read(&m_idxrecord,sizeof(IDXRecord));
		idx_txt_file.WriteString(_T("IDX Record:\r\n"));
		idx_txt_file.WriteString(_T("Frame Number\tStart Time\tEnd Time\tFrame Start\t\r\n"));
		strIdx.Format(_T("%d\t%.6f\t%.6f\t%d\t\r\n"),m_idxrecord.iFrameNum,m_idxrecord.dStartTime,m_idxrecord.dEndTime,m_idxrecord.pFrameStart);
		idx_txt_file.WriteString(strIdx);

		ndf_file.Seek(m_idxrecord.pFrameStart,CFile::begin);

		strContent.Format(_T("Frame %d:\r\n"),i+1);
		txt_file.WriteString(strContent);
		// read line
		// 16183
		//strContent.Format(_T("Line\tOffset to next line\t"));
		//txt_file.WriteString(strContent);
		for (int k=0;k<m_idxheader.iNumOfLinesPerFrame;k++)
		{
			ndf_file.Read(&nLineNum,sizeof(USHORT));
			strContent.Format(_T("Line %d:\r\n"),k+1);
			txt_file.WriteString(strContent);
			ndf_file.Read(&nOffset2NextLine,sizeof(USHORT));
			strContent.Format(_T("Offset to next line:%d\r\n"),nOffset2NextLine);
			txt_file.WriteString(strContent);
			ndf_file.Read(WaveData,40*sizeof(BYTE));
			for (int iw=0;iw<40;iw++)
			{
				strContent.Format(_T("Wavaform Data[%d]:%d\r\n"),iw+1,WaveData[iw]);
				txt_file.WriteString(strContent);
			}
			USHORT nRest = nOffset2NextLine+4-sizeof(USHORT)*2-40*sizeof(BYTE);
			// read segment
			int pc=0;
			while (nRest>0)
			{
				//strContent.Format(_T("Number Of Bytes in %d:"),++pc);
				ndf_file.Read(&nNumOfBytes,sizeof(BYTE));
				strContent.Format(_T("Number Of Bytes in %d Waveform Segment:%d\r\n"),++pc,nNumOfBytes);
				txt_file.WriteString(strContent);
				LPBYTE pBuffer = new BYTE [nNumOfBytes];
				memset(pBuffer,0,nNumOfBytes*sizeof(BYTE));

				ndf_file.Read(pBuffer,nNumOfBytes*sizeof(BYTE));
				for (int iw=0;iw<nNumOfBytes;iw++)
				{
					strContent.Format(_T("Waveform Segment Data[%d]:%d\r\n"),iw+1,*(pBuffer+iw));
					txt_file.WriteString(strContent);
				}
				delete[] pBuffer;
				ndf_file.Read(&nTimeInerval,sizeof(USHORT));
				strContent.Format(_T("TimeInterval:%d\r\n"),nTimeInerval);
				txt_file.WriteString(strContent);
				nRest -= nNumOfBytes+sizeof(USHORT)+sizeof(BYTE);
			}	
		}
	}
	// end
	memset(&m_idxrecord,0,sizeof(IDXRecord));
	idx_file.Read(&m_idxrecord,sizeof(IDXRecord));
	idx_txt_file.WriteString(_T("IDX Record:\r\n"));
	idx_txt_file.WriteString(_T("Frame Number\tStart Time\tEnd Time\tFrame Start\t\r\n"));
	strIdx.Format(_T("%d\t%.6f\t%.6f\t%d\t\r\n"),m_idxrecord.iFrameNum,m_idxrecord.dStartTime,m_idxrecord.dEndTime,m_idxrecord.pFrameStart);
	idx_txt_file.WriteString(strIdx);

	idx_txt_file.Close();
	idx_file.Close();
	ndf_file.Close();
	txt_file.Close();
}