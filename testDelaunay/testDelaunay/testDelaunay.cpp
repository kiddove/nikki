// testDelaunay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "ply.h"

using namespace std;
using namespace cv;

static void draw_subdiv_point (Mat& img, Point2f fp, Scalar color)
{
	//circle(img, fp, 3, color, FILLED, LINE_8, 0);
	circle(img, fp, 3, color, 1, 8, 0);
}

static void draw_subdiv(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color)
{
//#if 1
//	vector<Vec6f> triangleList;
//	subdiv.getTriangleList(triangleList);
//	vector<Point> pt(3);
//
//	for (size_t i = 0; i < triangleList.size(); i++)
//	{
//		Vec6f t = triangleList[i];
//		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
//		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
//		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
//
//		line(img, pt[0], pt[1], delaunay_color, 1, 8, 0);
//		line(img, pt[1], pt[2], delaunay_color, 1, 8, 0);
//		line(img, pt[2], pt[0], delaunay_color, 1, 8, 0);
//	}
//#else
	vector<Vec4f> edgeList;
	subdiv.getEdgeList(edgeList);

	for (size_t i = 0; i < edgeList.size(); i++)
	{
		Vec4f e = edgeList[i];
		Point pt0 = Point(cvRound(e[0]), cvRound(e[1]));
		Point pt1 = Point(cvRound(e[2]), cvRound(e[3]));
		line(img, pt0, pt1, delaunay_color, 1, 8, 0);
	}
//#endif
}

static void locate_point(Mat& img, Subdiv2D& subdiv, Point2f fp, Scalar active_color)
{
	int e0 = 0, vertex = 0;
	subdiv.locate(fp, e0, vertex);

	if (e0 > 0)
	{
		int e = e0;

		do
		{
			Point2f org, dst;
			if (subdiv.edgeOrg(e, &org) > 0 && subdiv.edgeDst(e, &dst) > 0)
				line(img, org, dst, active_color, 3, 8, 0);
			e = subdiv.getEdge(e, Subdiv2D::NEXT_AROUND_LEFT);
		}
		while (e != e0);
	}

	draw_subdiv_point(img, fp, active_color);
}

static void paint_voronoi(Mat& img, Subdiv2D& subdiv)
{
	vector<vector<Point2f>> facets;
	vector<Point2f> centers;
	subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

	vector<Point> ifacet;
	vector<vector<Point>> ifacets(1);

	for (size_t i = 0; i < facets.size(); i++)
	{
		ifacet.resize(facets[i].size());
		for (size_t j = 0; j < facets[i].size(); j++)
			ifacet[j] = facets[i][j];

		Scalar color;
		color[0] = rand() & 255;
		color[1] = rand() & 255;
		color[2] = rand() & 255;

		fillConvexPoly(img, ifacet, color, 8, 0);

		ifacets[0] = ifacet;
		polylines(img, ifacets, true, Scalar(), 1, 8, 0);
		circle(img, centers[i], 3, Scalar(), 1, 8, 0);
	}
}

void WriteToPLY(Subdiv2D& subdiv, char* pFileName)
{
	//// vertices & faces
	//PlyFile *ply;
	//float fVersion = 1.0f;
	//int num_of_vertices, num_of_faces;

	//// define 2 struct for vertex and face
	////char szFile[256];
	////ChooseFile(szFile, 256, true);
	////if (strlen(szFile) < 1)
	////	return;
	//char szFileName[256] = {0};
	//// pFileName may be the original input filename
	//char* p = strtok(pFileName, ".");
	//if (p == NULL)
	//{
	//	memcpy(szFileName, pFileName, strlen(pFileName));
	//}
	//else
	//{
	//	memcpy(szFileName, p, strlen(p));
	//}
	//struct Vertex
	//{
	//	float x;
	//	float y;
	//	float z;
	//	bool operator == (const Vertex& v) const
	//	{
	//		if (v.x < x || v.x > x)
	//			return false;
	//		else
	//		{
	//			if (v.y < y || v.y > y)
	//				return false;
	//			else
	//			{
	//				if (v.z < z || v.z > z)
	//					return false;
	//			}
	//			return true;
	//		}
	//		return true;
	//	}
	//};

	//struct Face
	//{
	//	unsigned char num_vertices;	// number of vertices indices in list
	//	int *vertices;				// vertex index list;
	//};

	//char *elem_names[] = { /* list of the kinds of elements in the user's object */
	//	"vertex", "face"
	//};

	//PlyProperty vert_props[] = { /* list of property information for a vertex */
	//	{"x", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,x), PLY_SCALAR, 0, 0, 0},
	//	{"y", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,y), PLY_SCALAR, 0, 0, 0},
	//	{"z", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex,z), PLY_SCALAR, 0, 0, 0},
	//};

	//PlyProperty face_props[] = { /* list of property information for a vertex */
	//	{"vertex_indices", PLY_INT, PLY_INT, offsetof(Face,vertices),
	//	PLY_LIST, PLY_UCHAR, PLY_UCHAR, offsetof(Face,num_vertices)},
	//};

	////num_of_faces = dt.number_of_faces();
	////num_of_vertices = dt.number_of_vertices();

	//num_of_faces = subdiv.getVertex;
	//num_of_vertices = dt.number_of_vertices();

	//ply = ply_open_for_writing(szFileName, 2, elem_names, PLY_ASCII, &fVersion);

	//ply_element_count(ply, "vertex", num_of_vertices);
	//ply_describe_property(ply, "vertex", &vert_props[0]);
	//ply_describe_property(ply, "vertex", &vert_props[1]);
	//ply_describe_property(ply, "vertex", &vert_props[2]);

	//ply_element_count(ply, "face", num_of_faces);
	//ply_describe_property(ply, "face", &face_props[0]);

	//ply_put_comment(ply, "this is a test");
	//ply_put_obj_info(ply, "generate from terrain()");

	//ply_header_complete(ply);

	//ply_put_element_setup(ply, "vertex");
	//
	//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	//typedef CGAL::Triangle_3<K> Triangle;
	//typedef K::Point_3   Point;

	//std::vector<Vertex> vecVertices;
	//for (Delaunay::Finite_vertices_iterator iter = dt.finite_vertices_begin(); iter != dt.finite_vertices_end(); iter++)
	//{
	//	Point pt = iter->point();
	//	//printf("%.3f, %.3f, %.3f", pt[0], pt[1], pt[2]);
	//	Vertex v;
	//	v.x = pt[0];
	//	v.y = pt[1];
	//	v.z = pt[2];
	//	// for find the index
	//	vecVertices.push_back(v);
	//	ply_put_element(ply, (void*)&v);
	//}

	//ply_put_element_setup(ply, "face");
	//for (Delaunay::Finite_faces_iterator iter = dt.finite_faces_begin(); iter != dt.finite_faces_end(); iter++)
	//{
	//	Triangle tr = dt.triangle(iter);
	//	Point pt1 = dt.triangle(iter).vertex(0);
	//	Point pt2 = dt.triangle(iter).vertex(1);
	//	Point pt3 = dt.triangle(iter).vertex(2);
	//	//printf("%.3f, %.3f, %.3f\n", pt1[0], pt1[1], pt1[2]);
	//	//printf("%.3f, %.3f, %.3f\n", pt2[0], pt2[1], pt2[2]);
	//	//printf("%.3f, %.3f, %.3f\n", pt3[0], pt3[1], pt3[2]);
	//	Vertex v1, v2, v3;
	//	v1.x = pt1[0];
	//	v1.y = pt1[1];
	//	v1.z = pt1[2];

	//	v2.x = pt2[0];
	//	v2.y = pt2[1];
	//	v2.z = pt2[2];

	//	v3.x = pt3[0];
	//	v3.y = pt3[1];
	//	v3.z = pt3[2];

	//	int index1, index2, index3;
	//	index1 = index2 = index3 = -1;
	//	std::vector<Vertex>::iterator iterFind = std::find(vecVertices.begin(), vecVertices.end(), v1);
	//	if (iterFind != vecVertices.end())
	//	{
	//		index1 = std::distance(vecVertices.begin(), iterFind);
	//		//printf("index : %d\n", ipos);
	//	}
	//	iterFind = std::find(vecVertices.begin(), vecVertices.end(), v2);
	//	if (iterFind != vecVertices.end())
	//	{
	//		index2 = std::distance(vecVertices.begin(), iterFind);
	//		//printf("index : %d\n", ipos);
	//	}
	//	iterFind = std::find(vecVertices.begin(), vecVertices.end(), v3);
	//	if (iterFind != vecVertices.end())
	//	{
	//		index3 = std::distance(vecVertices.begin(), iterFind);
	//		//printf("index : %d\n", ipos);
	//	}

	//	Face f;
	//	int Vertex_Indices[3];
	//	f.num_vertices = 3;
	//	for (int i = 0; i < 3; i++)
	//	{
	//		Vertex_Indices[0] = index1;
	//		Vertex_Indices[1] = index2;
	//		Vertex_Indices[2] = index3;
	//		
	//		f.vertices = Vertex_Indices;
	//	}
	//	ply_put_element(ply, (void*)&f);
	//}
	//ply_close (ply);
}

int _tmain(int argc, _TCHAR* argv[])
{
	Scalar active_facet_color(0, 0, 255), delaunay_color(255, 255, 255);
	//Rect rect(0, 0, 600, 600);
	ifstream is;
	is.open("I:\\OpenSource\\Code\\sdf\\sdfRead\\Temp\\Mixed_test_dense+las.txt", ifstream::out);
	std::vector<Point2f> vecPt;
	char* pData = new char[512];
	float fx = 0.0, fy = 0.0, mx = 1e8, my = 1e8;
	while(!is.eof())
	{
		//int ilen = is.tellg();
		
		memset(pData, 0, 512);
		is.getline(pData, 511);
		char *token = strtok(pData, "\t, ");
		Point2f pt;
		token = strtok(NULL, "\t, ");
		//printf("%.3f, ", atof(token));
		
		pt.x = atof(token);
		token = strtok(NULL, "\t, ");
		//printf("%.3f\n", atof(token));
		pt.y = atof(token);
		vecPt.push_back(pt);

		if (pt.x > fx)
			fx = pt.x;
		if (pt.y > fy)
			fy = pt.y;

		if (pt.x < mx)
			mx = pt.x;
		if (pt.y < my)
			my = pt.y;
	}

	int max_x = (int)fx + 1;
	int max_y = (int)fy + 1;
	int min_x = (int)mx - 1;
	int min_y = (int)my - 1;
	delete pData;
	//Subdiv2D subdiv(rect);
	//Mat img(rect.size(), CV_8UC3);

	//img = Scalar::all(0);
	//std::string win = "Delaunay Demo";
	//imshow(win, img);
	int row = max_x - min_x;
	int col = max_y - min_y;

	Rect rect(0, 0, row, col);
	Subdiv2D subdiv(rect);
	subdiv.initDelaunay(rect);
	Mat img(rect.size(), CV_8UC3);

	for (int i = 0; i < vecPt.size(); i++)
	{
		Point2f pt;
		pt.x = vecPt[i].x - min_x;
		pt.y = vecPt[i].y - min_y;
		subdiv.insert(pt);
	}
	//img = Scalar::all(0);
	//std::string win = "Delaunay Demo";
	//paint_voronoi(img, subdiv);
	//imshow(win, img);
	//waitKey(0);

	// write to ply
	// vertex and facets
	WriteToPLY(subdiv, "I://downloads//result_delaunay.ply");
	return 0;









	//Mat img = imread("I:\\download\\AverageMaleFace.jpg", -1);
	//Rect rect(0, 0, img.cols, img.rows);
	//Subdiv2D subdiv(rect);
	//subdiv.initDelaunay(rect);
	//draw_subdiv(img, subdiv, delaunay_color);
	//std::string win = "Delaunay Demo";
	//imshow(win, img);
	//waitKey(0);

	//for (int i = 0; i < 200; i++)
	//{
	//	Point2f fp((float)(rand()%(rect.width - 10) + 5), (float)(rand()%(rect.height - 10) + 5));

	//	locate_point(img, subdiv, fp, active_facet_color);
	//	imshow(win, img);

	//	//if (waitKey(1000) >= 0)
	//	//	break;

	//	subdiv.insert(fp);

	//	//img = Scalar::all(0);
	//	draw_subdiv(img, subdiv, delaunay_color);
	//	imshow(win, img);

	//	if (waitKey(100) >= 0 )
	//		break;
	//}

	//img = Scalar::all(0);
	//paint_voronoi(img, subdiv);
	//imshow(win, img);
	//waitKey(0);
	//return 0;
}

