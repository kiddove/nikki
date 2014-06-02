#include "stdafx.h"
#include "opencv\cv.h"
//#include <opencv2\highgui\highgui_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#define GLCM_DIS 1  //�Ҷȹ��������ͳ�ƾ���  
#define GLCM_CLASS 8 //����Ҷȹ��������ͼ��Ҷ�ֵ�ȼ���  
#define GLCM_ANGLE_HORIZATION 0  //ˮƽ  
#define GLCM_ANGLE_VERTICAL   1  //��ֱ  
#define GLCM_ANGLE_DIGONAL    2  //�Խ�  

const char* filename = "E:\\Code\\Github\\nikki\\GLCM\\s41.tif"; 
const CString output = "E:\\Code\\Github\\nikki\\GLCM\\result.txt";

int calGLCM(IplImage* bWavelet,int angleDirection,double* featureVector)  
{  
    int i,j;  
    int width,height;  
  
    if(NULL == bWavelet)  
        return 1;  
  
    width = bWavelet->width;  
    height = bWavelet->height;  
  
    int * glcm = new int[GLCM_CLASS * GLCM_CLASS];  
    int * histImage = new int[width * height];  
  
    if(NULL == glcm || NULL == histImage)  
        return 2;  
  
    //�Ҷȵȼ���---��GLCM_CLASS���ȼ�  
    uchar *data =(uchar*) bWavelet->imageData;  
	unsigned short *datas = (unsigned short*)bWavelet->imageData;
	float *dataf = (float*)bWavelet->imageData;
    for(i = 0;i < height;i++){  
        for(j = 0;j < width;j++){  
            histImage[i * width + j] = (int)(data[bWavelet->widthStep * i + j] * GLCM_CLASS / 256);  
        }  
    }  
  
    //��ʼ����������  
    for (i = 0;i < GLCM_CLASS;i++)  
        for (j = 0;j < GLCM_CLASS;j++)  
            glcm[i * GLCM_CLASS + j] = 0;  
  
    //����Ҷȹ�������  
    int w,k,l;  
    //ˮƽ����  
    if(angleDirection == GLCM_ANGLE_HORIZATION)  
    {  
        for (i = 0;i < height;i++)  
        {  
            for (j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
                if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width)  
                {  
                    k = histImage[i * width + j + GLCM_DIS];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
                if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width)  
                {  
                    k = histImage[i * width + j - GLCM_DIS];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
            }  
        }  

		CStdioFile f;
		f.Open(output, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);
		CString str1, str2;

		f.WriteString("RD\n");

		for (int i = 0; i < GLCM_CLASS; i++)
		{
			str1.Empty();
			for (int j =0 ; j < GLCM_CLASS; j++)
			{
				str2.Empty();
				str2.Format(_T("%d, "), glcm[i * GLCM_CLASS + j]);
				str1 += str2;
			}
			str1 += _T("\n");
			f.WriteString(str1);
		}

		f.Close();

    }  
    //��ֱ����  
    else if(angleDirection == GLCM_ANGLE_VERTICAL)  
    {  
        for (i = 0;i < height;i++)  
        {  
            for (j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
                if(i + GLCM_DIS >= 0 && i + GLCM_DIS < height)   
                {  
                    k = histImage[(i + GLCM_DIS) * width + j];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
                if(i - GLCM_DIS >= 0 && i - GLCM_DIS < height)   
                {  
                    k = histImage[(i - GLCM_DIS) * width + j];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
            }  
        }  
    }  
    //�ԽǷ���  
    else if(angleDirection == GLCM_ANGLE_DIGONAL)  
    {  
        for (i = 0;i < height;i++)  
        {  
            for (j = 0;j < width;j++)  
            {  
                l = histImage[i * width + j];  
  
                if(j + GLCM_DIS >= 0 && j + GLCM_DIS < width && i + GLCM_DIS >= 0 && i + GLCM_DIS < height)  
                {  
                    k = histImage[(i + GLCM_DIS) * width + j + GLCM_DIS];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
                if(j - GLCM_DIS >= 0 && j - GLCM_DIS < width && i - GLCM_DIS >= 0 && i - GLCM_DIS < height)  
                {  
                    k = histImage[(i - GLCM_DIS) * width + j - GLCM_DIS];  
                    glcm[l * GLCM_CLASS + k]++;  
                }  
            }  
        }  
    }  
  
    //��������ֵ  
    double entropy = 0,energy = 0,contrast = 0,homogenity = 0;  
    for (i = 0;i < GLCM_CLASS;i++)  
    {  
        for (j = 0;j < GLCM_CLASS;j++)  
        {  
            //��  
            if(glcm[i * GLCM_CLASS + j] > 0)  
                entropy -= glcm[i * GLCM_CLASS + j] * log10(double(glcm[i * GLCM_CLASS + j]));  
            //����  
            energy += glcm[i * GLCM_CLASS + j] * glcm[i * GLCM_CLASS + j];  
            //�Աȶ�  
            contrast += (i - j) * (i - j) * glcm[i * GLCM_CLASS + j];  
            //һ����  
            homogenity += 1.0 / (1 + (i - j) * (i - j)) * glcm[i * GLCM_CLASS + j];  
        }  
    }  
    //��������ֵ  
    i = 0;  
    featureVector[i++] = entropy;  
    featureVector[i++] = energy;  
    featureVector[i++] = contrast;  
    featureVector[i++] = homogenity;  
  
    delete[] glcm;  
    delete[] histImage;  
    return 0;  
}  

void main()
{
	//IplImage* dst = cvLoadImage(filename,CV_LOAD_IMAGE_ANYCOLOR);	
	IplImage* dst = cvLoadImage(filename,2);	
	double featureVector[10] = {0};

	//int calGLCM(IplImage* bWavelet,int angleDirection,double* featureVector)  
	calGLCM(dst, GLCM_ANGLE_HORIZATION, featureVector);

}