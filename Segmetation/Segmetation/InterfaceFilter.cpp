#include "stdafx.h"
#include "InterfaceFilter.h"


InterfaceFilter::InterfaceFilter(void)
{
}


InterfaceFilter::~InterfaceFilter(void)
{
}

float NewCriteria::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float u = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
		//f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
		f += (pSeg1->m_Avg[i] * pSeg1->m_Avg[i] * n1 + pSeg2->m_Avg[i] * pSeg2->m_Avg[i] * n2 - (n1 + n2) * u * u);
	}
	float ed = sqrt(f / (float)pSeg1->m_Avg.size());
	return ed;
}

float EuclideanDistance::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	// pSeg1 && pSeg2 must be valid before enter
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}

	float ed = 1e8;
	if (f > 0.0001)
		ed = sqrt(f);

	return ed;
}