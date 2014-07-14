#pragma once
#include "Segment.h"

class InterfaceFilter
{
public:
	InterfaceFilter(void);
	virtual ~InterfaceFilter(void);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2) = 0;
};

class EuclideanDistance : public InterfaceFilter
{
public:
	float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
};

class NewCriteria : public InterfaceFilter
{
	public:
	float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
};