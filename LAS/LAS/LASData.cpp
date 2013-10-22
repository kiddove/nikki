#include "StdAfx.h"
#include "LASData.h"

LASData::LASData(void)
{
	ASSERT( 243 == sizeof(PUBLIC_HEADER_BLOCK) );
}

LASData::~LASData(void)
{
}
