#include <fstream>
#include "ImageReader.h"


using namespace std;
using namespace cv;


//void redirectOutput()
//{
//	//std::ifstream in("I:\\in.txt");
//	//std::streambuf *cinbuf = std::cin.rdbuf();
//	//std::cin.rdbuf(in.rdbuf());
//
//	std::ofstream out("I:\\out.txt");
//	std::streambuf *coutbuf = std::cout.rdbuf();
//	std::cout.rdbuf(out.rdbuf());
//}

void outputMatrix(string name, cv::Mat& mat)
{
	// redirect cout to file
	string filename = name + ".txt";
	std::ofstream out(filename);
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());

	cout << "Mat(" << name << ") = "<< endl << " "  << mat << endl << endl;

	//reset cout to console
	std::cout.rdbuf(coutbuf);
}
void main()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/*// redirect cout to file
	std::ofstream out("out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());*/
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// no no no opencv may lost channels when reading this kind of file
	//Mat image = imread("I:\\Download\\s41.tif", IMREAD_COLOR);

	ImageReader image;
	image.LoadFile("I:\\Download\\s41.tif");
	image.CloseFile();

	int cn = image.m_data.channels();

	// first way, try reshape, reshape the Mat into (r = r*c, c = channels, channel=1), then do PCA
	Mat ref = image.m_data.reshape(1, image.m_data.rows * image.m_data.cols);

	Mat tobeDone = ref.clone();
	cn = tobeDone.channels();
	
	//cout << "M = "<< endl << " "  << tobeDone << endl << endl;

	try
	{
		PCA pca_analysis(tobeDone, Mat(), CV_PCA_DATA_AS_ROW);

		outputMatrix("eigenvectors", pca_analysis.eigenvectors);
		outputMatrix("eigenvalues", pca_analysis.eigenvalues);
	}
	catch (exception e)
	{
		cout << e.what()<< endl;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////
	/*// to test the result
	// do it in another way, separate each channel into different Mat, put into a vector, then do PCA
	std::vector<Mat> splitData;
	split(image, splitData);

	// now each channel should store in different Mat

	for (std::vector<Mat>::iterator iter = splitData.begin(); iter != splitData.end(); iter++) {
		Mat ref = iter->reshape(1, iter->rows * iter->cols);
		cn = ref.channels();
		cn = ref.rows;
		cn = ref.cols;
	}*/
	/////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////////
	/*//reset cout to console
	std::cout.rdbuf(coutbuf);*/
	/////////////////////////////////////////////////////////////////////////////////////////////////
}