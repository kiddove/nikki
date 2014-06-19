#pragma once
#include <vector>
#include <algorithm>

// only contain position informations


struct Pixel
{
public:
	Pixel()
	{
		row = 0;
		col = 0;
	}
	Pixel(int r, int c)
	{
		row = r;
		col = c;
	}
	int row;
	int col;

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
	// calc Euclidean distance
	void CalcEd();

	// store neighbour segment pointers

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

	::CRITICAL_SECTION cs;
};
