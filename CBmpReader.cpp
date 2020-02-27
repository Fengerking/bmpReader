#include "CBmpReader.h"
#include "stdio.h"
#include "tchar.h"
#include <algorithm>
#include <cmath>

CBmpReader::CBmpReader() {
	m_pBmpFile = NULL;
	m_pBmpData = NULL;
	m_hMemDC = NULL;
	m_hBmpFile = NULL;
	m_hBmpData = NULL;
	memset(&m_headFile, 0, sizeof(m_headFile));
	memset(&m_headData, 0, sizeof(m_headData));

	int tempX[3][3] = { { 1, 0, -1 }, { 2, 0, -2 }, { 1, 0, -1 } };
	for (int i = 0; i<3; i++){
		for (int j = 0; j < 3; j++){
			m_nKernelX[i][j] = tempX[i][j];
		}
	}
	int tempY[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };
	for (int i = 0; i<3; i++){
		for (int j = 0; j < 3; j++){
			m_nKernelY[i][j] = tempY[i][j];
		}
	}
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			m_dKernelSmooth[i][j] = (double)1 / 9;
		}
	}
}

CBmpReader::~CBmpReader() {
	releaseBmp();
}

void CBmpReader::releaseBmp(void) {
	if (m_pBmpFile != NULL)
		delete[]m_pBmpFile;
	m_pBmpFile = NULL;
	if (m_pBmpData != NULL)
		delete[]m_pBmpData;
	m_pBmpData = NULL;
	if (m_hMemDC != NULL)
		DeleteDC(m_hMemDC);
	m_hMemDC = NULL;
	if (m_hBmpFile != NULL)
		DeleteObject(m_hBmpFile);
	m_hBmpFile = NULL;
	if (m_hBmpData != NULL)
		DeleteObject(m_hBmpData);
	m_hBmpData = NULL;
}

int	CBmpReader::OpenFile(const TCHAR * pBmpFile) {
	FILE * hFile = _wfopen(pBmpFile, _T("r"));
	if (hFile == NULL)
		return -1;
	releaseBmp();
	BITMAPFILEHEADER bmpFileInfo;
	memset(&bmpFileInfo, 0, sizeof(bmpFileInfo));
	fread(&bmpFileInfo, 1, sizeof(bmpFileInfo), hFile);
	memset(&m_headFile, 0, sizeof(m_headFile));
	fread(&m_headFile, 1, sizeof(m_headFile), hFile);
	m_pBmpFile = new BYTE[m_headFile.biSizeImage];
	fseek(hFile, bmpFileInfo.bfOffBits, SEEK_SET);
	fread(m_pBmpFile, 1, m_headFile.biSizeImage, hFile);
	fclose(hFile);

	memcpy(&m_headData, &m_headFile, sizeof(m_headData));
	m_headData.biBitCount = 32;
	m_headData.biSizeImage = m_headData.biWidth * m_headData.biHeight * 4;
	m_pBmpData = new unsigned char[m_headData.biSizeImage];
	memset(m_pBmpData, 0, m_headData.biSizeImage);
	unsigned char * pFile = m_pBmpFile;
	unsigned char * pData = m_pBmpData;
	int				nStride = m_headFile.biSizeImage / m_headFile.biHeight;
	int				nValue = 0;
	for (int h = 0; h < m_headFile.biHeight; h++) {
		pFile = m_pBmpFile + nStride * h;
		for (int w = 0; w < m_headFile.biWidth; w++) {
			nValue = (int)(0.11 * (*pFile++) + 0.59 * (*pFile++) + 0.3 * (*pFile++));
			*pData++ = nValue;// *pFile++; // B
			*pData++ = nValue;// *pFile++; // G
			*pData++ = nValue;// *pFile++; // R
			*pData++;			 // Alpha
		}
	}
	Convolute();
	return 0;
}

void CBmpReader::Convolute(){
	int ** pic = new int *[m_headData.biHeight + 2];
	for (int i = 0; i < m_headData.biHeight + 2; i++){
		pic[i] = new int[m_headData.biWidth + 2];
	}
	unsigned char * pData = m_pBmpData;
	for (int h = 0; h < m_headData.biHeight + 2; h++){
		for (int w = 0; w < m_headData.biWidth + 2; w++){
			if (h == 0 || h == m_headData.biHeight + 1 || w == 0 || w == m_headData.biWidth + 1){
				pic[h][w] = 0;
				continue;
			}
			pic[h][w] = *pData;
			pData += 4;
		}
	}
	int **	picResult = new int *[m_headData.biHeight];
	for (int i = 0; i < m_headData.biHeight; i++){
		picResult[i] = new int[m_headData.biWidth];
	}

	/*int temp;
	for (int h = 0; h < m_headData.biHeight; h++){
		for (int w = 0; w < m_headData.biWidth; w++){
			temp = 0;
			for (int i = 0; i < 3; i++){
				for (int j = 0; j < 3; j++){
					temp += m_dKernelSmooth[i][j] * pic[h + i][w + j];
				}
			}
			picResult[h][w] = temp;
		}
	}*/

	int temp1;
	int temp2;
	for (int h = 0; h < m_headData.biHeight; h++){
		for (int w = 0; w < m_headData.biWidth; w++){
			temp1 = 0;
			for (int i = 0; i < 3; i++){
				for (int j = 0; j < 3; j++){
					temp1 += m_nKernelX[i][j]*pic[h+i][w+j];
				}
			}
			temp2 = 0;
			for (int i = 0; i < 3; i++){
				for (int j = 0; j < 3; j++){
					temp2 += m_nKernelY[i][j] * pic[h + i][w + j];
				}
			}
			picResult[h][w] = pow((pow(temp1,2) + pow(temp2,2))/2,(double)1/2);
		}
	}

	unsigned char * pPicBuff = m_pBmpData;
	for (int h = 0; h < m_headData.biHeight; h++){
		for (int w = 0; w < m_headData.biWidth; w++){
			if (picResult[h][w] < 0)
				picResult[h][w] = 0;
			*pPicBuff++ = picResult[h][w];
			*pPicBuff++ = picResult[h][w];
			*pPicBuff++ = picResult[h][w];
			*pPicBuff++;
		}
	}

}

int CBmpReader::DrawBmp(HDC hDC, int nW, int nH) {
	if (m_pBmpFile == NULL)
		return -1;

	if (m_hMemDC == NULL) {
		m_hMemDC = CreateCompatibleDC(hDC);
		void * pBmpBuff = NULL;
		m_hBmpFile = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_headFile, DIB_RGB_COLORS, (void **)&pBmpBuff, NULL, 0);
		if (pBmpBuff == NULL)
			return -1;
		memcpy(pBmpBuff, m_pBmpFile, m_headFile.biSizeImage);
		m_hBmpData = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_headData, DIB_RGB_COLORS, (void **)&pBmpBuff, NULL, 0);
		if (pBmpBuff == NULL)
			return -1;
		memcpy(pBmpBuff, m_pBmpData, m_headData.biSizeImage);

	}
	SetStretchBltMode(hDC, HALFTONE);
	SelectObject(m_hMemDC, m_hBmpFile);
	StretchBlt(hDC, 0, 0, nW/2, nH, m_hMemDC, 0, 0, m_headFile.biWidth, m_headFile.biHeight, SRCCOPY);

	SelectObject(m_hMemDC, m_hBmpData);
	StretchBlt(hDC, nW/2, 0, nW/2, nH, m_hMemDC, 0, 0, m_headData.biWidth, m_headData.biHeight, SRCCOPY);

	return 0;
}
