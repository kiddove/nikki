#pragma once
#include "afxmt.h"
#include <vector>
#include <algorithm>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;
// only contain position informations
class Segment;

enum CalcType
{
	None = 0,
	Euclidean_Distance,
	Old_BSMSE,
	New_BSMSE,
	Old_BSMSE_New_Equation,
	New_BSMSE_New_Equation,
	Similarity_Measure,
	Standard_Deviation,
	Spectral_Angle_Mapper,
	Entropy_Measure,
	Root_Squared_Error,
	Rooted_Difference_in_Variance,
	Rooted_Difference_in_Squared_Error
};

enum NeigbourType
{
	None_Neighbour = 0,
	Four_Neighbour,
	Eight_Neighbour
};

//struct Pixel
//{
//public:
//	Pixel()
//	{
//		row = 0;
//		col = 0;
//	}
//	Pixel(short r, short c)
//	{
//		row = r;
//		col = c;
//	}
//	short row;
//	short col;
//
//	bool operator==(const Pixel& obj) const
//	{
//		return row == obj.row && col == obj.col;
//	}
//
//	//bool operator < (const Pixel& p) const
//	//{
//	//	if (row < p.row)
//	//	{
//	//		return true;
//	//	}
//	//	else if (row == p.row)
//	//	{
//	//		if (col < p.col)
//	//			return true;
//	//	}
//
//	//	return false;
//	//}
//};

struct segCondition
{
	CalcType calcType;
	CalcType pre_calcType;
	float threshold_pre;
	//float threshold_BSMSE;
	float threshold;
	//float shapefactor;
	int min_size;
	NeigbourType neibType;
	//float alpha;
	//float beta;
	int iteration;
	bool scale;

public:
	segCondition()
	{
		calcType = None;
		threshold_pre = 0.0;
		threshold = 0.0;
		neibType = None_Neighbour;
		min_size = 1;
		//alpha = 0.0;
		//beta = 0.0;
		iteration = 0;
		scale = false;
	}
};

class Segment
{
public:
	Segment(void);
	virtual ~Segment(void);

	Segment(int r, int c)
	{
		if (r >= 0 && c >= 0)
		{
			m_area.clear();
			Add(r, c);
		}
		m_bValid = true;
		m_Entropy = 0.0;
	}

	void Add(int r, int c)
	{
		//Pixel p(r, c);

		cv::Point p;
		p.x = c;
		p.y = r;
		m_area.push_back(p);
	}

	void Clear()
	{
		m_iIndex = -1;
		//min_row = min_col = max_row = max_col = 0;
		m_neighbours.clear();
		m_area.clear();
		m_Avg.clear();
		//m_Variance.clear();
	}
	
	std::vector<int> m_neighbours;
	// include pixels 
	//std::vector<Pixel> m_area;
	std::vector<cv::Point> m_area;

	// index in all segments
	int m_iIndex;

	// segment avg 
	// mean value of each band
	std::vector<float> m_Avg;
	// currently not using 
	//std::vector<float> m_Variance;
	// stand deviation of each band
	std::vector<float> m_StandardDeviation;

	// for ENT
	std::vector<std::vector<int>> m_vecBrightness;
	float m_Entropy;
	//float m_maxDistance;
	//std::vector<cv::Point> m_Boundary;
	//float m_Compactness;
	//float m_Smoothness;

	//int min_row;
	//int max_row;
	//int min_col;
	//int max_col;

	//bool m_bBoundary;
	bool m_bValid;
};
