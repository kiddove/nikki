#pragma once

#include "gdal_priv.h"

struct ImgInfo
{
	int iType;
	int iWidth;
	int iHeight;
};
struct CalCondition
{
	int x;		// blocksize
	int y;
	int angle;	//0, 45, 90, 135
	int distance;
	int graynum;
};

class GLCMData
{
public:
	GLCMData(void);
	~GLCMData(void);

public:
	bool LoadFromFile(LPTSTR strFilePath);

	bool calGLCM(CalCondition& calCon);
	void calGLCM8();
	void calGLCM16();
private:
	char* m_pData;

	CString m_strFilePath;
	ImgInfo m_imgInfo;
	CalCondition m_calCon;
};
