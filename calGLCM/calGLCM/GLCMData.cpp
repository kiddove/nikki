#include "StdAfx.h"
#include "GLCMData.h"

GLCMData::GLCMData(void)
{
	m_pData = NULL;
	m_strFilePath.Empty();
	memset(&m_imgInfo, 0, sizeof(ImgInfo));
}

GLCMData::~GLCMData(void)
{
	if (m_pData != NULL)
	{
		delete m_pData;
		m_pData = NULL;
	}
}

bool GLCMData::LoadFromFile(LPTSTR strFilePath)
{
	GDALDataset *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(strFilePath, GA_ReadOnly);

	if (poDataset == NULL)
	{
		CString strErr;
		strErr.Format(_T("open file failed[%s]"), strFilePath);
		AfxMessageBox(strErr);
		//f.Close();
		return false;
	}

	int iRasterCount = poDataset->GetRasterCount();
	double adfGeoTransform[6];

	TRACE(_T("Drivers: %s/ %s\n"), poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

	if (poDataset->GetProjectionRef() != NULL)
	{
		TRACE(_T("Projection is \"{%s}\"\n"), poDataset->GetProjectionRef());
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		TRACE(_T("Origin(left, top) = (%.2f, %.2f)\n"), adfGeoTransform[0], adfGeoTransform[3]);
		TRACE(_T("Pixel Size = (%.2f, %.2f)\n"), adfGeoTransform[1], adfGeoTransform[5]);
	}

	///
	// fetch a raster band

	GDALRasterBand *poBand;

	int iBlockXSize = poDataset->GetRasterXSize();
	int iBlockYSize = poDataset->GetRasterYSize();
	int bGotMin, bGotMax;//, iBlockXSize, iBlockYSize;
	double adfMinMax[2];

	poBand = poDataset->GetRasterBand(1);
	GDALDataType iType = poBand->GetRasterDataType();

	poBand->GetBlockSize(&iBlockXSize, &iBlockYSize);

	TRACE(_T("Block=%d x %d, Type=%s, ColorType=%s\n"), iBlockXSize, iBlockYSize,
		GDALGetDataTypeName(poBand->GetRasterDataType()), GDALGetColorInterpretationName(poBand->GetColorInterpretation()));

	adfMinMax[0] = poBand->GetMinimum(&bGotMin);
	adfMinMax[1] = poBand->GetMaximum(&bGotMax);

	if (!(bGotMin && bGotMax))
	{
		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
	}

	TRACE(_T("Min=%.2f, Max=%.2f\n"), adfMinMax[0], adfMinMax[1]);

	if (poBand->GetOverviewCount() > 0)
		TRACE(_T("Band has %d overviews.\n"), poBand->GetOverviewCount());

	if (poBand->GetColorTable() != NULL)
		TRACE(_T("Band has a color table with %d entries.\n"), poBand->GetColorTable()->GetColorEntryCount());

	int iSize = 1;

	switch (iType)
	{
	case GDT_Byte:
		iSize = 1;
		break;
	case GDT_UInt16:
	case GDT_Int16:
		iSize = 2;
		break;
	default:
		break;
	}

	// reading raster data
	char *pafScanline;
	int nXSize = poBand->GetXSize();
	int nYSize = poBand->GetYSize();

	m_imgInfo.iType = iType;
	m_imgInfo.iWidth = nXSize;
	m_imgInfo.iHeight = nYSize;

	int iCount = sizeof(char)*nXSize*nYSize*iSize;
	pafScanline = (char*)CPLMalloc(iCount);
	poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, iType, 0, 0);

	if (m_pData != NULL)
	{
		delete m_pData;
		m_pData = NULL;
	}

	m_pData = new char [iCount];
	memcpy(m_pData, pafScanline, iCount);

	///
	m_strFilePath = strFilePath;
	GDALClose(poDataset);
	return true;
}

bool GLCMData::calGLCM(CalCondition& calCon)
{
	if (m_pData == NULL)
		return false;

	memcpy(&m_calCon, &calCon, sizeof(CalCondition));
	switch (m_imgInfo.iType)
	{
	case GDT_Byte:
		calGLCM8();
		break;
	case GDT_UInt16:
		calGLCM16();
		break;
	default:
		break;
	}
	return true;
}

void GLCMData::calGLCM8()
{
	// unsigned char
	int xroll = m_imgInfo.iWidth / m_calCon.x;
	if (m_imgInfo.iWidth % m_calCon.x != 0)
		xroll += 1;

	int yroll = m_imgInfo.iHeight / m_calCon.y;
	if (m_imgInfo.iHeight % m_calCon.y != 0)
		yroll += 1;

	// step 1
	for (int i = 0; i < m_imgInfo.iWidth; i++)
	{
		for (int j =0; j < m_imgInfo.iHeight; j++)
		{
			m_pData[i * m_imgInfo.iHeight + j] = m_pData[i * m_imgInfo.iHeight + j] * m_calCon.graynum / 256 + 1;
		}
	}


	// 补齐
	unsigned char* pData = new unsigned char [xroll * m_calCon.x * yroll * m_calCon.y];

	for (int i = 0; i < xroll * m_calCon.x; i++)
	{
		for (int j = 0; j < yroll * m_calCon.y; j++)
		{
			if (i < m_imgInfo.iWidth && j < m_imgInfo.iHeight)
				pData[j * m_imgInfo.iWidth + i] = m_pData[j * m_imgInfo.iWidth + i];
			else
				pData[j * m_imgInfo.iWidth + i] = 1;
		}
	}

	// step2
	int* pGLCM = new int [m_calCon.graynum * m_calCon.graynum];
	memset(pGLCM, 0, m_calCon.graynum * m_calCon.graynum);

	if (m_calCon.angle == 0)
	{
		// 
		for (int i = 0; i < xroll; i++)
		{
			for (int j = 0; j < yroll; j++)
			{
				for (int p = 0; p < m_calCon.x; p++)
				{
					for (int q =0; q < m_calCon.y; q++)
					{
						if (i * m_calCon.x + p < m_imgInfo.iWidth - m_calCon.distance)
						{
							int x = pData[(j * m_calCon.y + q) * m_imgInfo.iWidth + i * m_calCon.x + p];
							int y = pData[(j * m_calCon.y + q) * m_imgInfo.iWidth + i * m_calCon.x + p + m_calCon.distance];
							pGLCM[(x - 1) * m_calCon.graynum + y - 1] += 1;
						}
					}
				}
			}
		}
	}
	else if (m_calCon.angle == 45)
	{
	}
	else if (m_calCon.angle == 90)
	{
	}
	else if (m_calCon.angle == 135)
	{
	}


	delete[] pGLCM;
	delete[] pData;
}

void GLCMData::calGLCM16()
{
	// unsigned short
	int xroll = m_imgInfo.iWidth / m_calCon.x;
	if (m_imgInfo.iWidth % m_calCon.x != 0)
		xroll += 1;

	int yroll = m_imgInfo.iHeight / m_calCon.y;
	if (m_imgInfo.iHeight % m_calCon.y != 0)
		yroll += 1;

	unsigned short* pTemp = (unsigned short*)m_pData;
	// step 1
	// normalize
	for (int i = 0; i < m_imgInfo.iHeight; i++)
	{
		for (int j = 0; j < m_imgInfo.iWidth; j++)
		{
			pTemp[i * m_imgInfo.iWidth + j] = pTemp[i * m_imgInfo.iWidth + j] * m_calCon.graynum / 65536 + 1;
		}
	}


	// 补齐
	unsigned short* pData = new unsigned short [xroll * m_calCon.x * yroll * m_calCon.y * sizeof (unsigned short)];

	for (int i = 0; i < yroll * m_calCon.y; i++)
	{
		for (int j = 0; j < xroll * m_calCon.x; j++)
		{
			if (i < m_imgInfo.iHeight && j < m_imgInfo.iWidth)
				pData[i * yroll * m_calCon.y + j] = pTemp[i * m_imgInfo.iWidth + j];
			else
				pData[i * yroll * m_calCon.y + j] = 1;
		}
	}

	// step2
	int* pGLCM = new int [m_calCon.graynum * m_calCon.graynum * sizeof(int)];

	///
	TRACE(_T("\n10 * 10:\n"));
	for (int i = 0; i < 10; i++)
	{		
		for (int j = 0; j < 10; j++)
		{
			TRACE(_T("%d, "),  pData[i * yroll * m_calCon.y + j]);
		}
		TRACE(_T("\n"));
	}
	///
	if (m_calCon.angle == 0)
	{
		// 
		for (int i = 0; i < yroll; i++)	//height
		{
			for (int j = 0; j < xroll; j++)	//width
			{
				memset(pGLCM, 0, m_calCon.graynum * m_calCon.graynum * sizeof(int));
				for (int p = 0; p < m_calCon.y; p++)	//height
				{
					for (int q =0; q < m_calCon.x; q++)	// width
					{
						if (q + m_calCon.distance <= m_calCon.x && (j * m_calCon.x + q))
						if (i * m_calCon.y + p < m_imgInfo.iWidth - m_calCon.distance)
						{
							int x = pData[(i * m_calCon.y + p) * yroll * m_calCon.y + j * m_calCon.x + q];
							int y = pData[(i * m_calCon.y + p) * yroll * m_calCon.y + j * m_calCon.x + q + m_calCon.distance];
							pGLCM[(x - 1) * m_calCon.graynum + y - 1] += 1;
							TRACE(_T("[%d, %d] --- %d, %d\n"), p, q, x, y);
						}
					}
				}

				TRACE(_T("\nOriginal:\n"));
				for (int ii = 0; ii < m_calCon.graynum; ii++)  
				{  
					for (int jj = 0; jj < m_calCon.graynum; jj++)  
					{
						TRACE(_T("%d, "), pGLCM[ii * m_calCon.graynum + jj]);
					}
					TRACE(_T("\n"));
				}
				//转置
				int* pGLCMT = new int [m_calCon.graynum * m_calCon.graynum * sizeof(int)];
				memset(pGLCMT, 0, m_calCon.graynum * m_calCon.graynum * sizeof(int));
				TRACE(_T("\nTransport:\n"));
				for (int ii = 0; ii < m_calCon.graynum; ii++)  
				{  
					for (int jj = 0; jj < m_calCon.graynum; jj++)  
					{
						pGLCMT[ii * m_calCon.graynum + jj] = pGLCM[jj * m_calCon.graynum + ii];
						TRACE(_T("%d, "), pGLCMT[ii * m_calCon.graynum + jj]);
					}
					TRACE(_T("\n"));
				}
				int sum = 0;
				TRACE(_T("\nfinal:\n"));
				for (int ii = 0; ii < m_calCon.graynum; ii++)  
				{  
					for (int jj = 0; jj < m_calCon.graynum; jj++)  
					{
						pGLCM[ii * m_calCon.graynum + jj] += pGLCMT[ii * m_calCon.graynum + jj];
						sum += pGLCM[ii * m_calCon.graynum + jj];
						TRACE(_T("%d, "), pGLCM[ii * m_calCon.graynum + jj]);
					}
					TRACE(_T("\n"));
				}

				//计算特征值  
				double entropy = 0,energy = 0,contrast = 0,homogenity = 0, meanH = 0, meanV = 0, varianceH = 0, varianceV = 0, corelation_t = 0, corelation = 0, dissimilarity = 0;  
				TRACE(_T("\ndValue:\n"));
				for (int ii = 0; ii < m_calCon.graynum; ii++)  
				{
					for (int jj = 0; jj < m_calCon.graynum; jj++)  
					{  
						double dValue = (double)pGLCM[ii * m_calCon.graynum + jj] / (double)sum;
						TRACE(_T("%.3f, "), dValue);
						//熵  
						if(pGLCM[ii * m_calCon.graynum + jj] > 0)  
							entropy -= dValue * log(dValue);  
						//能量  
						energy += dValue * dValue;  
						//对比度  
						contrast += (ii - jj) * (ii - jj) * dValue;  
						//一致性  
						homogenity += 1.0 / (1 + (ii - jj) * (ii - jj)) * dValue;
						meanH += ii * dValue;
						meanV += jj * dValue;

						varianceH += (ii - meanH) * (ii - meanH) * dValue;

						varianceV += (ii - meanV) * (ii - meanV) * dValue;

						corelation_t += (ii * jj) * dValue - (meanH * meanV);

						dissimilarity += dValue * abs(ii - jj);
						corelation = corelation_t / (varianceH * varianceV);
					}  
					TRACE(_T("\n"));

				}


				//// varianceH
				//for (int ii = 0; ii < m_calCon.graynum; ii++)  
				//{  
				//	for (int jj = 0; jj < m_calCon.graynum; jj++)  
				//	{  
				//		double dValue = (double)pGLCM[ii * m_calCon.graynum + jj] / (double)sum;
				//		TRACE(_T("%.3f, "), dValue);

				//		varianceH += (ii - meanH) * (ii - meanH) * dValue;

				//		varianceV += (ii - meanV) * (ii - meanV) * dValue;

				//		//corelation_t += (ii * jj) * dValue - (meanH * meanV);

				//		//dissimilarity += dValue * abs(ii - jj);
				//	}  
				//	TRACE(_T("\n"));
				//}
				//corelation = corelation_t / (varianceH * varianceV);
				TRACE(_T("[%d, %d] sum: %d, 熵entropy(): %.3f, 能量(asm): %.3f, 对比度(contrast): %.3f, 一致性(homogenity): %.3f, meanH: %.3f, meanV: %.3f, varianceH: %.3f, varianceV: %.3f, corelation: %.3f, dissimilarity: %.3f\n")
					, i, j, sum, entropy, energy, contrast, homogenity, meanH, meanV, varianceH, varianceV, corelation, dissimilarity);
			}
		}
	}
	else if (m_calCon.angle == 45)
	{
	}
	else if (m_calCon.angle == 90)
	{
	}
	else if (m_calCon.angle == 135)
	{
	}

	//
	delete[] pGLCM;
	delete[] pData;

}