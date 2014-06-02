//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FeatureDetect.h"



#include "winnt.h"
ZBGLCM::ZBGLCM()
{    
	PMatrixRD = NULL;    
	PMatrixLD = NULL;
	PMatrixV = NULL;
	PMatrixH = NULL;    

	distance =1;
	FilterWindowWidth =51;
	GrayLayerNum =32; 
	L=8;

	int i;
	PMatrixH =new int*[GrayLayerNum];
	PMatrixLD=new int*[GrayLayerNum];
	PMatrixRD=new int*[GrayLayerNum];
	PMatrixV =new int*[GrayLayerNum];

	for(i=0; i<GrayLayerNum; i++)
	{
		PMatrixH[i] =new int[GrayLayerNum];
		PMatrixLD[i]=new int[GrayLayerNum];
		PMatrixRD[i]=new int[GrayLayerNum];
		PMatrixV[i] =new int[GrayLayerNum];        
	}
}

ZBGLCM::~ZBGLCM()
{
	if(PMatrixH !=NULL)
	{        
		for(int i=0; i<GrayLayerNum; i++)
		{
			delete [] PMatrixH[i];
			PMatrixH[i] = NULL; //已析构了，后不再加
		}
		delete [] PMatrixH;
		PMatrixH = NULL;
	}
	if(PMatrixLD !=NULL)
	{
		for(int i=0; i<GrayLayerNum; i++)
		{
			delete[] PMatrixLD[i];
		}
		delete [] PMatrixLD;
	}
	if(PMatrixRD !=NULL)
	{
		for(int i=0; i<GrayLayerNum; i++)
			delete [] PMatrixRD[i];
		delete [] PMatrixRD;
	}
	if(PMatrixV !=NULL)
	{
		for(int i=0; i<GrayLayerNum; i++)
			delete [] PMatrixV[i];
		delete [] PMatrixV;
	}    
}


////////////////////////////////////////////////////
// 计算纹理特征
void ZBGLCM::ComputeFeature(double&FeatureEnergy, double&FeatureEntropy, 
							double&FeatureInertiaQuadrature, double&FeatureCorrelation, 
							double&FeatureLocalCalm, int** pMatrix, int dim)
{
	int i,j;
	double**pdMatrix;
	pdMatrix =new double*[dim];
	for(i=0; i<dim; i++)
		pdMatrix[i] =new double[dim];

	int total =0;
	for(i=0; i<dim; i++)
	{
		for(j=0; j<dim; j++)
		{
			total += pMatrix[i][j];
		}
	}
	for(i=0; i<dim; i++)
	{
		for(j=0; j<dim; j++)
		{
			pdMatrix[i][j] = (double)pMatrix[i][j]/(double)total;
		}
	}

	FeatureEnergy =0.0;
	FeatureEntropy =0.0;
	FeatureInertiaQuadrature =0.0;
	FeatureLocalCalm =0.0;

	for(i=0; i<dim; i++)
	{
		for(j=0; j<dim; j++)
		{
			FeatureEnergy += pdMatrix[i][j]*pdMatrix[i][j];            
			if(pdMatrix[i][j]>1e-12)
			{
				FeatureEntropy -= pdMatrix[i][j]*log(pdMatrix[i][j]);
			}            
			FeatureInertiaQuadrature += (double)(i-j)*(double)(i-j)*pdMatrix[i][j];
			FeatureLocalCalm += pdMatrix[i][j]/(1+(double)(i-j)*(double)(i-j));
		}
	}

	double ux =0.0;
	double localtotal =0.0;
	for(i=0; i<dim; i++)
	{
		localtotal =0.0;
		for(j=0; j<dim; j++)
		{
			localtotal += pdMatrix[i][j];
		}
		ux += (double)i * localtotal;
	}

	double uy =0.0;
	for(j=0; j<dim; j++)
	{
		localtotal =0.0;
		for(i=0; i<dim; i++)
		{
			localtotal += pdMatrix[i][j];
		}
		uy += (double)j * localtotal;
	}

	double sigmax =0.0;
	for(i=0; i<dim; i++)
	{
		localtotal =0.0;
		for(j=0; j<dim; j++)
		{
			localtotal += pdMatrix[i][j];
		}
		sigmax += (double)(i-ux) * (double)(i-ux) * localtotal;
	}

	double sigmay =0.0;
	for(j=0; j<dim; j++)
	{
		localtotal =0.0;
		for(i=0; i<dim; i++)
		{
			localtotal += pdMatrix[i][j];
		}
		sigmay += (double)(j-uy) * (double)(j-uy) * localtotal;
	}

	FeatureCorrelation =0.0;
	for(i=0; i<dim; i++)
	{
		for(j=0; j<dim; j++)
		{
			FeatureCorrelation += (double)(i-ux) * (double)(j-uy) * pdMatrix[i][j];
		}
	}
	if(sigmax !=0&& sigmay !=0)
	{
		FeatureCorrelation /= sigmax;
		FeatureCorrelation /= sigmay;
	}
	else
		FeatureCorrelation =8;

	if(pdMatrix !=NULL)
	{
		for(i=0; i<dim; i++)
		{
			delete [] pdMatrix[i];
			pdMatrix[i] = NULL;
		}
		delete [] pdMatrix;
		pdMatrix = NULL;
	}
}
/////////////////////////////////////////////////////
// 计算共生矩阵
void ZBGLCM::ComputeMatrix(MYTYPE **LocalImage, int LocalImageWidth)
{
	int i,j;

	MYTYPE **NewImage;
	NewImage =new MYTYPE*[LocalImageWidth];
	if(NewImage==NULL)
		return;
	for(i=0; i<LocalImageWidth; i++)
	{
		NewImage[i] =new MYTYPE[LocalImageWidth];
		if(NewImage[i]==NULL)
			return;
	}


	for(i=0; i<LocalImageWidth; i++)
	{
		for(j=0; j<LocalImageWidth; j++)
		{            
			NewImage[i][j] = LocalImage[i][j] / (m_iNum/GrayLayerNum);
		}
	}

	for(i=0; i<GrayLayerNum; i++)
	{        
		for(j=0; j<GrayLayerNum; j++)
		{
			PMatrixH[i][j]  =0;
			PMatrixLD[i][j] =0;
			PMatrixRD[i][j] =0;
			PMatrixV[i][j]  =0;
		}
	}

	for(i=0; i<LocalImageWidth; i++)
	{
		for(j=0; j<LocalImageWidth-distance; j++)
		{
			PMatrixH[(unsigned int)NewImage[i][j]][(unsigned int)NewImage[i][j+distance]] +=1;
			PMatrixH[(unsigned int)NewImage[i][j+distance]][(unsigned int)NewImage[i][j]] +=1;
		}
	}

	for(i=0; i<LocalImageWidth-distance; i++)
	{
		for(j=0; j<LocalImageWidth; j++)
		{
			PMatrixV[(unsigned int)NewImage[i][j]][(unsigned int)NewImage[i+distance][j]] +=1;
			PMatrixV[(unsigned int)NewImage[i+distance][j]][(unsigned int)NewImage[i][j]] +=1;
		}
	}

	for(i=0; i<LocalImageWidth-distance; i++)
	{
		for(j=0; j<LocalImageWidth-distance; j++)
		{
			int newi, newj;
			newi = i+distance;
			newj = j+distance;
			PMatrixLD[(unsigned int)NewImage[i][j]][(unsigned int)NewImage[newi][newj]] +=1;
			PMatrixLD[(unsigned int)NewImage[newi][newj]][(unsigned int)NewImage[i][j]] +=1;
		}
	}

	for(i=distance; i<LocalImageWidth; i++)
	{
		for(j=0; j<LocalImageWidth-distance; j++)
		{
			int newi, newj;
			newi = i-distance;
			newj = j+distance;
			PMatrixRD[(unsigned int)NewImage[i][j]][(unsigned int)NewImage[newi][newj]] +=1;
			PMatrixRD[(unsigned int)NewImage[newi][newj]][(unsigned int)NewImage[i][j]] +=1;
		}
	}

	if(NewImage !=NULL)
	{
		for(i=0; i<LocalImageWidth; i++)
		{
			delete [] NewImage[i];
			NewImage[i] = NULL;
		}
		delete [] NewImage;
		NewImage = NULL;
	}
}
///////////////////////////////////////////////////////////
// 计算GLCM特征
glcmFeature ZBGLCM::GLCMFeature(MYTYPE* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth,int dir)
{
	//ASSERT(ImageHeight>FilterWindowWidth && ImageWidth > FilterWindowWidth);

	double dEnergy              =0.0;
	double dEntropy              =0.0;
	double dInertiaQuadrature =0.0;
	double dLocalCalm          =0.0;
	double dCorrelation          =0.0;
	double dEnergy1              =0.0;
	double dEntropy1          =0.0;
	double dInertiaQuadrature1=0.0;
	double dLocalCalm1          =0.0;
	double dCorrelation1      =0.0;    

	int rolltimeH = ImageHeight/FilterWindowWidth;
	int rolltimeW = ImageWidth /FilterWindowWidth;
	int i,j;
	int p,q;

	MYTYPE** arLocalImage;    
	arLocalImage=(MYTYPE**)calloc((unsigned)FilterWindowWidth,sizeof(MYTYPE*));
	for( i=0;i<FilterWindowWidth;i++) 
	{
		arLocalImage[i]=(MYTYPE*)calloc((unsigned)FilterWindowWidth,sizeof(MYTYPE));
	}

	for(i=0; i< rolltimeH; i++)
	{
		for(j=0; j<rolltimeW; j++)
		{
			for(p=0; p<FilterWindowWidth; p++)
			{
				for(q=0; q<FilterWindowWidth; q++)
				{
					//arLocalImage[p][q] =*((MYTYPE*)ImageArray+(ImageHeight-1-(i*FilterWindowWidth+p))*ImageWidth+j*FilterWindowWidth+q);
					arLocalImage[p][q] =*(ImageArray + (i*FilterWindowWidth+p + j*FilterWindowWidth+q));
				}
			}
			ComputeMatrix(arLocalImage, FilterWindowWidth);
			switch (dir)
			{
			case 0:
				ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixH, GrayLayerNum);
				break;
			case 1:
				ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixRD, GrayLayerNum);
				break;
			case 2:
				ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixV, GrayLayerNum);
				break;
			case 3:
				ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixLD, GrayLayerNum);
				break;
			default:
				ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixH, GrayLayerNum);
				break;
			}      

			CString str;
			str.Format(_T("[%d, %d] --- 能量：%.3f, 熵：%.3f, 对比度：%.3f, 相关：%.3f, 逆差距：%.3f\n"), i, j, dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1);
			::OutputDebugString(str);
			dEnergy              += dEnergy1;
			dEntropy             += dEntropy1;
			dInertiaQuadrature   += dInertiaQuadrature1;
			dCorrelation         += dCorrelation1;
			dLocalCalm           += dLocalCalm1;        

			cout<<"相关："<< dCorrelation1 << ", "<<"能量："<<dEnergy1<<", "<<"熵："<<dEntropy1<<", "<<"对比度："<<dInertiaQuadrature1<<", "<<"逆差距："<<dLocalCalm1<<endl;
		}
	}
	dEnergy              /= (rolltimeH*rolltimeW);
	dEntropy             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature   /= (rolltimeH*rolltimeW);
	dCorrelation         /= (rolltimeH*rolltimeW);
	dLocalCalm           /= (rolltimeH*rolltimeW);

	pGLCMF.dEnergy = dEnergy ;
	pGLCMF.dEntropy = dEntropy;
	pGLCMF.dInertiaQuadrature = dInertiaQuadrature;
	pGLCMF.dCorrelation = dCorrelation;
	pGLCMF.dLocalCalm = dLocalCalm;

	for(i=0; i<FilterWindowWidth; i++)
	{
		free(arLocalImage[i]) ;
		arLocalImage[i] = NULL;
	}
	free(arLocalImage);
	arLocalImage = NULL;


	return pGLCMF;
}
//////////////////////////////////////////
// 计算特征均值和方差
glcmFeatureVar ZBGLCM::GLCMFeatureVar(MYTYPE* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth)
{
	//assert(ImageHeight>FilterWindowWidth && ImageWidth > FilterWindowWidth);

	double dEnergy              =0.0;
	double dEntropy              =0.0;
	double dInertiaQuadrature =0.0;
	double dLocalCalm          =0.0;
	double dCorrelation          =0.0;

	double dEnergy1              =0.0;
	double dEntropy1          =0.0;
	double dInertiaQuadrature1=0.0;
	double dLocalCalm1          =0.0;
	double dCorrelation1      =0.0;

	double dEnergy2              =0.0;
	double dEntropy2          =0.0;
	double dInertiaQuadrature2=0.0;
	double dLocalCalm2          =0.0;
	double dCorrelation2      =0.0;

	double dEnergy3              =0.0;
	double dEntropy3          =0.0;
	double dInertiaQuadrature3=0.0;
	double dLocalCalm3          =0.0;
	double dCorrelation3      =0.0;    

	double dEnergy4              =0.0;
	double dEntropy4          =0.0;
	double dInertiaQuadrature4=0.0;
	double dLocalCalm4          =0.0;
	double dCorrelation4      =0.0;

	double dEnergy11              =0.0;
	double dEntropy11          =0.0;
	double dInertiaQuadrature11=0.0;
	double dLocalCalm11          =0.0;
	double dCorrelation11      =0.0;

	double dEnergy22              =0.0;
	double dEntropy22          =0.0;
	double dInertiaQuadrature22=0.0;
	double dLocalCalm22          =0.0;
	double dCorrelation22      =0.0;

	double dEnergy33              =0.0;
	double dEntropy33          =0.0;
	double dInertiaQuadrature33=0.0;
	double dLocalCalm33          =0.0;
	double dCorrelation33      =0.0;

	double dEnergy44              =0.0;
	double dEntropy44          =0.0;
	double dInertiaQuadrature44=0.0;
	double dLocalCalm44          =0.0;
	double dCorrelation44      =0.0;

	int rolltimeH = ImageHeight/FilterWindowWidth;
	int rolltimeW = ImageWidth /FilterWindowWidth;
	int i,j;
	int p,q;

	MYTYPE** arLocalImage;    
	arLocalImage=(MYTYPE**)calloc((unsigned)FilterWindowWidth,sizeof(MYTYPE*));
	for( i=0;i<FilterWindowWidth;i++) 
	{
		arLocalImage[i]=(MYTYPE*)calloc((unsigned)FilterWindowWidth,sizeof(MYTYPE));
	}

	for(i=0; i< rolltimeH; i++)
	{
		for(j=0; j<rolltimeW; j++)
		{
			for(p=0; p<FilterWindowWidth; p++)
			{
				for(q=0; q<FilterWindowWidth; q++)
				{
					arLocalImage[p][q] =*((char*)ImageArray+(ImageHeight-1-(i*FilterWindowWidth+p))*ImageWidth+j*FilterWindowWidth+q);
				}
			}            
			ComputeMatrix(arLocalImage, FilterWindowWidth);
			ComputeFeature(dEnergy1, dEntropy1, dInertiaQuadrature1, dCorrelation1, dLocalCalm1, PMatrixH, GrayLayerNum);
			dEnergy              += dEnergy1;
			dEntropy             += dEntropy1;
			dInertiaQuadrature   += dInertiaQuadrature1;
			dCorrelation         += dCorrelation1;
			dLocalCalm           += dLocalCalm1;
			dEnergy11              += dEnergy1;
			dEntropy11             += dEntropy1;
			dInertiaQuadrature11   += dInertiaQuadrature1;
			dCorrelation11         += dCorrelation1;
			dLocalCalm11           += dLocalCalm1;
			ComputeMatrix(arLocalImage, FilterWindowWidth);
			ComputeFeature(dEnergy2, dEntropy2, dInertiaQuadrature2, dCorrelation2, dLocalCalm2, PMatrixRD, GrayLayerNum);
			dEnergy              += dEnergy2;
			dEntropy             += dEntropy2;
			dInertiaQuadrature   += dInertiaQuadrature2;
			dCorrelation         += dCorrelation2;
			dLocalCalm           += dLocalCalm2;
			dEnergy22              += dEnergy2;
			dEntropy22             += dEntropy2;
			dInertiaQuadrature22   += dInertiaQuadrature2;
			dCorrelation22         += dCorrelation2;
			dLocalCalm22           += dLocalCalm2;
			ComputeMatrix(arLocalImage, FilterWindowWidth);
			ComputeFeature(dEnergy3, dEntropy3, dInertiaQuadrature3, dCorrelation3, dLocalCalm3, PMatrixV, GrayLayerNum);
			dEnergy              += dEnergy3;
			dEntropy             += dEntropy3;
			dInertiaQuadrature   += dInertiaQuadrature3;
			dCorrelation         += dCorrelation3;
			dLocalCalm           += dLocalCalm3;
			dEnergy33              += dEnergy3;
			dEntropy33             += dEntropy3;
			dInertiaQuadrature33   += dInertiaQuadrature3;
			dCorrelation33         += dCorrelation3;
			dLocalCalm33           += dLocalCalm3;
			ComputeMatrix(arLocalImage, FilterWindowWidth);
			ComputeFeature(dEnergy4, dEntropy4, dInertiaQuadrature4, dCorrelation4, dLocalCalm4, PMatrixLD, GrayLayerNum);
			dEnergy              += dEnergy4;
			dEntropy             += dEntropy4;
			dInertiaQuadrature   += dInertiaQuadrature4;
			dCorrelation         += dCorrelation4;
			dLocalCalm           += dLocalCalm4;
			dEnergy44              += dEnergy4;
			dEntropy44             += dEntropy4;
			dInertiaQuadrature44   += dInertiaQuadrature4;
			dCorrelation44         += dCorrelation4;
			dLocalCalm44           += dLocalCalm4;        

		}
	}
	dEnergy              /= (rolltimeH*rolltimeW);
	dEntropy             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature   /= (rolltimeH*rolltimeW);
	dCorrelation         /= (rolltimeH*rolltimeW);
	dLocalCalm           /= (rolltimeH*rolltimeW);

	dEnergy11              /= (rolltimeH*rolltimeW);
	dEntropy11             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature11   /= (rolltimeH*rolltimeW);
	dCorrelation11         /= (rolltimeH*rolltimeW);
	dLocalCalm11           /= (rolltimeH*rolltimeW);

	dEnergy22              /= (rolltimeH*rolltimeW);
	dEntropy22             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature22   /= (rolltimeH*rolltimeW);
	dCorrelation22         /= (rolltimeH*rolltimeW);
	dLocalCalm22           /= (rolltimeH*rolltimeW);

	dEnergy33              /= (rolltimeH*rolltimeW);
	dEntropy33             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature33   /= (rolltimeH*rolltimeW);
	dCorrelation33         /= (rolltimeH*rolltimeW);
	dLocalCalm33           /= (rolltimeH*rolltimeW);

	dEnergy44              /= (rolltimeH*rolltimeW);
	dEntropy44             /= (rolltimeH*rolltimeW);
	dInertiaQuadrature44   /= (rolltimeH*rolltimeW);
	dCorrelation44         /= (rolltimeH*rolltimeW);
	dLocalCalm44           /= (rolltimeH*rolltimeW);

	pGLCMFVar.dAveEnergy = dEnergy/4 ;
	pGLCMFVar.dAveEntropy = dEntropy/4;
	pGLCMFVar.dAveInertiaQuadrature = dInertiaQuadrature/4;
	pGLCMFVar.dAveCorrelation = dCorrelation/4;
	pGLCMFVar.dAveLocalCalm = dLocalCalm/4;

	pGLCMFVar.dVarEnergy=((dEnergy11-pGLCMFVar.dAveEnergy)*(dEnergy11-pGLCMFVar.dAveEnergy)
		+(dEnergy22-pGLCMFVar.dAveEnergy)*(dEnergy22-pGLCMFVar.dAveEnergy)
		+(dEnergy33-pGLCMFVar.dAveEnergy)*(dEnergy33-pGLCMFVar.dAveEnergy)
		+(dEnergy44-pGLCMFVar.dAveEnergy)*(dEnergy44-pGLCMFVar.dAveEnergy))/4;
	pGLCMFVar.dVarEntropy=((dEntropy11-pGLCMFVar.dAveEntropy)*(dEntropy11-pGLCMFVar.dAveEntropy)
		+(dEntropy22-pGLCMFVar.dAveEntropy)*(dEntropy22-pGLCMFVar.dAveEntropy)
		+(dEntropy33-pGLCMFVar.dAveEntropy)*(dEntropy33-pGLCMFVar.dAveEntropy)
		+(dEntropy44-pGLCMFVar.dAveEntropy)*(dEntropy44-pGLCMFVar.dAveEntropy))/4;
	pGLCMFVar.dVarInertiaQuadrature=((dInertiaQuadrature11-pGLCMFVar.dAveInertiaQuadrature)*(dInertiaQuadrature11-pGLCMFVar.dAveInertiaQuadrature)
		+(dInertiaQuadrature22-pGLCMFVar.dAveInertiaQuadrature)*(dInertiaQuadrature22-pGLCMFVar.dAveInertiaQuadrature)
		+(dInertiaQuadrature33-pGLCMFVar.dAveInertiaQuadrature)*(dInertiaQuadrature33-pGLCMFVar.dAveInertiaQuadrature)
		+(dInertiaQuadrature44-pGLCMFVar.dAveInertiaQuadrature)*(dInertiaQuadrature44-pGLCMFVar.dAveInertiaQuadrature))/4;
	pGLCMFVar.dVarCorrelation=((dCorrelation11-pGLCMFVar.dAveCorrelation)*(dCorrelation11-pGLCMFVar.dAveCorrelation)
		+(dCorrelation22-pGLCMFVar.dAveCorrelation)*(dCorrelation22-pGLCMFVar.dAveCorrelation)
		+(dCorrelation33-pGLCMFVar.dAveCorrelation)*(dCorrelation33-pGLCMFVar.dAveCorrelation)
		+(dCorrelation44-pGLCMFVar.dAveCorrelation)*(dCorrelation44-pGLCMFVar.dAveCorrelation))/4;
	pGLCMFVar.dVarLocalCalm=((dLocalCalm11-pGLCMFVar.dAveLocalCalm)*(dLocalCalm11-pGLCMFVar.dAveLocalCalm)
		+(dLocalCalm22-pGLCMFVar.dAveLocalCalm)*(dLocalCalm22-pGLCMFVar.dAveLocalCalm)
		+(dLocalCalm33-pGLCMFVar.dAveLocalCalm)*(dLocalCalm33-pGLCMFVar.dAveLocalCalm)
		+(dLocalCalm44-pGLCMFVar.dAveLocalCalm)*(dLocalCalm44-pGLCMFVar.dAveLocalCalm))/4;


	for(i=0; i<FilterWindowWidth; i++)
	{
		free(arLocalImage[i]) ;
		arLocalImage[i] = NULL;
	}
	free(arLocalImage);
	arLocalImage = NULL;


	return pGLCMFVar;
}

void ZBGLCM::SetNum(int iDepth)
{
	switch (iDepth)
	{
	case 8:
		m_iNum = 256;
		break;
	case 16:
		m_iNum = 65536;
		break;
	default:
		m_iNum = 0;
		break;
	}
}