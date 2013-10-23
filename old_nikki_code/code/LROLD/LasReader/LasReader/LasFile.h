#pragma once
#include <map>
#include <algorithm>

struct Public_Header_Block 
{
	char szFile_Signature[4]; // "LASF"
	ULONG uReserved;
	ULONG uGUID_Data1;
	USHORT uGUID_Data2;
	USHORT uGUID_Data3;
	byte uGUID_Data4[8];
	byte uVersion_Major;
	byte uVersion_Minor;
	char szSystem_Identifier[32];
	char szGenerating_SoftWare[32];
	USHORT uFlight_Date_Julian;
	USHORT uYear;
	USHORT uHearder_Size;
	ULONG uOffset_to_data;
	ULONG uNum_of_variable_length_records;
	byte uPoint_Data_Format_ID;
	USHORT uPoint_Data_Record_Length;
	ULONG uNum_of_point_records;
	ULONG uNum_of_points_by_return[5];
	double dX_scale_factor;
	double dY_scale_factor;
	double dZ_scale_factor;
	double dX_Offset;
	double dY_Offset;
	double dZ_Offset;
	double dMax_X;
	double dMin_X;
	double dMax_Y;
	double dMin_Y;
	double dMax_Z;
	double dMin_Z;
};
struct Variable_Length_Record 
{
	USHORT uRecord_Signature;
	char szUser_ID[16];
	USHORT uRecord_ID;
	USHORT uRecord_Length_After_Header;
	char szDiscription[32];
};
struct Point_Data_Record 
{
	long x;
	long y;
	long z;
	USHORT uIntensity;
	BYTE szReturn_Num : 3; //3 bits
	BYTE szNum_of_Returns : 3; // 3 bits
	BYTE Scan_Direction_Flag : 1; // 1 bit
	BYTE Edge_of_Flight_Line: 1; // 1 bit
	byte uClassification;
	char uScan_Angle_Range;
	byte uFile_Marker;
	USHORT uUser_Bit_Field;
	double dGPS_Time;
};

class CLasFile
{
public:
	CLasFile(LPCTSTR lpFilePath = NULL);
	~CLasFile(void);
private:
	CString m_strFilePath;
public:
	std::map<CString,UINT> m_map;
public:
	void ParseFile();
	void ReadFile(int iIndex);
	Public_Header_Block m_header;
	CStdioFile txtfile;
	CFile lasfile;
	void WriteTitle();
};
