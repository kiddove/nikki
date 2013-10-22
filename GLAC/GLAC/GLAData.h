#pragma once
#include "DataStruct.h"
#include <set>
#include <algorithm>
#define MAX_PATH_LEN 1024
#define CONFIG_FILE_NAME	"GLAC.ini"

class GLAData
{
public:
	GLAData(void);
	virtual ~GLAData(void);
private:
	//GLA14 m_GLA14;
	//std::map<int, GLA01_MAIN> m_Data01_Main;
	//std::map<int, GLA01_LONG> m_Data01_Long;
	//std::map<int, GLA01_SHORT> m_Data01_Short;
	//std::map<int, GLA14> m_Data14;
	CString m_strIniFilePath;
	CString m_strData14FileDir;
	CString m_strData01FileDir;

	CStdioFile m_LogFile;
	
	// output
	CStdioFile m_DataFile14;
	CStdioFile m_DataFile01;

	void WriteLog(CString strLog);

	void TraversalData14_Little(char* pBuffer);
	void PrintData14_Little(char* pBuffer);

	void TraversalData01_Little(char* pBuffer);
	void PrintData01_Little(char* pBuffer);

	void TraversalData14_Big(char* pBuffer);
	void PrintData14_Big(char* pBuffer);

	void TraversalData01_Big(char* pBuffer);
	void PrintData01_Big(char* pBuffer);
	
	int m_iCount14;
	int m_iCount01;
	
	std::set<int> m_Search_Rec_Index;

	CString m_strMark;
	int m_iLatMin,m_iLatMax,m_iLonMin, m_iLonMax;

public:
	bool Init();

	bool IsInvalid(CString& strFile, int& stLen);

	void ReadGLA01();
	void ReadGLA14();

	//  get file header length && struct length
	void GetLength(CString& strFile, int& headLen, int& stLen);

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
};
