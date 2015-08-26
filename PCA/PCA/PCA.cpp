// PCA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImageReader.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string strFilePath = "E:\\Downloads\\s41.tif";
	//ImageReader image;
	//image.LoadFile(strFilePath);
	//image.CloseFile();

	cv::Mat mat = cv::imread(strFilePath);
	if (mat.empty() || !mat.data)
		return -1;
	if ( ! mat.isContinuous() )
		return -2;
	int iType = mat.type();
	cv::Mat M(mat.rows * mat.cols, mat.channels(), CV_32FC1);
	//cv::Mat setdata(mat.rows * mat.cols, mat.channels(), CV_32FC1);

	try
	{
		for (int b = 0; b < mat.channels(); b++)
		{
			for (int c = 0; c < mat.cols; c++)
			{
				for (int r = 0; r < mat.rows; r++)
				{
					float f = mat.at<unsigned short>(r, c, b);
				}
			}
		}

		//cv::Mat setdata = mat.reshape(1, 0).t();
	
	}
	catch (exception e)
	{
		cout << e.what();
	}

	//int ic = setdata.channels();
	try
	{
		cv::PCA pca(M, // pass the data
			cv::Mat(), // there is no pre-computed mean vector,
			// so let the PCA engine to compute it
			CV_PCA_DATA_AS_ROW // indicate that the vectors
			// are stored as matrix rows
			// (use CV_PCA_DATA_AS_COL if the vectors are
			// the matrix columns)
			);

		int row = pca.eigenvectors.rows;
		int col = pca.eigenvectors.cols;
		int channel = pca.eigenvectors.channels();

		for (int b = 0; b < channel; b++) 
		{
			for (int r = 0; r < row; r++)
			{
				for (int c = 0; c < col; c++)
				{
					cout << pca.eigenvectors.at<float>(r, c, b);
				}
			}
		}
	}
	catch (exception e)
	{
		cout << e.what();
	}
	return 0;
}

