// 1104
// time_t----8bit
#pragma once
struct IDXHeader 
{
	char szFileName[40];
	char szAuthor[40];
	int iNumOfFrame;
	int iNumOfBytesPerLine;
	int iNumOfLinesPerFrame;
	int iNumOfBitsPerSample;
	int iSizeOfAncillaryDataBlock;
	time_t tModified;
	int iGPSWeekNum;
	char szUserComments[992];
};

struct IDXRecord 
{
	int iFrameNum;
	double dStartTime;
	double dEndTime;
	fpos_t pFrameStart;
};

// 2048
struct CSDHeader
{
	char szSignature[4];
	char szVendorID[64];
	char szSoftwareVersion[32];
	float fFormatVersion;
	short iSize;
	short iGPSWeek;
	double MinTime;
	double MaxTime;
	int iNumOfRecords;
	short iNumOfStrips;
	int StripPointArray[256];
	double MisalignmentAngles[3];
	double IMUOffsets[3];
	double dTemperature;
	double dPressure;
	char szFreeSpace[830];
};
// 69
struct CSDData 
{
	double dGPSTime;
	byte iPulseCount;
	float Ranges[4];
	short Intensities[4];
	float fScanAngle;
	float fRoll;
	float fPitch;
	float fHeading;
	double dAirCraftLatitude;
	double dAirCraftLongitude;
	float fAirCraftElevation;
};

//struct NDFData 
//{
//};