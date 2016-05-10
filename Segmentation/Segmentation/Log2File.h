#pragma once

struct TraceInfo
{
	int row;
	int col;
	int idx;
	bool bTrace;
};

class CLog2File
{
public:
	CLog2File(void);
	virtual ~CLog2File(void);
	static CLog2File* GetInstance();

	bool CreateLogFile();
	//void WriteLog(char* pzFormat, ...);
	void WriteLog(CString strLog);
private:
	bool CloseFile();
	CStdioFile m_LogFile;
	bool m_bCreated;
};

