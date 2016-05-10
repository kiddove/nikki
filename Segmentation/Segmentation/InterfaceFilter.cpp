#include "stdafx.h"
#include "InterfaceFilter.h"

#define PI 3.14159265

std::vector<cv::Point> FindBoundary(std::vector<cv::Point>& points)
{
	std::vector<cv::Point> boundary;

	if (points.empty())
		return boundary;

	// use opencv convexhull
	vector<int> hull;
	convexHull(Mat(points), hull, true);
	//std::vector<cv::Point>().swap(pSeg->m_Boundary);

	int hullCount = (int)hull.size();

	for (int i = 0; i < hullCount; i++)
	{
		boundary.push_back(points[hull[i]]);
	}
	return boundary;
}

// get maxDistande of segment
float GetMaxDistance(std::vector<cv::Point>& points)
{
	if (points.empty())
		return 0.0f;
	// get radius of minEnclosingCircle

	float radius = 0.0;
	Point2f center;
	minEnclosingCircle(Mat(points), center, radius);

	return 2.0f * radius;
}

InterfaceFilter::InterfaceFilter(void)
{
}


InterfaceFilter::~InterfaceFilter(void)
{
}

//SimilarityMeasure::SimilarityMeasure(segCondition& segCon)
//{
//	memcpy(&m_segCon, &segCon, sizeof(segCondition));
//}

EuclideanDistance::EuclideanDistance(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

OldBSMSE::OldBSMSE(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

NewBSMSE::NewBSMSE(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

OldBSMSE_NewEquation::OldBSMSE_NewEquation(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

NewBSMSE_NewEquation::NewBSMSE_NewEquation(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

StandardDeviation::StandardDeviation(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

SAM::SAM(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

ENT::ENT(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

RSE::RSE(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

RDV::RDV(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

RDSE::RDSE(segCondition& segCon)
{
	memcpy(&m_segCon, &segCon, sizeof(segCondition));
}

float OldBSMSE::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		// old
		float u = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
		f += (pSeg1->m_Avg[i] * pSeg1->m_Avg[i] * n1 + pSeg2->m_Avg[i] * pSeg2->m_Avg[i] * n2 - (n1 + n2) * u * u);
	}
	float ed = sqrt(f);
	return ed;
}

float NewBSMSE::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		// new
		float u = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
		f += (pSeg1->m_Avg[i] * pSeg1->m_Avg[i] * n1 + pSeg2->m_Avg[i] * pSeg2->m_Avg[i] * n2) / (float)(n1 + n2) - u * u;
	}
	float ed = sqrt(f);
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
	if (f > 0.0000001)
		ed = sqrt(f);
	else
		//TRACE(_T("%.6f\n"), f);
		ed = 0.0f;

	return ed;
}

//float SimilarityMeasure::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
//{
//	vector<cv::Point> points, boundary;
//	points.insert(points.end(), pSeg1->m_area.begin(), pSeg1->m_area.end());
//	points.insert(points.end(), pSeg2->m_area.begin(), pSeg2->m_area.end());
//	
//	boundary = FindBoundary(points);
//	float maxDistance = GetMaxDistance(points);
//
//	float smoothness = 0.0f;
//	float compactness = 0.0f;
//
//	smoothness = (float)points.size() / (float)boundary.size();
//	compactness = maxDistance / 2.0f * sqrt((float)points.size() / 3.1415926f);
//	float fValue = 0.0f;
//
//	int n1 = pSeg1->m_area.size();
//	int n2 = pSeg2->m_area.size();
//
//	float fAvg1 = 0.0f, fAvg2 = 0.0f;
//
//	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
//	{
//		fAvg1 += pSeg1->m_Avg[i];
//		fAvg2 += pSeg2->m_Avg[i];
//		//fValue += sqrt((n1*n2/(n1+n2)) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i])) * (1 + m_segCon.alpha * log(compactness) + m_segCon.beta * log(smoothness));
//	}
//
//	fAvg1 /=  (float)pSeg1->m_Avg.size();
//	fAvg2 /=  (float)pSeg2->m_Avg.size();
//	fValue += sqrt((n1*n2/(n1+n2)) * (fAvg1 - fAvg2) * (fAvg1 - fAvg2)) * (1 + m_segCon.alpha * log(compactness) + m_segCon.beta * log(smoothness));
//	//sqrt((n1*n2/(n1+n2)) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i])) * (1 + m_segCon.alpha * log(compactness) + m_segCon.beta * log(smoothness))
//
//	return fValue;
//}

float OldBSMSE_NewEquation::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += (float)(n1 * n2) / (float)(n1 + n2) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}
	float ed = sqrt(f);
	return ed;
}

float NewBSMSE_NewEquation::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		f += n1 * n2 * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[i] - pSeg2->m_Avg[i]);
	}
	float ed = sqrt(f) / (float)(n1 + n2);
	return ed;
}

//float StandardDeviation::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
//{
//	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
//	// to be continued...
//	int n1 = pSeg1->m_area.size();
//	int n2 = pSeg2->m_area.size();
//
//	float f = 0.0f;
//	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
//	{
//		// StandardDeviation, do not take root
//		float avg = (n1 * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i]) / (float)(n1 + n2);
//		float standarddeviation = (n1 * pSeg1->m_StandardDeviation[i] * pSeg1->m_StandardDeviation[i] + n2 * pSeg2->m_StandardDeviation[i] * pSeg2->m_StandardDeviation[i]) / (float)(n1 + n2)
//			+ (n1 * pSeg1->m_Avg[i] * pSeg1->m_Avg[i] + n2 * pSeg2->m_Avg[i] * pSeg2->m_Avg[i]) / (float)(n1 + n2) - avg * avg;
//		ASSERT(standarddeviation > 0.0001);
//
//		//standarddeviation = sqrt(standarddeviation);
//		
//		f += standarddeviation;
//	}
//	//float ed = sqrt(f);
//	return f;
//}

float SAM::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float a = 0.0f, b = 0.0f, AB = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		// spectral angle mapper
		a += pSeg1->m_Avg[i] * pSeg1->m_Avg[i];
		b += pSeg2->m_Avg[i] * pSeg2->m_Avg[i];
		AB += pSeg1->m_Avg[i] * pSeg2->m_Avg[i];
	}
	// degree
	float f = 0.0f;

	// acos --- Returns the principal value of the arc cosine of x, expressed in radians.
	f = acos(AB / sqrt(a * b));
	f = f * 180.0f / (float)PI;
	ASSERT(f > -0.000001f && f < 180.00001f);
	return f;
}

float ENT::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float fEntropy = 0.0;
	int iBand = pSeg1->m_Avg.size();
	for (int b = 0; b < iBand; b++)
	{
		ASSERT(pSeg1->m_vecBrightness[b].size() == pSeg2->m_vecBrightness[b].size());
		std::vector<int> vecBrightness;
		vecBrightness.assign(BRIGHTNESS, 0);
		for (int i = 0; i < (int)pSeg1->m_vecBrightness[b].size(); i++)
		{
			vecBrightness[i] = pSeg1->m_vecBrightness[b][i] + pSeg2->m_vecBrightness[b][i];

			int v = vecBrightness[i];
			if (v > 0)
			{
				float p = (float)v / (float)(n1 + n2);
				fEntropy -=  (float)(p * (log(p) / log(2)));
			}
		}
	}
	float f = (n1 + n2) * fEntropy - n1 * pSeg1->m_Entropy - n2 * pSeg2->m_Entropy;
	f /= iBand;

	return f;
}

float StandardDeviation::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float standarddeviation = (n1 * pSeg1->m_StandardDeviation[i] * pSeg1->m_StandardDeviation[i] + n2 * pSeg2->m_StandardDeviation[i] * pSeg2->m_StandardDeviation[i]) / (float)(n1 + n2)
			+ n1 * n2 * (pSeg1->m_Avg[1] - pSeg2->m_Avg[2]) * (pSeg1->m_Avg[1] - pSeg2->m_Avg[2]) / (float)((n1 + n2) * (n1 + n2));
		ASSERT(standarddeviation > 0.0001);

		//standarddeviation = sqrt(standarddeviation);
		
		f += standarddeviation;
	}
	//float ed = sqrt(f);
	f /= (float)pSeg1->m_Avg.size();
	f = sqrt(f);
	return f;
}

float RSE::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float fRSE = n1 * pSeg1->m_StandardDeviation[i] * pSeg1->m_StandardDeviation[i] + n2 * pSeg2->m_StandardDeviation[i] * pSeg2->m_StandardDeviation[i]
		+ n1 * n2 * (pSeg1->m_StandardDeviation[i] - pSeg2->m_StandardDeviation[i]) * (pSeg1->m_StandardDeviation[i] - pSeg2->m_StandardDeviation[i]) / (float)(n1 + n2);
		f += fRSE;
	}

	f /= (float)pSeg1->m_Avg.size();
	f = sqrt(f);
	return f;
}

float RDV::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float fRDV =  n1 * n2 * (pSeg1->m_Avg[1] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[1] - pSeg2->m_Avg[i]) / (float)((n1 + n2) * (n1 + n2));
		f += fRDV;
	}

	f /= (float)pSeg1->m_Avg.size();
	f = sqrt(f);
	return f;
}

float RDSE::CalcCoefficient(Segment* pSeg1, Segment* pSeg2)
{
	ASSERT(pSeg1->m_Avg.size() == pSeg2->m_Avg.size());
	// to be continued...
	int n1 = pSeg1->m_area.size();
	int n2 = pSeg2->m_area.size();

	float f = 0.0f;
	for (int i = 0; i < (int)pSeg1->m_Avg.size(); i++)
	{
		float fRDSE =  n1 * n2 * (pSeg1->m_Avg[1] - pSeg2->m_Avg[i]) * (pSeg1->m_Avg[1] - pSeg2->m_Avg[i]) / (float)(n1 + n2);
		f += fRDSE;
	}

	f /= (float)pSeg1->m_Avg.size();
	f = sqrt(f);
	return f;
}