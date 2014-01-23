#pragma once
#pragma pack(1)
// PUBLIC HEADER BLOCK
// 243
//struct PUBLIC_HEADER_BLOCK
//{
//	char szFile_Signature[4];								//"LASF"
//	unsigned short iFile_Source_ID;
//	unsigned short iGlobal_Encoding;
//	unsigned int iProjectID_GUID_data1;
//	unsigned short iProjectID_GUID_data2;
//	unsigned short iProjectID_GUID_data3;
//	char szProjectID_GUID_data4[8];
//	unsigned char iVersion_Major;
//	unsigned char iVersion_Minor;
//	char szSystem_Identifier[32];
//	char szGenerating_Software[32];
//	unsigned short iFile_Creation_Day_of_Year;
//	unsigned short iFile_Creation_Year;
//	unsigned short iHeader_Size;
//	unsigned int iOffset_to_point_data;
//	unsigned int iNumber_of_Varable_Length_Records;
//	unsigned char iPoint_Data_Format_ID;					//(0-99 for spec)
//	unsigned short iPoint_Data_Record_Length;
//	unsigned int iNumber_of_point_records;
//	unsigned int iNumber_of_points_by_return[7];
//	double dX_scale_factor;
//	double dY_scale_factor;
//	double dZ_scale_factor;
//	double dX_offset;
//	double dY_offset;
//	double dZ_offset;
//	double dMax_X;
//	double dMin_X;
//	double dMax_Y;
//	double dMin_Y;
//	double dMax_Z;
//	double dMin_Z;
//	unsigned long long iStart_of_Waveform_Data_Packet_Record;
//};
//

// old  227
struct PUBLIC_HEADER_BLOCK
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
	ULONG iOffset_to_point_data;
	ULONG uNum_of_variable_length_records;
	byte iPoint_Data_Format_ID;
	USHORT iPoint_Data_Record_Length;
	ULONG iNumber_of_point_records;
	ULONG iNumber_of_points_by_return[5];
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
};

// VARIABLE LENGTH RECORDS
// 54
struct VARIABLE_LENGTH_RECORDS
{
	unsigned short iReserved;
	// ASCII character
	char szUser_ID[16];
	unsigned short iRecord_ID;
	unsigned short i_Record_Length_After_Header;
	char szDescription[32];
};

// POINT DATA RECORD
// 20
struct POINT_DATA_RECORD_0
{
	int X;
	int Y;
	int Z;
	unsigned short Intensity;
	char Return_Number : 3;
	char Number_of_Returns : 3;
	char Scan_Direction_Flag : 1;
	char Edge_of_Flight_Line : 1;
	char Classification;
	char Scan_Angle_Rank;
	unsigned char User_Data;
	unsigned short Point_Source_ID;
public:
	POINT_DATA_RECORD_0(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_0()\n"));
	}

	POINT_DATA_RECORD_0(char* pBuffer)
	{
		//memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_0));
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_0));
			//POINT_DATA_RECORD_0* p = (POINT_DATA_RECORD_0*)pBuffer;
			//X = p->X;
			//Y = p->Y;
			//Z = p->Z;
			//Intensity = p->Intensity;
			//Return_Number = p->Return_Number;
			//Number_of_Returns = p->Number_of_Returns;
			//Scan_Direction_Flag = p->Scan_Direction_Flag;
			//Edge_of_Flight_Line = p->Edge_of_Flight_Line;
			//Classification = p->Classification;
			//Scan_Angle_Rank = p->Scan_Angle_Rank;
			//User_Data = p->User_Data;
			//Point_Source_ID = p->Point_Source_ID;
		}
	}
};

// 28
struct POINT_DATA_RECORD_1 : public POINT_DATA_RECORD_0
{
	double GPS_Time;

	POINT_DATA_RECORD_1(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_1()\n"));
	}
	POINT_DATA_RECORD_1(char* pBuffer)
	{
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_1));
		}
	}
};

// 26
struct POINT_DATA_RECORD_2 : public POINT_DATA_RECORD_0
{
	unsigned short Red;
	unsigned short Green;
	unsigned short Blue;
	
	POINT_DATA_RECORD_2(char* pBuffer)
	{
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_2));
		}
	}

	POINT_DATA_RECORD_2(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_2()\n"));
	}
};

// 34
struct POINT_DATA_RECORD_3 : public POINT_DATA_RECORD_1
{
	unsigned short Red;
	unsigned short Green;
	unsigned short Blue;

	POINT_DATA_RECORD_3(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_3()\n"));
	}
	
	POINT_DATA_RECORD_3(char* pBuffer)
	{
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_3));
		}
	}
};

// 57
struct POINT_DATA_RECORD_4 : public POINT_DATA_RECORD_1
{
	unsigned char Wave_Packet_Descriptor_Index;
	unsigned long long Byte_offset_to_waveform_data;
	unsigned int Waveform_packet_size_in_bytes;
	float Return_Point_Waveform_Location;
	float Xt;
	float Yt;
	float Zt;

	POINT_DATA_RECORD_4(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_4()\n"));
	}
	POINT_DATA_RECORD_4(char* pBuffer)
	{
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_4));
		}
	}
};

// 63
struct POINT_DATA_RECORD_5 : public POINT_DATA_RECORD_3
{
	unsigned char Wave_Packet_Descriptor_Index;
	unsigned long long Byte_offset_to_waveform_data;
	unsigned int Waveform_packet_size_in_bytes;
	float Return_Point_Waveform_Location;
	float Xt;
	float Yt;
	float Zt;

	POINT_DATA_RECORD_5(void)
	{
		//TRACE(_T("POINT_DATA_RECORD_5()\n"));
	}
	POINT_DATA_RECORD_5(char* pBuffer)
	{
		if (pBuffer != NULL)
		{
			memcpy(this, pBuffer, sizeof(POINT_DATA_RECORD_5));
		}
	}
};

/*
class CPOINT_DATA_RECORD_0
{
private:
	POINT_DATA_RECORD_0 m_Data;
public:
	CPOINT_DATA_RECORD_0() {};
	CPOINT_DATA_RECORD_0(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_0));
	}

	virtual ~CPOINT_DATA_RECORD_0(void)
	{
	}

	virtual void Output()
	{
	}
	
	virtual int GetReturnNumber()
	{
		return m_Data.Return_Number;
	}
};

class CPOINT_DATA_RECORD_1 : public CPOINT_DATA_RECORD_0
{
private:
	POINT_DATA_RECORD_1 m_Data;
public:
	CPOINT_DATA_RECORD_1() {};
	CPOINT_DATA_RECORD_1(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_1));
	}

	virtual ~CPOINT_DATA_RECORD_1(void)
	{
	}

	virtual void Output()
	{
	}
};

class CPOINT_DATA_RECORD_2 : public CPOINT_DATA_RECORD_0
{
private:
	POINT_DATA_RECORD_2 m_Data;
public:
	CPOINT_DATA_RECORD_2() {};
	CPOINT_DATA_RECORD_2(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_2));
	}

	virtual ~CPOINT_DATA_RECORD_2(void)
	{
	}

	virtual void Output()
	{
	}
};

class CPOINT_DATA_RECORD_3 : public CPOINT_DATA_RECORD_1
{
private:
	POINT_DATA_RECORD_3 m_Data;
public:
	CPOINT_DATA_RECORD_3() {};
	CPOINT_DATA_RECORD_3(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_3));
	}

	virtual ~CPOINT_DATA_RECORD_3(void)
	{
	}

	virtual void Output()
	{
	}
};

class CPOINT_DATA_RECORD_4 : public CPOINT_DATA_RECORD_1
{
private:
	POINT_DATA_RECORD_4 m_Data;
public:
	CPOINT_DATA_RECORD_4() {};
	CPOINT_DATA_RECORD_4(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_4));
	}

	virtual ~CPOINT_DATA_RECORD_4(void)
	{
	}

	virtual void Output()
	{
	}
};

class CPOINT_DATA_RECORD_5 : public CPOINT_DATA_RECORD_3
{
private:
	POINT_DATA_RECORD_5 m_Data;
public:
	CPOINT_DATA_RECORD_5() {};
	CPOINT_DATA_RECORD_5(char* pBuffer)
	{
		memcpy(&m_Data, pBuffer, sizeof(POINT_DATA_RECORD_5));
	}

	virtual ~CPOINT_DATA_RECORD_5(void)
	{
	}

	virtual void Output()
	{
	}
};
*/

class CPOINT_DATA_RECORD
{
public:
	POINT_DATA_RECORD_0* m_pData;

	CPOINT_DATA_RECORD(char *pBuffer, int type)
	{
		if (pBuffer == NULL)
			return;

		switch (type)
		{
		case 0:
			{
				m_pData = new POINT_DATA_RECORD_0(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_0));
			}
			break;
		case 1:
			{
				m_pData = new POINT_DATA_RECORD_1(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_1));
			}
			break;
		case 2:
			{
				m_pData = new POINT_DATA_RECORD_2(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_2));
			}
			break;
		case 3:
			{
				m_pData = new POINT_DATA_RECORD_3(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_3));
			}
			break;
		case 4:
			{
				m_pData = new POINT_DATA_RECORD_4(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_4));
			}
			break;
		case 5:
			{
				m_pData = new POINT_DATA_RECORD_5(pBuffer);
				//memcpy(m_pData, pBuffer, sizeof(POINT_DATA_RECORD_5));
			}
			break;
		default:
			m_pData = NULL;
			break;
		}
	}

	virtual ~CPOINT_DATA_RECORD(void)
	{
		if (m_pData != NULL)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}

	//virtual int GetReturnNumber()
	//{
	//	return m_pData->Return_Number;
	//}

	//virtual void Output()
	//{
	//	// Only X,Y,Z
	//}
};

// for storage
struct LasStorage {
	double x;		// x
	double y;		// y
	double z;		// z
	int r;			// return [0,4]
};

struct DemStorage {
	double x;		// x
	double y;		// y
	double z;		// z
};

struct DemHeader {
	int nCols;
	int nRows;
	double xllcorner;
	double yllcorner;
	int cellsize;
};
struct AdjustStorage {
	double x;
	double y;
	double z;
	int r;
	//bool operator < (const AdjustStorage &a) const
	//{
	//	if (x < a.x)
	//		return true;
	//	else if (x - a.x > 0.00001)
	//		return false;
	//	else // ==
	//	{
	//		if (y < a.y)
	//			return true;
	//		else
	//			return false;
	//	}
	//}
};
// result
struct Result {
	double avg_x;		// min_x + delta_x / 2.0;
	double avg_y;		// min_y + delta_y / 2.0;
	double avg_z;		// (sum of z) / count;
	double standarddeviation;
	double max_z;
	double retrun_per;
};

struct Result_Header {
	double min_x;
	double min_y;
	double max_x;
	double max_y;
	int delta_x;
	int delta_y;
};

class Comparer
{
public:

    bool operator () (const AdjustStorage & va1, const AdjustStorage & va2)
    {
		//// x from min->max,y from min -> max
		//if (va1.x < va2.x)
		//	return true;
		//else if (va1.x - va2.x > 0.00001)
		//	return false;
		//else // ==
		//{
		//	if (va1.y < va2.y)
		//		return true;
		//	else
		//		return false;
		//}

		//// y from min->max, x from min->max
		//if (va1.y < va2.y)
		//	return true;
		//else if (va1.y - va2.y > 0.00001)
		//	return false;
		//else // ==
		//{
		//	if (va1.x < va2.x)
		//		return true;
		//	else
		//		return false;
		//}

		// y from max->min, x from min->max
		if (va1.y > va2.y)
			return true;
		else if (va1.y - va2.y < -0.00001)
			return false;
		else // ==
		{
			if (va1.x < va2.x)
				return true;
			else
				return false;
		}
	}
};


class Compare_Double {
public:
	bool operator () (const double & va1, const double & va2)
	{
		if (va2 - va1 > 0.00001)
			return true;
		else
			return false;
	}
};

#pragma pack()
