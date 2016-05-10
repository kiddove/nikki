#pragma once
#include "Segment.h"
#define BRIGHTNESS	16

class InterfaceFilter
{
public:
	InterfaceFilter(void);
	virtual ~InterfaceFilter(void);
	//InterfaceFilter(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2) = 0;
	virtual CString GetName() = 0;
protected:
	segCondition m_segCon;
};

class EuclideanDistance : public InterfaceFilter
{
public:
	EuclideanDistance(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("Euclidean Distance");
	}
};

////  fitting criterion of Definien
//class NewCriteria : public InterfaceFilter
//{
//public:
//	NewCriteria(segCondition& segCon);
//	float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
//};

//class SimilarityMeasure : public InterfaceFilter
//{
//public:
//	SimilarityMeasure(segCondition& segCon);
//	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
//	virtual CString GetName()
//	{
//		return _T("Similarity Measure");
//	}
//};

// BSSE
class OldBSMSE : public InterfaceFilter
{
public:
	OldBSMSE(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("BSSE");
	}
};

class NewBSMSE : public InterfaceFilter
{
public:
	NewBSMSE(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("New BSMSE");
	}
};

class OldBSMSE_NewEquation : public InterfaceFilter
{
public:
	OldBSMSE_NewEquation(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("OldBSMSE_NewEquation");
	}
};

class NewBSMSE_NewEquation : public InterfaceFilter
{
public:
	NewBSMSE_NewEquation(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("NewBSMSE_NewEquation");
	}
};

// BSSD
class StandardDeviation : public InterfaceFilter
{
public:
	StandardDeviation(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("BSSD");
	}
};

// Spectral angle mapper
class SAM : public InterfaceFilter
{
public:
	SAM(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("SAM");
	}
};

class ENT : public InterfaceFilter
{
public:
	ENT(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("ENT");
	}
};

// Root Squared Error
class RSE : public InterfaceFilter
{
public:
	RSE(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("RSE");
	}
};

// Rooted Difference in Variance
class RDV : public InterfaceFilter
{
public:
	RDV(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("RDV");
	}
};

// Rooted Difference in Squared Error
class RDSE : public InterfaceFilter
{
public:
	RDSE(segCondition& segCon);
	virtual float CalcCoefficient(Segment* pSeg1, Segment* pSeg2);
	virtual CString GetName()
	{
		return _T("RDSE");
	}
};

