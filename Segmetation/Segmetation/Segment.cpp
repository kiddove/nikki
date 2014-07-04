#include "StdAfx.h"
#include "Segment.h"

Segment::Segment(void)
{
	//::InitializeCriticalSection(&cs);
	m_bValid = true;
}

Segment::~Segment(void)
{
	//::DeleteCriticalSection(&cs);
}

//float Segment::GetAvfValue()
//{
//	float fValue = 0.0;
//	for (int i = 0; i < m_area.size(); i++)
//	{
//		
//	}
//}