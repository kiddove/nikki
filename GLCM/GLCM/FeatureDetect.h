// FeatureDetect.h: interface for the FeatureDetect class. 
//
///////////////////////////////////////////////////////////////////////////////////////
/* Author: skyseraph/zhaobo   2011/4    zgzhaobo@gmal.com
*/
///////////////////////////////////////////////////////////////////////////////////////

#include <math.h>    
#include "windows.h"
#include "iostream"
using namespace std;

typedef struct glcmFeature 
{
	double    dCorrelation;        
	double    dEnergy;            
	double    dEntropy;            
	double    dInertiaQuadrature;    
	double    dLocalCalm;                
}glcmFeature;

typedef struct glcmFeatureVar 
{
	double    dAveCorrelation;    
	double    dAveEnergy;    
	double    dAveEntropy;    
	double    dAveInertiaQuadrature;
	double    dAveLocalCalm;    
	double    dVarCorrelation;    
	double    dVarEnergy;        
	double    dVarEntropy;        
	double    dVarInertiaQuadrature;
	double    dVarLocalCalm;        
}glcmFeatureVar;

#define MYTYPE unsigned short

class ZBGLCM
{
public:    
	ZBGLCM();    
	~ZBGLCM();

	// 计算共生矩阵
	void ComputeMatrix(MYTYPE **LocalImage, int LocalImageWidth);   
	// 计算纹理特征
	void ComputeFeature(double&FeatureEnergy, double&FeatureEntropy, 
		double&FeatureInertiaQuadrature, double&FeatureCorrelation, 
		double&FeatureLocalCalm, int** pMatrix, int dim);
	glcmFeature pGLCMF;
	glcmFeatureVar pGLCMFVar;
	// 计算GLCM特征
	glcmFeature GLCMFeature(MYTYPE* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth,int dir);  
	// // 计算特征均值和方差
	glcmFeatureVar GLCMFeatureVar(MYTYPE* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth);


public:    
	double FeatureLocalCalmRD;    
	double FeatureLocalCalmLD;    
	double FeatureLocalCalmV;    
	double FeatureLocalCalmH;
	double FeatureCorrelationRD;
	double FeatureCorrelationLD;
	double FeatureCorrelationV;    
	double FeatureCorrelationH;    
	double FeatureInertiaQuadratureRD;
	double FeatureInertiaQuadratureLD;
	double FeatureInertiaQuadratureV;
	double FeatureInertiaQuadratureH;
	double FeatureEntropyRD;
	double FeatureEntropyLD;
	double FeatureEntropyV;    
	double FeatureEntropyH;    
	double FeatureEnergyRD;    
	double FeatureEnergyLD;    
	double FeatureEnergyV;    
	double FeatureEnergyH;    

	int FilterWindowWidth;    
	int distance;    
	int GrayLayerNum;
	int L;                
	int** PMatrixRD;
	int** PMatrixLD;    
	int** PMatrixV;    
	int** PMatrixH;    

	int m_iNum;
	void SetNum(int iDepth);
};