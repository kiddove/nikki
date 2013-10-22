#pragma once
#pragma pack(1)
// PUBLIC HEADER BLOCK
// 243
struct PUBLIC_HEADER_BLOCK{
	char szFile_Signature[4];								//(¡°LASF¡±)
	unsigned short iFile_Source_ID;
	unsigned short iGlobal_Encoding;
	unsigned int iProjectID_GUID_data1;
	unsigned short iProjectID_GUID_data2;
	unsigned short iProjectID_GUID_data3;
	char szProjectID_GUID_data4[8];
	unsigned char iVersion_Major;
	unsigned char iVersion_Minor;
	char szSystem_Identifier[32];
	char szGenerating_Software[32];
	unsigned short iFile_Creation_Day_of_Year;
	unsigned short iFile_Creation_Year;
	unsigned short iHeader_Size;
	unsigned int iOffset_to_point_data;
	unsigned int iNumber_of_Varable_Length_Records;
	unsigned char iPoint_Data_Format_ID;					//(0-99 for spec)
	unsigned short iPoint_Data_Record_Length;
	unsigned int iNumber_of_point_records;
	unsigned int iNumber_of_points_by_return[7];
	double dX_scale_factor;
	double dY_scale_factor;
	double dZ_scale_factor;
	double dX_offset;
	double dY_offset;
	double dZ_offset;
	double dMax_X;
	double dMin_X;
	double dMax_Y;
	double dMin_Y;
	double dMax_Z;
	double dMin_Z;
	unsigned long long iStart_of_Waveform_Data_Packet_Record;
};

#pragma pack()