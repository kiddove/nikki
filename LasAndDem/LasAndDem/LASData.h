#pragma once
#include "DataStruct.h"
#define MAX_PATH_LEN 1024
#define CONFIG_FILE_NAME	"LAS.ini"
#include <vector>
//#include <set>
#include <algorithm>

#ifdef DEM
#define STEP 1
#endif

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
#ifdef NEEDTXT
	//output txt
	CStdioFile m_LASDataFile;
#endif

	//
	int GetStructLength(int type);

	// read from ini
	int m_iReturn_Number;
	bool IsValid(CPOINT_DATA_RECORD* pData);
	PUBLIC_HEADER_BLOCK m_phb;
#ifdef NEEDTXT
	void Output(CPOINT_DATA_RECORD* pData, PUBLIC_HEADER_BLOCK* pHeader);
#endif

#ifdef DEM
	CString m_strDEMDataFile;
	std::vector<LasStorage> m_LasOriginal;
	std::vector<DemStorage> m_DemOriginal;
	DemHeader m_DemHeader;
	//std::set<AdjustStorage> m_Adjust;
	std::vector<AdjustStorage> m_Adjust;

	// 3 dimension
	std::vector<std::vector<std::vector<AdjustStorage>>> m_arrAdjust;

	Result_Header m_resultHeader;
#endif

public:
	bool Init();
	void WriteLog(CString strLog);

	void ReadLAS();
#ifdef DEM
	void ReadDEM();
	void Adjust();

	void LoadAdjust();
	void Calculate();
#endif
};
