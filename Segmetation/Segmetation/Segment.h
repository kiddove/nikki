#pragma once
#include "afxmt.h"
#include <vector>
#include <algorithm>

// only contain position informations
class Segment;

struct Neighbour
{
	Segment* pSeg;
	int iIndex;

	// for find
	bool operator==(const Neighbour& n) const
	{
		return iIndex == n.iIndex && pSeg == n.pSeg;
	}
	//// for sort
	//bool operator < (const Neighbour& n) const
	//{
	//	return iIndex < n.iIndex;
	//}
};

struct Pixel
{
public:
	Pixel()
	{
		row = 0;
		col = 0;
	}
	Pixel(short r, short c)
	{
		row = r;
		col = c;
	}
	short row;
	short col;

	bool operator==(const Pixel& obj) const
	{
		return row == obj.row && col == obj.col;
	}

	//bool operator < (const Pixel& p) const
	//{
	//	if (row < p.row)
	//	{
	//		return true;
	//	}
	//	else if (row == p.row)
	//	{
	//		if (col < p.col)
	//			return true;
	//	}

	//	return false;
	//}
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
	}

	void Add(int r, int c)
	{
		Pixel p(r, c);
		m_area.push_back(p);
	}

	void Clear()
	{
		m_iIndex = -1;
		min_row = min_col = max_row = max_col = 0;
		m_neighbours.clear();
		m_area.clear();
		m_Avg.clear();
		m_Variance.clear();
	}
	
	// lock / unlock neighbours
	void LockNeighbours();
	void UnlockNeighbours();

	// store neighbour segment pointers

	//std::vector<Segment*> m_neighbours;
	//std::vector<Neighbour> m_neighbours;
	std::vector<int> m_neighbours;
	// include pixels 
	std::vector<Pixel> m_area;

	// index in all segments
	int m_iIndex;

	// segment avg 
	std::vector<float> m_Avg;
	std::vector<float> m_Variance;
	//float m_fAvg;
	//float m_fVariance;

	int min_row;
	int max_row;
	int min_col;
	int max_col;

	//bool m_bBoundary;
	bool m_bValid;
};
