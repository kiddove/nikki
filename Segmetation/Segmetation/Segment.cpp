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