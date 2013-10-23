#pragma once
#include "DataStruct.h"
#define MAX_PATH_LEN 1024
#define CONFIG_FILE_NAME	"LAS.ini"

class LASData
{
public:
	LASData(void);
	virtual ~LASData(void);

private:
	// log
	CStdioFile m_LogFile;
	// little or big endian
	bool m_bBigEndian;

	bool IsBigEndian()
	{
		union
		{
			char str;
			int data;
		};
		data = 0x01020304;
		if (str == 0x01)
		{
			WriteLog("BigEndian");
			return true;
		}
		else
		{
			WriteLog("LittleEndian");
			return false;
		}
	}

	// ini
	CString m_strIniFilePath;
	// file dir
	CString m_strLASDataFileDir;
	//output
	CStdioFile m_LASDataFile;

	//
	int GetStructLength(int type);

	// read from ini
	int m_iReturn_Number;
	bool IsValid(CPOINT_DATA_RECORD* pData);

	void Output(CPOINT_DATA_RECORD* pData, PUBLIC_HEADER_BLOCK* pHeader);
public:
	bool Init();
	void WriteLog(CString strLog);

	void ReadLAS();
};
